// Header
#include "Modbus.h"

// Definitions
//
#define MODBUS_MAX_FRAME_SIZE			256

#define MODBUS_FC_READ_HOLDING_REGS		0x03
#define MODBUS_FC_WRITE_SINGLE_REG		0x06
#define MODBUS_FC_WRITE_MULTIPLE_REGS	0x10

#define MODBUS_MIN_FRAME_SIZE			5
#define MODBUS_CHAR_BITS				11
#define MODBUS_FRAME_GAP_MIN_TICKS		2
#define MODBUS_TIME_TICK_US				1000

// Variables
//
typedef struct __ModbusInterface
{
	ModbusFunc_SendByte			IO_SendByte;
	ModbusFunc_GetBytesToReceive	IO_GetBytesToReceive;
	ModbusFunc_ReceiveByte		IO_ReceiveByte;
	ModbusFunc_SetTxMode		IO_SetTxMode;
	Int32U						BaudRate;
	Int16U						FrameGap;
	Int16U						ResponseTimeout;
	volatile Int64U				*pTimeCounter;
	Int8U						LastExceptionCode;
} ModbusInterface, *pModbusInterface;

static ModbusInterface Interface;

// Forward functions
//
static Int16U Modbus_CalcFrameGapTicks(Int32U BaudRate);
static void Modbus_SendBuffer(pInt8U Buffer, Int16U Length);
static void Modbus_WaitFrameGap();
static ModbusError Modbus_ReceiveFrame(pInt8U Buffer, Int16U BufferSize, pInt16U Length);

// Functions
//
// Инициализация интерфейса Modbus и сохранение указателей на функции обмена
void Modbus_Init(ModbusFunc_SendByte SendByte, ModbusFunc_GetBytesToReceive GetBytesToReceive,
		ModbusFunc_ReceiveByte ReceiveByte, ModbusFunc_SetTxMode SetTxMode,
		Int32U BaudRate, volatile Int64U *pTimeCounter, Int16U ResponseTimeoutTicks)
{
	Interface.IO_SendByte = SendByte;
	Interface.IO_GetBytesToReceive = GetBytesToReceive;
	Interface.IO_ReceiveByte = ReceiveByte;
	Interface.IO_SetTxMode = SetTxMode;
	Interface.BaudRate = BaudRate;
	Interface.pTimeCounter = pTimeCounter;
	Interface.ResponseTimeout = ResponseTimeoutTicks;
	Interface.LastExceptionCode = 0;
	Interface.FrameGap = Modbus_CalcFrameGapTicks(BaudRate);
}
// ----------------------------------------

// Расчёт межкадровой паузы Modbus RTU в тиках внутреннего таймера
static Int16U Modbus_CalcFrameGapTicks(Int32U BaudRate)
{
	Int32U CharTimeUs, T35Us;
	Int16U Ticks;

	if(BaudRate == 0)
		return MODBUS_FRAME_GAP_MIN_TICKS;

	CharTimeUs = (MODBUS_CHAR_BITS * 1000000UL) / BaudRate;
	T35Us = (CharTimeUs * 35UL) / 10UL;
	Ticks = (Int16U)((T35Us + MODBUS_TIME_TICK_US - 1) / MODBUS_TIME_TICK_US);

	if(Ticks < MODBUS_FRAME_GAP_MIN_TICKS)
		Ticks = MODBUS_FRAME_GAP_MIN_TICKS;

	return Ticks;
}
// ----------------------------------------

// Расчёт контрольной суммы CRC16 Modbus для буфера данных
Int16U Modbus_CRC16(pInt8U Data, Int16U Length)
{
	Int16U crc = 0xFFFF;
	Int16U i, j;

	for(i = 0; i < Length; ++i)
	{
		crc ^= Data[i];
		for(j = 0; j < 8; ++j)
		{
			if(crc & 0x0001)
				crc = (crc >> 1) ^ 0xA001;
			else
				crc >>= 1;
		}
	}

	return crc;
}
// ----------------------------------------

// Проверка CRC у принятого Modbus кадра
Boolean Modbus_CheckCRC(pInt8U Buffer, Int16U Length)
{
	Int16U CrcReceived;

	if(Length < MODBUS_MIN_FRAME_SIZE)
		return FALSE;

	CrcReceived = Buffer[Length - 2] | ((Int16U)Buffer[Length - 1] << 8);
	return Modbus_CRC16(Buffer, Length - 2) == CrcReceived;
}
// ----------------------------------------

// Упаковка запроса чтения holding-регистров с добавлением CRC
Int16U Modbus_BuildReadHoldingRegs(Int8U Slave, Int16U Address, Int16U Count, pInt8U Buffer)
{
	Int16U Length = 6;
	Int16U crc;

	if(Buffer == NULL || Count == 0 || Count > 125)
		return 0;

	Buffer[0] = Slave;
	Buffer[1] = MODBUS_FC_READ_HOLDING_REGS;
	Buffer[2] = (Int8U)(Address >> 8);
	Buffer[3] = (Int8U)(Address & 0xFF);
	Buffer[4] = (Int8U)(Count >> 8);
	Buffer[5] = (Int8U)(Count & 0xFF);

	crc = Modbus_CRC16(Buffer, Length);
	Buffer[Length++] = (Int8U)(crc & 0xFF);
	Buffer[Length++] = (Int8U)(crc >> 8);

	return Length;
}
// ----------------------------------------

// Упаковка запроса записи одного регистра с добавлением CRC
Int16U Modbus_BuildWriteSingleReg(Int8U Slave, Int16U Address, Int16U Value, pInt8U Buffer)
{
	Int16U Length = 6;
	Int16U crc;

	if(Buffer == NULL)
		return 0;

	Buffer[0] = Slave;
	Buffer[1] = MODBUS_FC_WRITE_SINGLE_REG;
	Buffer[2] = (Int8U)(Address >> 8);
	Buffer[3] = (Int8U)(Address & 0xFF);
	Buffer[4] = (Int8U)(Value >> 8);
	Buffer[5] = (Int8U)(Value & 0xFF);

	crc = Modbus_CRC16(Buffer, Length);
	Buffer[Length++] = (Int8U)(crc & 0xFF);
	Buffer[Length++] = (Int8U)(crc >> 8);

	return Length;
}
// ----------------------------------------

// Упаковка запроса записи нескольких регистров с добавлением CRC
Int16U Modbus_BuildWriteMultipleRegs(Int8U Slave, Int16U Address, Int16U Count, pInt16U Values, pInt8U Buffer)
{
	Int16U i, Length, ByteCount, crc;

	if(Buffer == NULL || Values == NULL || Count == 0 || Count > 123)
		return 0;

	ByteCount = Count * 2;
	Length = 7 + ByteCount;

	if(Length + 2 > MODBUS_MAX_FRAME_SIZE)
		return 0;

	Buffer[0] = Slave;
	Buffer[1] = MODBUS_FC_WRITE_MULTIPLE_REGS;
	Buffer[2] = (Int8U)(Address >> 8);
	Buffer[3] = (Int8U)(Address & 0xFF);
	Buffer[4] = (Int8U)(Count >> 8);
	Buffer[5] = (Int8U)(Count & 0xFF);
	Buffer[6] = (Int8U)ByteCount;

	for(i = 0; i < Count; ++i)
	{
		Buffer[7 + i * 2] = (Int8U)(Values[i] >> 8);
		Buffer[8 + i * 2] = (Int8U)(Values[i] & 0xFF);
	}

	crc = Modbus_CRC16(Buffer, Length);
	Buffer[Length++] = (Int8U)(crc & 0xFF);
	Buffer[Length++] = (Int8U)(crc >> 8);

	return Length;
}
// ----------------------------------------

// Проверка адреса, кода функции и CRC у ответа slave-устройства
ModbusError Modbus_ValidateResponse(pInt8U Buffer, Int16U Length, Int8U ExpectedSlave, Int8U ExpectedFunction)
{
	if(Length < MODBUS_MIN_FRAME_SIZE)
		return MODBUS_ERR_FRAME_BREAK;

	if(!Modbus_CheckCRC(Buffer, Length))
		return MODBUS_ERR_BAD_CRC;

	if(Buffer[0] != ExpectedSlave)
		return MODBUS_ERR_WRONG_SLAVE;

	if(Buffer[1] == (Int8U)(ExpectedFunction | 0x80))
	{
		Interface.LastExceptionCode = Buffer[2];

		return MODBUS_ERR_SLAVE_EXCEPTION;
	}

	if(Buffer[1] != ExpectedFunction)
		return MODBUS_ERR_WRONG_FUNCTION;

	return MODBUS_OK;
}
// ----------------------------------------

// Отправка Modbus запроса и приём ответа с базовой обработкой ошибок
ModbusError Modbus_SendReceive(pInt8U TxBuffer, Int16U TxLength, pInt8U RxBuffer, Int16U RxBufferSize, pInt16U RxLength)
{
	ModbusError ErrorCode;

	if(Interface.IO_SendByte == NULL || Interface.IO_GetBytesToReceive == NULL
			|| Interface.IO_ReceiveByte == NULL || Interface.pTimeCounter == NULL || TxBuffer == NULL || RxBuffer == NULL
			|| RxLength == NULL || TxLength < MODBUS_MIN_FRAME_SIZE)
		return MODBUS_ERR_BAD_INIT;

	Interface.LastExceptionCode = 0;

	while(Interface.IO_GetBytesToReceive())
		Interface.IO_ReceiveByte();

	Modbus_WaitFrameGap();
	Modbus_SendBuffer(TxBuffer, TxLength);
	Modbus_WaitFrameGap();

	ErrorCode = Modbus_ReceiveFrame(RxBuffer, RxBufferSize, RxLength);
	if(ErrorCode != MODBUS_OK)
		return ErrorCode;

	return Modbus_ValidateResponse(RxBuffer, *RxLength, TxBuffer[0], TxBuffer[1]);
}
// ----------------------------------------

// Получение кода последнего exception-ответа slave
Int8U Modbus_GetLastExceptionCode()
{
	return Interface.LastExceptionCode;
}
// ----------------------------------------

// Передача буфера в линию с переключением RS485 в режим передачи
static void Modbus_SendBuffer(pInt8U Buffer, Int16U Length)
{
	Int16U i;

	if(Interface.IO_SetTxMode != NULL)
		Interface.IO_SetTxMode(TRUE);

	for(i = 0; i < Length; ++i)
		Interface.IO_SendByte(Buffer[i]);

	if(Interface.IO_SetTxMode != NULL)
		Interface.IO_SetTxMode(FALSE);
}
// ----------------------------------------

// Выдержка межкадровой паузы по внутреннему таймеру
static void Modbus_WaitFrameGap()
{
	Int64U StartTime = *Interface.pTimeCounter;

	while(*Interface.pTimeCounter - StartTime < Interface.FrameGap);
}
// ----------------------------------------

// Приём Modbus кадра до паузы на линии или до таймаута ожидания
static ModbusError Modbus_ReceiveFrame(pInt8U Buffer, Int16U BufferSize, pInt16U Length)
{
	Int64U StartTime = *Interface.pTimeCounter;
	Int64U LastByteTime = 0;
	Int16U Received = 0;
	Boolean FrameStarted = FALSE;

	*Length = 0;

	while(*Interface.pTimeCounter - StartTime <= Interface.ResponseTimeout)
	{
		while(Interface.IO_GetBytesToReceive())
		{
			if(Received >= BufferSize)
				return MODBUS_ERR_BUFFER_OVERFLOW;

			Buffer[Received++] = (Int8U)Interface.IO_ReceiveByte();
			LastByteTime = *Interface.pTimeCounter;
			FrameStarted = TRUE;
		}

		if(FrameStarted && (*Interface.pTimeCounter - LastByteTime) >= Interface.FrameGap)
			break;
	}

	*Length = Received;

	if(Received == 0)
		return MODBUS_ERR_TIMEOUT;

	if(Received < MODBUS_MIN_FRAME_SIZE)
		return MODBUS_ERR_FRAME_BREAK;

	if(!FrameStarted || (*Interface.pTimeCounter - LastByteTime) < Interface.FrameGap)
		return MODBUS_ERR_FRAME_BREAK;

	return MODBUS_OK;
}
// ----------------------------------------
