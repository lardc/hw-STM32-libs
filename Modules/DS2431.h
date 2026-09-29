#ifndef DS2431_H_
#define DS2431_H_

#include "stdinc.h"

#define DS2431_MAC_SIZE			8
#define DS2431_FAMILY_CODE		0x2D
#define DS2431_EEPROM_SIZE		128
#define DS2431_MAX_DEVICES		8

typedef enum __DS2431Error
{
	DS2431_OK = 0,
	DS2431_ERR_LINE,
	DS2431_ERR_NO_DEVICE,
	DS2431_ERR_VERIFY,
	DS2431_ERR_PARAM
} DS2431Error;

// Сканирование шины и сохранение ROM всех найденных DS2431 (шина 1-Wire уже инициализирована).
// false — устройств семейства не найдено (LastError = NO_DEVICE).
Boolean DS2431_Init();

Int16U DS2431_GetDeviceCount();
DS2431Error DS2431_GetLastError();

// Установить ROM-адрес устройства
// После вызова адресация идёт через MATCH ROM вместо SKIP ROM.
void DS2431_Begin(Int8U serialNumber[DS2431_MAC_SIZE]);

// Чтение нескольких байт из EEPROM. false — нет ответа на шине.
Boolean DS2431_Read(Int16U address, Int8U *buf, Int16U len);

// Запись до 8 байт; address должен быть кратен 8.
// После вызова шина 1-Wire должна быть обесточена (OneWire_Depower вызывается внутри).
Boolean DS2431_Write(Int16U address, const Int8U *buf, Int16U count, Boolean verify);

// Запись 0xFF во все 16 строк данных (0x00..0x7F). Регистры защиты (0x80..) не затрагиваются.
// В EPROM-режиме биты только 1→0: уже запрограммированные ячейки нельзя вернуть в 0xFF.
// verify=true — проверка scratchpad перед copy; на запрограммированной памяти erase вернёт false.
Boolean DS2431_EraseAll(Int8U deviceIndex, Boolean verify);

// Чтение массива с адреса 0. len не должен превышать DS2431_EEPROM_SIZE.
Boolean DS2431_ReadArray(Int8U deviceIndex, Int8U *buf, Int16U len);

// Запись массива с адреса 0. len не должен превышать DS2431_EEPROM_SIZE.
Boolean DS2431_WriteArray(Int8U deviceIndex, const Int8U *buf, Int16U len);

#endif /* DS2431_H_ */
