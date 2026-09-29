#ifndef DS18B20_H_
#define DS18B20_H_

#include "stdinc.h"

#define DS18B20_ONE_WIRE_MAC_SIZE		8
#define DS18B20_FAMILY_CODE				0x28

void DS18B20_Init(Boolean HasPowerPin);

// Установить ROM-адрес (family code + serial + CRC из OneWire_Search).
// После вызова адресация идёт через MATCH ROM вместо SKIP ROM.
void DS18B20_Begin(Int8U serialNumber[DS18B20_ONE_WIRE_MAC_SIZE]);

// Запись / чтение 16-битного слова в EEPROM-байты TH и TL (scratchpad [2..3])
Boolean DS18B20_WriteReg(pInt16U Data);
Boolean DS18B20_ReadReg(pInt16U Data);

// Температура в °C. Если запись не в float, то надо домножать на 10.
Boolean DS18B20_ReadTemperature(pFloat32 Data);

#endif /* DS18B20_H_ */
