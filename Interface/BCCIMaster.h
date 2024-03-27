/** 
 * @file BCCIMaster.h
 * @brief Бибиблиотека интерфейса Master BCCI
*/

#ifndef __BCCI_MASTER_H
#define __BCCI_MASTER_H

// Include
#include "stdinc.h"
#include "BCCITypes.h"

// Definitions
#define READ_BLOCK_FLOAT_BUFFER_SIZE	5000
#define READ_BLOCK_16_BUFFER_SIZE		READ_BLOCK_FLOAT_BUFFER_SIZE * 2

// Variables
extern pInt16U	BCCIM_ReadBlock16Buffer;
extern Int16U	BCCIM_ReadBlockBufferCounter;
extern float	BCCIM_ReadBlockFloatBuffer[];

// Types
/** BCCI instance state */
typedef struct __BCCIM_Interface
{
	pBCCI_IOConfig IOConfig;			///< Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
	Int32U TimeoutValueTicks;			///< Величина таймаута при обмене сообщениями по CAN, в миллисекундах
	volatile Int64U *pTimerCounter;		///< Указатель на системный таймер, в миллисекундах
} BCCIM_Interface, *pBCCIM_Interface;

// Functions

/** 
 * @fn void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer)
 * @brief Инициализация Master BCCI интерфейса - сетап Mailbox для записи и чтения, запись Mailbox в IOConfig.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param IOConfig - Указатель на структуру, содержащую функции обратного вызова. Эти функции передаются в интерфейс.
 * @param MessageTimeoutTicks - Величина таймаута при обмене сообщениями по CAN, в миллисекундах
 * @param pTimer - Указатель на системный таймер, в миллисекундах
*/
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer);

/** 
 * @fn void BCCIM_InitWithNodeID(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer, Int16U NodeID)
 * @brief Инициализация Master BCCI интерфейса для конкретного узла(NodeID)
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param IOConfig - Указатель на структуру, содержащую функции обратного вызова. Эти функции передаются в интерфейс.
 * @param MessageTimeoutTicks - Величина таймаута при обмене сообщениями по CAN, в миллисекундах
 * @param pTimer - Указатель на системный таймер, в миллисекундах
 * @param NodeID - ID узла в CAN сети
*/
void BCCIM_InitWithNodeID(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks,
		volatile Int64U *pTimer, Int16U NodeID);

/**
 * @fn Int16U BCCIM_Read16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, pInt16U Data)
 * @brief Чтение 16-битного значения из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Register - Регистр блока из которого будет читаться значение
 * @param Data - Указатель на буфер, в который будет записан результат
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_Read16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, pInt16U Data);

/**
 * @fn Int16U BCCIM_ReadFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float* Data)
 * @brief Чтение 32-битного float значения из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Register - Регистр блока из которого будет читаться значение
 * @param Data - Указатель на буфер, в котором будет записан результат
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_ReadFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float* Data);

/**
 * @fn BCCIM_ReadLimitFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Boolean ReadHighLimit, float* Data)
 * @brief Чтение 32-битного float значения из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Register - Регистр блока из которого будет читаться значение
 * @param ReadHighLimit - Если true - читается верхняя граница, если false - нижняя
 * @param Data - Указатель на буфер, в котором будет записан результат
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_ReadLimitFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Boolean ReadHighLimit, float* Data);

/**
 * @fn Int16U BCCIM_Write16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Int16U Data)
 * @brief Запись 16-битного значения в регистр узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Register - Регистр блока в который будет записано значение
 * @param Data - Значение, которое будет записано в регистр
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_Write16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Int16U Data);

/**
 * @fn Int16U BCCIM_WriteFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float Data)
 * @brief Запись 32-битного float значения в регистр узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Register - Регистр блока в котором будет записано значение
 * @param Data - Значение, которое будет записано в регистр
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_WriteFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float Data);

/**
 * @fn Int16U BCCIM_Call(pBCCIM_Interface Interface, Int16U Node, Int16U Action)
 * @brief Вызов функции узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Action - ID вызываемой функции
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_Call(pBCCIM_Interface Interface, Int16U Node, Int16U Action);

/**
 * @fn Int16U BCCIM_ReadBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint)
 * @brief Чтение блока(массива) данных из узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Endpoint - Регистр начала массива
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_ReadBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint);

/**
 * @fn Int16U BCCIM_WriteBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, pInt16U Data, Int16U DataLength)
 * @brief Запись блока(массива) данных в узел в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Endpoint - Регистр начала массива
 * @param Data - Указатель на массив, который будет записан в узел
 * @param DataLength - Размер записываемого массива
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_WriteBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, pInt16U Data, Int16U DataLength);

/**
 * @fn Int16U BCCIM_ReadBlockFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint)
 * @brief Чтение блока(массива) данных со значениями типа данных float из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Endpoint - Регистр начала массива
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_ReadBlockFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint);

/**
 * @fn Int16U BCCIM_WriteBlockFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, pFloat Data, Int16U DataLength)
 * @brief Запись блока(массива) данных со значениями типа данных float в регистр узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети
 * @param Endpoint - Регистр начала массива
 * @param Data - Указатель на массив, который будет записан в узел
 * @param DataLength - Размер записываемого массива
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_WriteBlockFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, pFloat Data, Int16U DataLength);

void BCCIM_ReadBlock16Load(pInt16U DataArray, Int16U DataSize, pInt16U DataRead);

/**
 * @fn Int16U BCCIM_GetSavedErrorDetails()
 * @brief Возвращает сохраненную информацию об ошибке чтения или записи
*/
Int16U BCCIM_GetSavedErrorDetails();

#endif // __BCCI_MASTER_H
