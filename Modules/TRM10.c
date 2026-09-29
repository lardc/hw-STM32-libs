// Header
#include "TRM10.h"

// Includes
#include "Modbus.h"

// Definitions
#define TRM10_FRAME_BUFFER_SIZE		32

#define TRM10_REG_FUN1					0x1009
#define TRM10_REG_SP1					0x100B
#define TRM10_REG_OUT_P					0x100F
#define TRM10_REG_CTRL					0x1011

#define TRM10_CTRL_STOP					0
#define TRM10_CTRL_RUN					1

#define TRM10_FLOAT_REG_COUNT			2
#define TRM10_FLOAT_BYTE_COUNT			4
#define TRM10_UINT16_REG_COUNT			1
#define TRM10_UINT16_BYTE_COUNT			2
#define TRM10_FC03_FLOAT_RESPONSE_SIZE	9
#define TRM10_FC03_UINT16_RESPONSE_SIZE	7
#define TRM10_FC16_RESPONSE_SIZE		8

// Forward functions
static TRMError TRM10_MapModbusError(ModbusError error);
static float TRM10_UnpackFloat32(pInt8U Buffer);
static void TRM10_PackFloat32(float Value, pInt16U Registers);

// Functions
//
// Преобразование кода ошибки Modbus в код ошибки TRM
static TRMError TRM10_MapModbusError(ModbusError error)
{
	ModbusError ModbusErrorCode = error;

	switch(ModbusErrorCode)
	{
		case MODBUS_OK:
			return TRME_None;
		case MODBUS_ERR_TIMEOUT:
			return TRME_ResponseTimeout;
		case MODBUS_ERR_BUFFER_OVERFLOW:
			return TRME_InputBufferOverrun;
		case MODBUS_ERR_BAD_CRC:
			return TRME_CheckSumError;
		default:
			return TRME_WrongResponse;
	}
}
// ----------------------------------------

// Распаковка FLOAT32
static float TRM10_UnpackFloat32(pInt8U Buffer)
{
	union
	{
		Int32U u;
		float f;
	} Value;

	Value.u = ((Int32U)Buffer[2] << 24)
			| ((Int32U)Buffer[3] << 16)
			| ((Int32U)Buffer[0] << 8)
			| (Int32U)Buffer[1];

	return Value.f;
}
// ----------------------------------------

// Упаковка FLOAT32
static void TRM10_PackFloat32(float Value, pInt16U Registers)
{
	union
	{
		Int32U u;
		float f;
	} Packed;

	Packed.f = Value;
	Registers[0] = (Int16U)(Packed.u & 0xFFFF);
	Registers[1] = (Int16U)(Packed.u >> 16);
}
// ----------------------------------------

// Чтение FLOAT32 из holding-регистра по FC03
float TRM10_ReadRegFloat(Int8U Slave, Int16U RegAddress, pTRMError error)
{
	Int8U TransmitBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int8U ReceiveBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int16U TransmitLength, ReceiveLength;
	ModbusError ModbusErrorCode;

	TransmitLength = Modbus_BuildReadHoldingRegs(Slave, RegAddress, TRM10_FLOAT_REG_COUNT, TransmitBuffer);
	if(TransmitLength == 0)
	{
		*error = TRME_WrongResponse;
		return 0.0f;
	}

	ModbusErrorCode = Modbus_SendReceive(TransmitBuffer, TransmitLength, ReceiveBuffer, TRM10_FRAME_BUFFER_SIZE, &ReceiveLength);
	*error = TRM10_MapModbusError(ModbusErrorCode);
	if(*error != TRME_None)
		return 0.0f;

	if(ReceiveLength < TRM10_FC03_FLOAT_RESPONSE_SIZE || ReceiveBuffer[2] != TRM10_FLOAT_BYTE_COUNT)
	{
		*error = TRME_WrongResponse;
		return 0.0f;
	}

	return TRM10_UnpackFloat32(&ReceiveBuffer[3]);
}
// ----------------------------------------

// Запись FLOAT32 в holding-регистр по FC16
Boolean TRM10_WriteRegFloat(Int8U Slave, Int16U RegAddress, float Value, pTRMError error)
{
	Int16U Registers[TRM10_FLOAT_REG_COUNT];
	Int8U TransmitBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int8U ReceiveBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int16U TransmitLength, ReceiveLength;
	ModbusError ModbusErrorCode;

	TRM10_PackFloat32(Value, Registers);

	TransmitLength = Modbus_BuildWriteMultipleRegs(Slave, RegAddress, TRM10_FLOAT_REG_COUNT, Registers, TransmitBuffer);
	if(TransmitLength == 0)
	{
		*error = TRME_WrongResponse;
		return FALSE;
	}

	ModbusErrorCode = Modbus_SendReceive(TransmitBuffer, TransmitLength, ReceiveBuffer, TRM10_FRAME_BUFFER_SIZE, &ReceiveLength);
	*error = TRM10_MapModbusError(ModbusErrorCode);
	if(*error != TRME_None)
		return FALSE;

	if(ReceiveLength < TRM10_FC16_RESPONSE_SIZE)
	{
		*error = TRME_WrongResponse;
		return FALSE;
	}

	return TRUE;
}
// ----------------------------------------

// Чтение UINT16
Int16U TRM10_ReadUint16(Int8U Slave, Int16U RegAddress, pTRMError error)
{
	Int8U TransmitBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int8U ReceiveBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int16U TransmitLength, ReceiveLength;
	ModbusError ModbusErrorCode;

	TransmitLength = Modbus_BuildReadHoldingRegs(Slave, RegAddress, TRM10_UINT16_REG_COUNT, TransmitBuffer);
	if(TransmitLength == 0)
	{
		*error = TRME_WrongResponse;
		return 0;
	}

	ModbusErrorCode = Modbus_SendReceive(TransmitBuffer, TransmitLength, ReceiveBuffer, TRM10_FRAME_BUFFER_SIZE, &ReceiveLength);
	*error = TRM10_MapModbusError(ModbusErrorCode);
	if(*error != TRME_None)
		return 0;

	if(ReceiveLength < TRM10_FC03_UINT16_RESPONSE_SIZE || ReceiveBuffer[2] != TRM10_UINT16_BYTE_COUNT)
	{
		*error = TRME_WrongResponse;
		return 0;
	}

	return ((Int16U)ReceiveBuffer[3] << 8) | ReceiveBuffer[4];
}
// ----------------------------------------

// Запись UINT16 в holding-регистр по FC16
Boolean TRM10_WriteUint16(Int8U Slave, Int16U RegAddress, Int16U Value, pTRMError error)
{
	Int8U TransmitBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int8U ReceiveBuffer[TRM10_FRAME_BUFFER_SIZE];
	Int16U TransmitLength, ReceiveLength;
	ModbusError ModbusErrorCode;

	TransmitLength = Modbus_BuildWriteMultipleRegs(Slave, RegAddress, TRM10_UINT16_REG_COUNT, &Value, TransmitBuffer);
	if(TransmitLength == 0)
	{
		*error = TRME_WrongResponse;
		return FALSE;
	}

	ModbusErrorCode = Modbus_SendReceive(TransmitBuffer, TransmitLength, ReceiveBuffer, TRM10_FRAME_BUFFER_SIZE, &ReceiveLength);
	*error = TRM10_MapModbusError(ModbusErrorCode);
	if(*error != TRME_None)
		return FALSE;

	if(ReceiveLength < TRM10_FC16_RESPONSE_SIZE)
	{
		*error = TRME_WrongResponse;
		return FALSE;
	}

	return TRUE;
}
// ----------------------------------------

// Чтение измеренной температуры (Fun1)
float TRM10_ReadTemp(Int8U Address, pTRMError error)
{
	return TRM10_ReadRegFloat(Address, TRM10_REG_FUN1, error);
}
// ----------------------------------------

// Чтение выходной мощности (out.P)
float TRM10_ReadPower(Int8U Address, pTRMError error)
{
	return TRM10_ReadRegFloat(Address, TRM10_REG_OUT_P, error);
}
// ----------------------------------------

// Установка уставки регулятора (SP1)
void TRM10_SetTemp(Int8U Address, float Temperature, pTRMError error)
{
	TRM10_WriteRegFloat(Address, TRM10_REG_SP1, Temperature, error);
}
// ----------------------------------------

// Запуск регулирования (CtrL = RUN)
void TRM10_Start(Int8U Address, pTRMError error)
{
	TRM10_WriteUint16(Address, TRM10_REG_CTRL, TRM10_CTRL_RUN, error);
}
// ----------------------------------------

// Останов регулирования (CtrL = STOP)
void TRM10_Stop(Int8U Address, pTRMError error)
{
	TRM10_WriteUint16(Address, TRM10_REG_CTRL, TRM10_CTRL_STOP, error);
}
// ----------------------------------------
