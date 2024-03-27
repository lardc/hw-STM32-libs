/** 
 * @file SCCISlave.h
 * @brief SCCI-Slave communication interface
*/ 

#ifndef __SCCIS_H
#define __SCCIS_H

// Include
#include "stdinc.h"
//
#include "SCCIParams.h"
#include "xCCICommon.h"
#include "SCCITypes.h"
#include "DataTable.h"

// Functions
//

/**
 * @fn void SCCI_Init(pSCCI_Interface Interface, pSCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig, pInt16U DataTable, Int16U DataTableSize, Int32U MessageTimeoutTicks, void *ArgumentForCallback)
 * @brief Инициализация Slave SCCI интерфейса - установка всех параметров по умолчанию, получение дата тейбла.
 * @param Interface - Указатель на структуру, хранящую дата тейбл, таймеры таймаута.
 * @param IOConfig - Указатель на структуру, содержащую функции обратного вызова. Эти функции передаются в интерфейс.
 * @param ServiceConfig - Указатель на структуру, хранящую параметры службы SCCI
 * @param DataTable - Указатель на дата тейбл
 * @param DataTableSize - Размер дата тейбла.
 * @param MessageTimeoutTicks - Величина таймаута при обмене сообщениями по rs232, в миллисекундах
 * @param ArgumentForCallback - Указатель на аргумент, передаваемый в функцию обратного вызова
*/
void SCCI_Init(pSCCI_Interface Interface, pSCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig, pInt16U DataTable,
		Int16U DataTableSize, Int32U MessageTimeoutTicks, void *ArgumentForCallback);

/**
 * @fn void SCCI_Process(pSCCI_Interface Interface, Int64U CurrentTickCount, Boolean MaskStateChangeOperations)
 * @brief Передача пакетов из таблицы данных в интерфейс
 * @param Interface - Указатель на структуру, хранящую дата тейбл, таймеры таймаута.
 * @param CurrentTickCount - Текущее значение системного таймера
 * @param MaskStateChangeOperations - Маска состояний изменений
*/
void SCCI_Process(pSCCI_Interface Interface, Int64U CurrentTickCount, Boolean MaskStateChangeOperations);

// Create protected area
Int16U SCCI_AddProtectedArea(pSCCI_Interface Interface, Int16U StartAddress, Int16U EndAddress);

// Remove protected area
Boolean SCCI_RemoveProtectedArea(pSCCI_Interface Interface, Int16U AreaIndex);

// Register read endpoint 16-bit source callback
Boolean SCCI_RegisterReadEndpoint16(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpoint16 ReadCallback);

// Register read endpoint float source callback
Boolean SCCI_RegisterReadEndpointFloat(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpointFloat ReadCallback);

// Register write endpoint 16-bit destination callback
Boolean SCCI_RegisterWriteEndpoint16(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackWriteEndpoint16 WriteCallback);

void MemZero16(uint16_t *Data, uint16_t Number);
void MemCopy16(uint16_t *Source, uint16_t *Destination, uint16_t Length);

#endif // __SCCIS_H
