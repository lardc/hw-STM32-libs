// Header
#include "DS2431.h"

// Includes
#include "OneWire.h"
#include "Delay.h"

// EEPROM layout
#define DS2431_ROW_SIZE					8

// Scratchpad ES byte (Ending Offset)
#define DS2431_ES_OFFSET_MASK			0x07
#define DS2431_ES_PF_FLAG				0x20	// programming failure
#define DS2431_ES_AA_FLAG				0x80	// authorization accepted

// Scratchpad read buffer layout
#define DS2431_CMD_SIZE					3
#define DS2431_CRC_SIZE					2
#define DS2431_BUFFER_SIZE				(DS2431_ROW_SIZE + DS2431_CMD_SIZE + DS2431_CRC_SIZE)

// DS2431 function commands
#define DS2431_WRITE_SCRATCHPAD			0x0F
#define DS2431_READ_SCRATCHPAD			0xAA
#define DS2431_COPY_SCRATCHPAD			0x55
#define DS2431_READ_MEMORY				0xF0

// Erase
#define DS2431_ERASE_BYTE				0xFF

// Timing and retries
#define DS2431_READ_RETRY				3
#define DS2431_COPY_TIMEOUT				10
#define DS2431_COPY_DELAY_MS			25
#define DS2431_BUS_RECOVERY_MS			2

// Variables
static Int8U SerialNumber[DS2431_MAC_SIZE];
static Boolean SkipRom = true;

static Int8U DeviceRom[DS2431_MAX_DEVICES][DS2431_MAC_SIZE];
static Int16U DeviceCount = 0;
static DS2431Error LastError = DS2431_OK;

static const Int8U EraseRow[DS2431_ROW_SIZE] =
{
	DS2431_ERASE_BYTE, DS2431_ERASE_BYTE, DS2431_ERASE_BYTE, DS2431_ERASE_BYTE,
	DS2431_ERASE_BYTE, DS2431_ERASE_BYTE, DS2431_ERASE_BYTE, DS2431_ERASE_BYTE
};

// Forward functions
static void DS2431_SetError(DS2431Error Error);
static Boolean DS2431_Select(Int8U deviceIndex);
static Boolean DS2431_StartTransmission();
static Boolean DS2431_WriteScratchpad(Int16U address, const Int8U *buf, Int8U count);
static Boolean DS2431_ReadScratchpadRaw(Int8U *readBuf, Int16U len);
static Boolean DS2431_CopyScratchpad(const Int8U copyAuth[3]);
static Boolean DS2431_CheckReadScratchpadCrc(const Int8U *readBuf);
static Boolean DS2431_IsScratchpadStatusValid(Int8U esByte, Int16U address, Int8U count);
static Boolean DS2431_VerifyRow(Int16U address, const Int8U *buf, Int8U count);
static Boolean DS2431_ParseScratchpad(const Int8U *readBuf, Int16U address, const Int8U *buf, Int8U count,
		Boolean verify, Int8U copyAuth[3]);
static Boolean DS2431_WriteInternal(Int16U address, const Int8U *buf, Int16U count, Boolean verify);
static Boolean DS2431_IsIdleBusResponse(const Int8U *readBuf, Int16U len);
static Boolean DS2431_VerifyCopyAccepted();

// Functions
static void DS2431_SetError(DS2431Error Error)
{
	LastError = Error;
}
//-------------------

DS2431Error DS2431_GetLastError()
{
	return LastError;
}
//-------------------

// Сканирование шины и заполнение таблицы ROM
Boolean DS2431_Init()
{
	Int8U addr[DS2431_MAC_SIZE];

	DeviceCount = 0;

	OneWire_ResetSearch();
	OneWire_TargetSearch(DS2431_FAMILY_CODE);

	while(DeviceCount < DS2431_MAX_DEVICES && OneWire_Search(addr, true))
	{
		if(!OneWire_CheckCrc8(addr, 7, addr[7]))
			continue;

		if(addr[0] != DS2431_FAMILY_CODE)
			continue;

		for(Int8U i = 0; i < DS2431_MAC_SIZE; i++)
			DeviceRom[DeviceCount][i] = addr[i];

		DeviceCount++;
	}

	if(DeviceCount == 0)
	{
		DS2431_SetError(DS2431_ERR_NO_DEVICE);
		return false;
	}

	DS2431_SetError(DS2431_OK);
	return true;
}
//-------------------

Int16U DS2431_GetDeviceCount()
{
	return DeviceCount;
}
//-------------------

static Boolean DS2431_Select(Int8U deviceIndex)
{
	if(deviceIndex >= DeviceCount)
	{
		DS2431_SetError(DS2431_ERR_NO_DEVICE);
		return false;
	}

	DS2431_Begin(DeviceRom[deviceIndex]);
	return true;
}
//-------------------

// Сохранить ROM-адрес; далее обмен только через MATCH ROM (не SKIP)
void DS2431_Begin(Int8U serialNumber[DS2431_MAC_SIZE])
{
	for(Int8U i = 0; i < DS2431_MAC_SIZE; i++)
		SerialNumber[i] = serialNumber[i];

	SkipRom = false;
}
//-------------------

// Команда 0xF0: чтение len байт с address
Boolean DS2431_Read(Int16U address, Int8U *buf, Int16U len)
{
	if(!DS2431_StartTransmission())
	{
		for(Int16U i = 0; i < len; i++)
			buf[i] = 0xFF;

		OneWire_Depower();
		DELAY_MS(DS2431_BUS_RECOVERY_MS);
		return false;	// нет presence на reset
	}

	OneWire_Write(DS2431_READ_MEMORY, 0);
	OneWire_Write((Int8U)(address & 0xFF), 0);
	OneWire_Write((Int8U)((address >> 8) & 0xFF), 0);
	OneWire_ReadBytes(buf, len);

	OneWire_Depower();
	DELAY_MS(DS2431_BUS_RECOVERY_MS);
	DS2431_SetError(DS2431_OK);
	return true;
}
//-------------------

// Запись одной 8-байтной строки; count должен быть DS2431_ROW_SIZE, address кратен 8
Boolean DS2431_Write(Int16U address, const Int8U *buf, Int16U count, Boolean verify)
{
	Boolean ret = DS2431_WriteInternal(address, buf, count, verify);

	OneWire_Depower();
	DELAY_MS(DS2431_BUS_RECOVERY_MS);
	return ret;
}
//-------------------

// Запись 0xFF в область данных 0x00..0x7F (по одной строке)
Boolean DS2431_EraseAll(Int8U deviceIndex, Boolean verify)
{
	if(!DS2431_Select(deviceIndex))
		return false;

	for(Int16U address = 0; address < DS2431_EEPROM_SIZE; address += DS2431_ROW_SIZE)
	{
		if(!DS2431_Write(address, EraseRow, DS2431_ROW_SIZE, verify))
			return false;
	}

	DS2431_SetError(DS2431_OK);
	return true;
}
//-------------------

// Чтение len байт с адреса 0 области данных
Boolean DS2431_ReadArray(Int8U deviceIndex, Int8U *buf, Int16U len)
{
	if(!DS2431_Select(deviceIndex))
		return false;

	if(len > DS2431_EEPROM_SIZE)
	{
		DS2431_SetError(DS2431_ERR_PARAM);
		return false;
	}

	if(len == 0)
	{
		DS2431_SetError(DS2431_ERR_PARAM);
		return false;
	}

	if(buf == NULL)
	{
		DS2431_SetError(DS2431_ERR_PARAM);
		return false;
	}

	return DS2431_Read(0, buf, len);
}
//-------------------

// Запись len байт с адреса 0; неполный хвост строки дополняется текущим содержимым EEPROM
Boolean DS2431_WriteArray(Int8U deviceIndex, const Int8U *buf, Int16U len)
{
	Int8U row[DS2431_ROW_SIZE];

	if(!DS2431_Select(deviceIndex))
		return false;

	if(len > DS2431_EEPROM_SIZE)
	{
		DS2431_SetError(DS2431_ERR_PARAM);
		return false;	// запрос выходит за область данных 0x00..0x7F
	}

	if(len == 0 || (buf == NULL))
	{
		DS2431_SetError(DS2431_ERR_PARAM);
		return false;
	}

	for(Int16U address = 0; address < len; address += DS2431_ROW_SIZE)
	{
		Int16U chunk = len - address;
		const Int8U *writeBuf;

		if(chunk > DS2431_ROW_SIZE)	// остаток длиннее строки — ограничить 8 байтами
			chunk = DS2431_ROW_SIZE;

		if(chunk < DS2431_ROW_SIZE)	// хвост короче 8 байт — нужно слить с текущей строкой EEPROM
		{
			if(!DS2431_Read(address, row, DS2431_ROW_SIZE))
				return false;	// не удалось прочитать строку для слияния с новыми байтами

			for(Int16U i = 0; i < chunk; i++)
				row[i] = buf[address + i];

			writeBuf = row;
		}
		else
			writeBuf = &buf[address];	// полная строка — пишем напрямую из buf

		if(!DS2431_Write(address, writeBuf, DS2431_ROW_SIZE, true))
			return false;	// copy не записал строку в EEPROM
	}

	DS2431_SetError(DS2431_OK);
	return true;
}
//-------------------

// Ответ шины «все единицы» — устройство не ответило на Read Scratchpad
static Boolean DS2431_IsIdleBusResponse(const Int8U *readBuf, Int16U len)
{
	for(Int16U i = 0; i < len; i++)
	{
		if(readBuf[i] != 0xFF)
			return false;
	}

	return true;
}
//-------------------

// Reset и адресация: SKIP ROM или MATCH ROM после DS2431_Begin
static Boolean DS2431_StartTransmission()
{
	if(!OneWire_Reset())
	{
		DS2431_SetError(DS2431_ERR_LINE);
		return false;
	}

	if(SkipRom)
		OneWire_Skip();
	else
		OneWire_Select(SerialNumber);

	return true;
}
//-------------------

// Команда 0x0F: загрузка count байт в scratchpad (без программирования EEPROM)
static Boolean DS2431_WriteScratchpad(Int16U address, const Int8U *buf, Int8U count)
{
	if(!DS2431_StartTransmission())
		return false;

	OneWire_Write(DS2431_WRITE_SCRATCHPAD, 0);
	OneWire_Write((Int8U)(address & 0xFF), 0);
	OneWire_Write((Int8U)((address >> 8) & 0xFF), 0);

	for(Int8U i = 0; i < count; i++)
		OneWire_Write(buf[i], 0);

	return true;
}
//-------------------

// Команда 0xAA: чтение TA1, TA2, E/S и данных scratchpad без разбора
static Boolean DS2431_ReadScratchpadRaw(Int8U *readBuf, Int16U len)
{
	if(!DS2431_StartTransmission())
		return false;

	OneWire_Write(DS2431_READ_SCRATCHPAD, 0);
	OneWire_ReadBytes(readBuf, len);

	return true;
}
//-------------------

// Команда 0x55: copy scratchpad → EEPROM; strong pullup на t_PROG, затем проверка AA
static Boolean DS2431_CopyScratchpad(const Int8U copyAuth[3])
{
	Int16U timeout = 0;

	if(!DS2431_StartTransmission())
		return false;	// нет presence на reset

	OneWire_Write(DS2431_COPY_SCRATCHPAD, 0);
	OneWire_Write(copyAuth[0], 0);
	OneWire_Write(copyAuth[1], 0);
	OneWire_Write(copyAuth[2], 0);

	OneWire_StrongPullupHold(true);
	DELAY_MS(DS2431_COPY_DELAY_MS);
	OneWire_StrongPullupHold(false);

	while(!OneWire_Reset())
	{
		DELAY_US(1000);

		if(++timeout >= DS2431_COPY_TIMEOUT)
		{
			DS2431_SetError(DS2431_ERR_VERIFY);
			return false;	// чип не завершил t_PROG (шина не отпустилась)
		}
	}

	DELAY_MS(DS2431_BUS_RECOVERY_MS);

	return DS2431_VerifyCopyAccepted();
}
//-------------------

// Подтверждение, что Copy Scratchpad принят чипом (байт E/S после copy)
static Boolean DS2431_VerifyCopyAccepted()
{
	Int8U readBuf[DS2431_CMD_SIZE];

	if(!DS2431_ReadScratchpadRaw(readBuf, sizeof(readBuf)))
		return false;	// нет ответа на шине

	if(readBuf[2] & DS2431_ES_PF_FLAG)
	{
		DS2431_SetError(DS2431_ERR_VERIFY);
		return false;	// PF=1: scratchpad невалиден (сбой питания или неполная запись)
	}

	if(!(readBuf[2] & DS2431_ES_AA_FLAG))
	{
		DS2431_SetError(DS2431_ERR_VERIFY);
		return false;	// AA=0: copy не начался (неверный auth, защита, PF)
	}

	return true;
}
//-------------------

// CRC-16 ответа Read Scratchpad (команда + TA1 + TA2 + E/S + данные)
static Boolean DS2431_CheckReadScratchpadCrc(const Int8U *readBuf)
{
	Int8U buffer[DS2431_BUFFER_SIZE];
	Int8U crc16[DS2431_CRC_SIZE];
	Int8U tOff = readBuf[0] & DS2431_ES_OFFSET_MASK;
	Int8U eOff = readBuf[2] & DS2431_ES_OFFSET_MASK;
	Int8U dataLen;
	Int16U crcLen;

	if(eOff < tOff)
		return false;

	dataLen = (Int8U)(eOff - tOff + 1);
	crcLen = (Int16U)(1 + 3 + dataLen);

	buffer[0] = DS2431_READ_SCRATCHPAD;

	for(Int8U i = 0; i < 3 + dataLen; i++)
		buffer[1 + i] = readBuf[i];

	crc16[0] = readBuf[3 + dataLen];
	crc16[1] = readBuf[3 + dataLen + 1];

	return OneWire_CheckCrc16(buffer, crcLen, crc16, 0);
}
//-------------------

// E/S перед copy: PF и AA сброшены, E[2:0] соответствует концу 8-байтной строки
static Boolean DS2431_IsScratchpadStatusValid(Int8U esByte, Int16U address, Int8U count)
{
	Int8U expectedEnd;

	if(esByte & DS2431_ES_PF_FLAG)
		return false;

	if(esByte & DS2431_ES_AA_FLAG)
		return false;

	expectedEnd = (Int8U)(((address & 0x07) + count - 1) & DS2431_ES_OFFSET_MASK);
	return (esByte & DS2431_ES_OFFSET_MASK) == expectedEnd;
}
//-------------------

// Сверка count байт EEPROM с ожидаемым буфером после copy
static Boolean DS2431_VerifyRow(Int16U address, const Int8U *buf, Int8U count)
{
	Int8U row[DS2431_ROW_SIZE];

	if(!DS2431_StartTransmission())
		return false;

	OneWire_Write(DS2431_READ_MEMORY, 0);
	OneWire_Write((Int8U)(address & 0xFF), 0);
	OneWire_Write((Int8U)((address >> 8) & 0xFF), 0);
	OneWire_ReadBytes(row, count);

	for(Int8U i = 0; i < count; i++)
	{
		if(row[i] != buf[i])
		{
			DS2431_SetError(DS2431_ERR_VERIFY);
			return false;
		}
	}

	return true;
}
//-------------------

// Разбор Read Scratchpad: статус, адрес, данные, CRC; copyAuth = TA1, TA2, E/S
static Boolean DS2431_ParseScratchpad(const Int8U *readBuf, Int16U address, const Int8U *buf, Int8U count,
		Boolean verify, Int8U copyAuth[3])
{
	if(!DS2431_IsScratchpadStatusValid(readBuf[2], address, count))
	{
		DS2431_SetError(DS2431_ERR_VERIFY);
		return false;	// E/S: PF/AA или смещение конца строки не совпадает с count
	}

	if(address != ((Int16U)readBuf[1] << 8 | readBuf[0]))
	{
		DS2431_SetError(DS2431_ERR_VERIFY);
		return false;	// TA1/TA2 в scratchpad не совпадают с целевым address
	}

	if(verify)	// сверить данные scratchpad с буфером перед copy
	{
		for(Int16U i = 0; i < DS2431_ROW_SIZE; i++)
		{
			if(readBuf[DS2431_CMD_SIZE + i] != buf[i])
			{
				DS2431_SetError(DS2431_ERR_VERIFY);
				return false;	// байт scratchpad не совпал с buf[i]
			}
		}
	}

	if(!DS2431_CheckReadScratchpadCrc(readBuf))
	{
		DS2431_SetError(DS2431_ERR_VERIFY);
		return false;	// CRC-16 ответа Read Scratchpad не сошёлся
	}

	copyAuth[0] = readBuf[0];
	copyAuth[1] = readBuf[1];
	copyAuth[2] = readBuf[2];

	return true;
}
//-------------------

// Запись одной 8-байтной строки EEPROM: scratchpad → copy → опциональная проверка
static Boolean DS2431_WriteInternal(Int16U address, const Int8U *buf, Int16U count, Boolean verify)
{
	Int8U errorCount = 0;
	Int8U readBuf[DS2431_CMD_SIZE + DS2431_ROW_SIZE + DS2431_CRC_SIZE];
	Int8U copyAuth[3];

	if(address >= DS2431_EEPROM_SIZE || (address % DS2431_ROW_SIZE) != 0 || count != DS2431_ROW_SIZE)
	{
		DS2431_SetError(DS2431_ERR_PARAM);
		return false;	// вне данных EEPROM или не целая строка
	}

	do
	{
		if(!DS2431_WriteScratchpad(address, buf, count))
		{
			errorCount++;	// reset / MATCH ROM или обрыв на Write Scratchpad
			continue;
		}

		DELAY_MS(DS2431_BUS_RECOVERY_MS);

		if(!DS2431_ReadScratchpadRaw(readBuf, sizeof(readBuf)))
		{
			errorCount++;	// нет presence на Read Scratchpad
			continue;
		}

		if(DS2431_IsIdleBusResponse(readBuf, sizeof(readBuf)))
		{
			DS2431_SetError(DS2431_ERR_NO_DEVICE);
			errorCount++;	// нет ответа устройства на Read Scratchpad
			continue;
		}

		if(!DS2431_ParseScratchpad(readBuf, address, buf, count, verify, copyAuth))
		{
			errorCount++;	// scratchpad / CRC / данные не совпали
			continue;
		}

		if(!DS2431_CopyScratchpad(copyAuth))
		{
			errorCount++;	// таймаут t_PROG или copy не принят (AA)
			continue;
		}

		if(verify && !DS2431_VerifyRow(address, buf, count))
		{
			DELAY_MS(5);

			if(!DS2431_CopyScratchpad(copyAuth) || !DS2431_VerifyRow(address, buf, count))
			{
				errorCount++;	// EEPROM после copy и повтора copy не совпала с buf
				continue;
			}
		}

		DS2431_SetError(DS2431_OK);
		return true;
	}
	while(errorCount < DS2431_READ_RETRY);

	if(LastError == DS2431_OK)
		DS2431_SetError(DS2431_ERR_VERIFY);

	return false;
}
//-------------------
