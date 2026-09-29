#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "stdinc.h"
#include "ZwGPIO.h"

typedef struct __OneWireBus
{
	GPIO_PortPinSetting writePin;
	GPIO_PortPinSetting readPin;
	GPIO_PortPinSetting powerPin;
	Boolean useSinglePin;
	Boolean hasPowerPin;
	Boolean invertWrite;
	Boolean invertPower;
	Int8U ROM_NO[8];
	Int8U LastVariance;
	Int8U LastFamilyVariance;
	Boolean LastDeviceFlag;
} OneWireBus, *pOneWireBus;

// Инициализация шины: writePin, readPin, powerPin, usePowerPin, useSinglePin,
// invertWrite/invertPower — инверсия логики управляющих пинов
void OneWire_Init(OneWireBus Config);

// Обмен данными
Int8U OneWire_Reset();
void OneWire_Select(const Int8U *rom);
void OneWire_Skip();
void OneWire_Write(Int8U value, Int8U power);
void OneWire_WriteBytes(const Int8U *buf, Int16U count, Boolean power);
Int8U OneWire_Read();
void OneWire_ReadBytes(Int8U *buf, Int16U count);
void OneWire_WriteBit(Int8U value);
Int8U OneWire_ReadBit();
void OneWire_Depower();

// Сильная подтяжка на время convert / copy: MOSFET (hasPowerPin) или push-pull (один DQ)
void OneWire_StrongPullupHold(Boolean enable);

// Поиск устройств на шине
void OneWire_ResetSearch();
void OneWire_TargetSearch(Int8U familyCode);
Boolean OneWire_Search(Int8U *newAddr, Boolean searchMode);

// Подсчёт устройств заданного family code. firstRom — ROM первого найденного (NULL, если не нужен).
Boolean OneWire_SearchFamily(Int8U familyCode, Int16U *foundCount, Int8U firstRom[8]);

// ROM-адрес устройства по family code и индексу (0 — первое устройство).
Boolean OneWire_SelectByIndex(Int8U familyCode, Int8U deviceIndex, Int8U rom[8]);

// Контрольные суммы Dallas
Int8U OneWire_Crc8(const Int8U *addr, Int8U len);
Boolean OneWire_CheckCrc8(const Int8U *data, Int8U len, Int8U receivedCrc);
Boolean OneWire_CheckCrc16(const Int8U *input, Int16U len, const Int8U *invertedCrc, Int16U crc);
Int16U OneWire_Crc16(const Int8U *input, Int16U len, Int16U crc);

#endif // __ONEWIRE_H
