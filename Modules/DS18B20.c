// Header
#include "DS18B20.h"

// Includes
#include "OneWire.h"
#include "Board.h"
#include "Delay.h"

// DS18B20 ROM commands
#define DS18B20_READ_ROM				0x33
#define DS18B20_MATCH_ROM				0x55
#define DS18B20_SKIP_ROM				0xCC
#define DS18B20_SEARCH_ROM				0xF0
#define DS18B20_ALARM_SEARCH			0xEC

// DS18B20 function commands
#define DS18B20_WRITE_SCRATCHPAD		0x4E
#define DS18B20_READ_SCRATCHPAD			0xBE
#define DS18B20_COPY_SCRATCHPAD			0x48
#define DS18B20_RECALL_E2				0xB8
#define DS18B20_READ_PWR_SUPPLY			0xB4
#define DS18B20_CONVERT_T				0x44

// CONFIG register (resolution)
#define CONFIG_RES_9BIT 				0x1F
#define CONFIG_RES_10BIT 				0x3F
#define CONFIG_RES_11BIT 				0x5F
#define CONFIG_RES_12BIT 				0x7F

// Scratchpad
#define DS18B20_SCRATCHPAD_SIZE			9
#define REG_TEMPERATURE_LSB				0
#define REG_TEMPERATURE_MSB				1
#define REG_USER_BYTE_1					2	// TH (EEPROM)
#define REG_USER_BYTE_2					3	// TL (EEPROM)
#define REG_CONFIGURATION				4
#define REG_CRC							8

#define DS18B20_WRITE_TIMEOUT		5
#define DS18B20_CONVERT_DELAY_MS	750

// Variables
static Int8U SerialNumber[DS18B20_ONE_WIRE_MAC_SIZE];
static Boolean SkipRom = true, PowerPin = false;

// Forward functions
static Boolean DS18B20_StartTransmission();
static Boolean DS18B20_ReadScratchpad(Int8U *Scratchpad);
static Boolean DS18B20_StartConvert();

// Functions
void DS18B20_Init(Boolean HasPowerPin)
{
	PowerPin = HasPowerPin;
	Int8U addr[DS18B20_ONE_WIRE_MAC_SIZE];

	OneWire_ResetSearch();
	OneWire_TargetSearch(DS18B20_FAMILY_CODE);

	if(OneWire_Search(addr, true) && OneWire_CheckCrc8(addr, 7, addr[7]))
		DS18B20_Begin(addr);
}
//-------------------

void DS18B20_Begin(Int8U serialNumber[DS18B20_ONE_WIRE_MAC_SIZE])
{
	for (Int8U i = 0; i < DS18B20_ONE_WIRE_MAC_SIZE; i++)
		SerialNumber[i] = serialNumber[i];

	SkipRom = false;
}
//-------------------

Boolean DS18B20_WriteReg(pInt16U Data)
{
	Int8U Scratchpad[DS18B20_SCRATCHPAD_SIZE];

	if(!DS18B20_StartTransmission())
		return false;

	OneWire_Write(DS18B20_WRITE_SCRATCHPAD, 0);
	OneWire_Write((*Data >> 8) & 0xFF, 0);
	OneWire_Write(*Data & 0xFF, 0);
	OneWire_Write(CONFIG_RES_12BIT, 0);

	if(!DS18B20_ReadScratchpad(Scratchpad))
		return false;

	if(Scratchpad[REG_USER_BYTE_1] != ((*Data >> 8) & 0xFF)
			|| Scratchpad[REG_USER_BYTE_2] != (*Data & 0xFF)
			|| Scratchpad[REG_CONFIGURATION] != CONFIG_RES_12BIT)
		return false;

	if(!DS18B20_StartTransmission())
		return false;

	OneWire_Write(DS18B20_COPY_SCRATCHPAD, 1);
	DELAY_MS(10);
	OneWire_Depower();

	{
		Int16U TimeoutCounter = 0;

		while(!OneWire_Reset())
		{
			DELAY_US(1000);
			if(++TimeoutCounter >= DS18B20_WRITE_TIMEOUT)
				return false;
		}
	}

	return true;
}
//-------------------

Boolean DS18B20_ReadReg(pInt16U Data)
{
	Int8U Scratchpad[DS18B20_SCRATCHPAD_SIZE];

	if(!DS18B20_ReadScratchpad(Scratchpad))
		return false;

	*Data = ((Int16U)Scratchpad[REG_USER_BYTE_1] << 8) | Scratchpad[REG_USER_BYTE_2];
	return true;
}
//-------------------

Boolean DS18B20_ReadTemperature(pFloat32 Data)
{
	Int8U Scratchpad[DS18B20_SCRATCHPAD_SIZE];
	Int16S Raw;

	if(!DS18B20_StartConvert())
		return false;

	if(!DS18B20_ReadScratchpad(Scratchpad))
		return false;

	Raw = (Int16S)((Int16U)Scratchpad[REG_TEMPERATURE_MSB] << 8 | Scratchpad[REG_TEMPERATURE_LSB]);
	*Data = Raw / 16.0f;

	return true;
}
//-------------------

static Boolean DS18B20_StartTransmission()
{
	if(!OneWire_Reset())
		return false;

	if(SkipRom)
		OneWire_Skip();
	else
		OneWire_Select(SerialNumber);

	return true;
}
//-------------------

static Boolean DS18B20_StartConvert()
{
	if(!DS18B20_StartTransmission())
		return false;

	if(PowerPin)
	{
		OneWire_Write(DS18B20_CONVERT_T, 1);
		DELAY_MS(DS18B20_CONVERT_DELAY_MS);
		OneWire_Depower();
	}
	else
	{
		OneWire_Write(DS18B20_CONVERT_T, 0);
		OneWire_StrongPullupHold(true);
		DELAY_MS(DS18B20_CONVERT_DELAY_MS);
		OneWire_StrongPullupHold(false);
	}

	return true;
}
//-------------------

static Boolean DS18B20_ReadScratchpad(Int8U *Scratchpad)
{
	if(!DS18B20_StartTransmission())
		return false;

	OneWire_Write(DS18B20_READ_SCRATCHPAD, 0);
	OneWire_ReadBytes(Scratchpad, DS18B20_SCRATCHPAD_SIZE);

	return OneWire_CheckCrc8(Scratchpad, REG_CRC, Scratchpad[REG_CRC]);
}
//-------------------
