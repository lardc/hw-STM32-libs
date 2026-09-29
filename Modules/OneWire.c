#include "OneWire.h"
#include "Delay.h"

// 1-Wire ROM commands
#define MATCH_ROM				0x55
#define SKIP_ROM 				0xCC

// Reset
#define LINE_RETRIES_UNTIL_FREE	125

static OneWireBus Bus;

// Отключение прерываний с сохранением Primask
static inline Int32U OneWire_irq_save()
{
	Int32U primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}
//-------------------

// Управление линией записи
static void OneWire_SetWriteLine(Boolean active)
{
	GPIO_SetState(Bus.writePin, Bus.invertWrite ? !active : active);
}
//-------------------

// Чтение линии данных
static Boolean OneWire_ReadLine()
{
	return GPIO_GetState(Bus.readPin);
}
//-------------------

// Управление линией parasite power (сильная подтяжка к питанию)
static void OneWire_SetPowerLine(Boolean active)
{
	if(Bus.hasPowerPin)
		GPIO_SetState(Bus.powerPin, Bus.invertPower ? !active : active);
}
//-------------------

// Инициализация шины
void OneWire_Init(OneWireBus Config)
{
	Bus = Config;

	if(Bus.useSinglePin)
	{
		Bus.writePin = Bus.readPin;
		Bus.powerPin = Bus.readPin;

		GPIO_InitOpenDrainOutput(Bus.writePin, NoPull);
	}
	else
	{
		GPIO_InitPushPullOutput(Bus.writePin);
		GPIO_InitInput(Bus.readPin, NoPull);

		if(Bus.hasPowerPin)
		{
			GPIO_InitOpenDrainOutput(Bus.powerPin, NoPull);
			OneWire_SetPowerLine(false);
		}
	}

	OneWire_SetWriteLine(true);
	OneWire_ResetSearch();
}
//-------------------

// Сброс шины 1-Wire. Возвращает 1 при ответе устройства (presence pulse), 0 — при отсутствии или КЗ
Int8U OneWire_Reset()
{
	Int8U result;
	Int8U retries = LINE_RETRIES_UNTIL_FREE;
	Int32U Primask;

	OneWire_SetWriteLine(true);

	// Ожидание освобождения линии
	do
	{
		if(--retries == 0)
			return 0;
		DELAY_US(2);
	}
	while(!OneWire_ReadLine());

	OneWire_SetWriteLine(false);
	DELAY_US(480);

	Primask = OneWire_irq_save();
	OneWire_SetWriteLine(true);
	DELAY_US(70);
	result = !OneWire_ReadLine();
	__set_PRIMASK(Primask);
	DELAY_US(410);

	return result;
}
//-------------------

// Запись одного бита на шину
void OneWire_WriteBit(Int8U value)
{
	Int32U Primask;
	if(value & 1)
	{
		Primask = OneWire_irq_save();
		OneWire_SetWriteLine(false);
		DELAY_US(10);
		OneWire_SetWriteLine(true);
		__set_PRIMASK(Primask);
		DELAY_US(55);
	}
	else
	{
		Primask = OneWire_irq_save();
		OneWire_SetWriteLine(false);
		DELAY_US(65);
		OneWire_SetWriteLine(true);
		__set_PRIMASK(Primask);
		DELAY_US(5);
	}
}
//-------------------

// Чтение одного бита с шины
Int8U OneWire_ReadBit()
{
	Int8U result;
	Int32U Primask;

	Primask = OneWire_irq_save();
	OneWire_SetWriteLine(false);
	DELAY_US(3);
	OneWire_SetWriteLine(true);
	DELAY_US(10);
	result = OneWire_ReadLine();
	__set_PRIMASK(Primask);
	DELAY_US(53);

	return result;
}
//-------------------

// Запись байта. При power != 0 удерживает parasite power до вызова OneWire_Depower()
void OneWire_Write(Int8U value, Int8U power)
{
	for(Int8U bitMask = 0x01; bitMask; bitMask <<= 1)
		OneWire_WriteBit((bitMask & value) ? 1 : 0);

	if(power && Bus.hasPowerPin)
		OneWire_SetPowerLine(true);
	else
		OneWire_SetWriteLine(true);
}
//-------------------

// Запись буфера байтов
void OneWire_WriteBytes(const Int8U *buf, Int16U count, Boolean power)
{
	for(Int16U i = 0; i < count; i++)
		OneWire_Write(buf[i], 0);

	if(power && Bus.hasPowerPin)
		OneWire_SetPowerLine(true);
}
//-------------------

// Чтение байта
Int8U OneWire_Read()
{
	Int8U result = 0;

	for(Int8U bitMask = 0x01; bitMask; bitMask <<= 1)
	{
		if(OneWire_ReadBit())
			result |= bitMask;
	}

	return result;
}
//-------------------

// Чтение буфера байтов
void OneWire_ReadBytes(Int8U *buf, Int16U count)
{
	for(Int16U i = 0; i < count; i++)
		buf[i] = OneWire_Read();
}
//-------------------

// Команда MATCH ROM (0x55) — выбор устройства по 64-битному адресу
void OneWire_Select(const Int8U *rom)
{
	OneWire_Write(MATCH_ROM, 0);

	for(Int8U i = 0; i < 8; i++)
		OneWire_Write(rom[i], 0);
}
//-------------------

// Команда SKIP ROM (0xCC) — обращение ко всем устройствам на шине
void OneWire_Skip()
{
	OneWire_Write(SKIP_ROM, 0);
}
//-------------------

// Отключение parasite power и освобождение линии данных
void OneWire_Depower()
{
	if(Bus.hasPowerPin)
		OneWire_SetPowerLine(false);

	OneWire_SetWriteLine(true);
}
//-------------------

// Сильная подтяжка для parasite power на время convert / copy (DS18B20, DS2431).
// hasPowerPin — включить линию PWR (MOSFET); иначе DQ переключается в push-pull и удерживает высокий уровень.
void OneWire_StrongPullupHold(Boolean enable)
{
	if(Bus.hasPowerPin)
		OneWire_SetPowerLine(enable);
	else
	{
		// Один DQ без MOSFET: push-pull на время convert
		GPIO_SetState(Bus.writePin, true);
		GPIO_Config(Bus.writePin.Port, Bus.writePin.Pin, Output, enable ? PushPull : OpenDrain, HighSpeed, NoPull);
		GPIO_SetState(Bus.writePin, true);
	}
}
//-------------------

// Сброс состояния поиска устройств на шине
void OneWire_ResetSearch()
{
	Bus.LastVariance = 0;
	Bus.LastDeviceFlag = false;
	Bus.LastFamilyVariance = 0;

	for(Int8S i = 7; i >= 0; i--)
		Bus.ROM_NO[i] = 0;
}
//-------------------

// Настройка поиска устройств заданного семейства (первый байт ROM)
void OneWire_TargetSearch(Int8U familyCode)
{
	Bus.ROM_NO[0] = familyCode;

	for(Int8U i = 1; i < 8; i++)
		Bus.ROM_NO[i] = 0;

	Bus.LastVariance = 64;
	Bus.LastFamilyVariance = 0;
	Bus.LastDeviceFlag = false;
}
//-------------------

// Поиск следующего устройства на шине. Возвращает true и записывает ROM в newAddr при успехе
Boolean OneWire_Search(Int8U *newAddr, Boolean searchMode)
{
	Int8U idBitNumber;
	Int8U lastZero, romByteNumber;
	Boolean searchResult;
	Int8U idBit, cmpIdBit;
	Int8U romByteMask, searchDirection;

	idBitNumber = 1;
	lastZero = 0;
	romByteNumber = 0;
	romByteMask = 1;
	searchResult = false;

	if(!Bus.LastDeviceFlag)
	{
		if(!OneWire_Reset())
		{
			Bus.LastVariance = 0;
			Bus.LastDeviceFlag = false;
			Bus.LastFamilyVariance = 0;
			return false;
		}

		if(searchMode)
			OneWire_Write(0xF0, 0);
		else
			OneWire_Write(0xEC, 0);

		do
		{
			idBit = OneWire_ReadBit();
			cmpIdBit = OneWire_ReadBit();

			if((idBit == 1) && (cmpIdBit == 1))
				break;

			if(idBit != cmpIdBit)
				searchDirection = idBit;
			else
			{
				if(idBitNumber < Bus.LastVariance)
					searchDirection = ((Bus.ROM_NO[romByteNumber] & romByteMask) > 0);
				else
					searchDirection = (idBitNumber == Bus.LastVariance);

				if(searchDirection == 0)
				{
					lastZero = idBitNumber;
					if(lastZero < 9)
						Bus.LastFamilyVariance = lastZero;
				}
			}

			if(searchDirection == 1)
				Bus.ROM_NO[romByteNumber] |= romByteMask;
			else
				Bus.ROM_NO[romByteNumber] &= ~romByteMask;

			OneWire_WriteBit(searchDirection);

			idBitNumber++;
			romByteMask <<= 1;

			if(romByteMask == 0)
			{
				romByteNumber++;
				romByteMask = 1;
			}
		}
		while(romByteNumber < 8);

		if(!(idBitNumber < 65))
		{
			Bus.LastVariance = lastZero;
			if(Bus.LastVariance == 0)
				Bus.LastDeviceFlag = true;
			searchResult = true;
		}
	}

	if(!searchResult || !Bus.ROM_NO[0])
	{
		Bus.LastVariance = 0;
		Bus.LastDeviceFlag = false;
		Bus.LastFamilyVariance = 0;
		searchResult = false;
	}
	else
	{
		for(Int8S i = 0; i < 8; i++)
			newAddr[i] = Bus.ROM_NO[i];
	}

	return searchResult;
}
//-------------------

// Подсчёт устройств заданного family code на шине
Boolean OneWire_SearchFamily(Int8U familyCode, Int16U *foundCount, Int8U firstRom[8])
{
	Int8U addr[8];

	*foundCount = 0;

	OneWire_ResetSearch();
	OneWire_TargetSearch(familyCode);

	while(OneWire_Search(addr, true))
	{
		if(!OneWire_CheckCrc8(addr, 7, addr[7]))
			return false;

		if(addr[0] != familyCode)
			continue;

		if(*foundCount == 0 && firstRom != NULL)
		{
			for(Int8U i = 0; i < 8; i++)
				firstRom[i] = addr[i];
		}

		(*foundCount)++;
	}

	return true;
}
//-------------------

// ROM-адрес устройства по family code и индексу (0 — первое устройство)
Boolean OneWire_SelectByIndex(Int8U familyCode, Int8U deviceIndex, Int8U rom[8])
{
	Int8U addr[8];
	Int16U count = 0;

	OneWire_ResetSearch();
	OneWire_TargetSearch(familyCode);

	while(OneWire_Search(addr, true))
	{
		if(!OneWire_CheckCrc8(addr, 7, addr[7]))
			return false;

		if(addr[0] != familyCode)
			continue;

		if(count == deviceIndex)
		{
			for(Int8U i = 0; i < 8; i++)
				rom[i] = addr[i];

			return true;
		}

		count++;
	}

	return false;
}
//-------------------

// Расчёт 8-битного CRC
Int8U OneWire_Crc8(const Int8U *addr, Int8U len)
{
	Int8U crc = 0;

	while(len--)
	{
		Int8U inbyte = *addr++;

		for(Int8U i = 8; i; i--)
		{
			Int8U mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if(mix)
				crc ^= 0x8C;
			inbyte >>= 1;
		}
	}

	return crc;
}
//-------------------

// Проверка 8-битного CRC (receivedCrc — байт CRC из ответа устройства)
Boolean OneWire_CheckCrc8(const Int8U *data, Int8U len, Int8U receivedCrc)
{
	return OneWire_Crc8(data, len) == receivedCrc;
}
//-------------------

// Проверка 16-битного CRC (принимает инвертированные байты из ответа устройства)
Boolean OneWire_CheckCrc16(const Int8U *input, Int16U len, const Int8U *invertedCrc, Int16U crc)
{
	crc = ~OneWire_Crc16(input, len, crc);
	return (crc & 0xFF) == invertedCrc[0] && (crc >> 8) == invertedCrc[1];
}
//-------------------

// Расчёт 16-битного CRC Dallas
Int16U OneWire_Crc16(const Int8U *input, Int16U len, Int16U crc)
{
	static const Int8U oddparity[16] =
		{ 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

	for(Int16U i = 0; i < len; i++)
	{
		Int16U cdata = input[i];
		cdata = (cdata ^ crc) & 0xff;
		crc >>= 8;

		if(oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
			crc ^= 0xC001;

		cdata <<= 6;
		crc ^= cdata;
		cdata <<= 1;
		crc ^= cdata;
	}

	return crc;
}
//-------------------
