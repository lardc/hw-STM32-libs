/**
 * @file BCCIMaster.c
 * @brief Бибиблиотека интерфейса Master BCCI
*/

// Header
#include "BCCIMaster.h"

// Includes
#include "BCCIxParams.h"
#include "CRC16.h"
#include "SysConfig.h"
#include "DeviceProfile.h"
#include "ZwIWDG.h"

// Definitions
#define Master_MBOX_W_16			0
#define Master_MBOX_W_16_A			1
#define Master_MBOX_R_16			2
#define Master_MBOX_R_16_A			3
#define Master_MBOX_C				4
#define Master_MBOX_C_A				5
#define Master_MBOX_ERR_A			6
#define Master_MBOX_RB_16			7
#define Master_MBOX_RB_16_A			8
#define Master_MBOX_WB_16			9
#define Master_MBOX_WB_16_A			10
#define Master_MBOX_R_F				11
#define Master_MBOX_R_F_A			12
#define Master_MBOX_W_F				13
#define Master_MBOX_W_F_A			14
#define Master_MBOX_RB_F			15
#define Master_MBOX_RB_F_A			16
#define Master_MBOX_RLIM_F			17
#define Master_MBOX_RLIM_F_A		18
#define Master_MBOX_BP				40
#define Master_MBOX_BP_A			41

#define MIN(a, b) 					(((a) < (b)) ? (a) : (b))

// Forward functions
void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node);
Int16U BCCIM_WaitResponse(pBCCIM_Interface Interface, Int16U Mailbox);
void BCCIM_ReadBlock16Subfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start);
Boolean BCCIM_HandleReadBlock16(pBCCIM_Interface Interface);
void BCCIM_ReadBlockFloatSubfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start);
Boolean BCCIM_HandleReadBlockFloat(pBCCIM_Interface Interface);
void BCCIM_WaitBroadcastResponse(pBCCIM_Interface Interface, pInt16U NodeArray, pInt16U NodeArraySize);

// Variables
Int16U BCCIM_ReadBlockBufferCounter;
float BCCIM_ReadBlockFloatBuffer[READ_BLOCK_FLOAT_BUFFER_SIZE];
pInt16U BCCIM_ReadBlock16Buffer = (pInt16U)BCCIM_ReadBlockFloatBuffer;
static Int16U ReadBlockSavedEndpoint, ReadBlockSavedNode;
static Int16U SavedErrorDetails = 0;

// Functions
/** 
 * @brief Инициализация Master BCCI интерфейса - сетап Mailbox для записи и чтения, запись Mailbox в IOConfig.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param IOConfig - Указатель на структуру, содержащую функции обратного вызова. Эти функции передаются в интерфейс.
 * @param MessageTimeoutTicks - Величина таймаута при обмене сообщениями по CAN, в миллисекундах.
 * @param pTimer - Указатель на системный таймер, в миллисекундах.
*/
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks,
		volatile Int64U *pTimer)
{
#ifdef CAN_MASTER_NID
	const Int16U NodeID = CAN_MASTER_NID;
#elif defined(CAN_SLAVE_NID)
	const Int16U NodeID = CAN_SLAVE_NID;
#elif defined(CAN_SALVE_NID)
	const Int16U NodeID = CAN_SALVE_NID;
#elif defined(CAN_NID)
	const Int16U NodeID = CAN_NID;
#endif

	BCCIM_InitWithNodeID(Interface, IOConfig, MessageTimeoutTicks, pTimer, NodeID);
}
// ----------------------------------------

/** 
 * @brief Инициализация Master BCCI интерфейса для конкретного узла(NodeID).
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param IOConfig - Указатель на структуру, содержащую функции обратного вызова. Эти функции передаются в интерфейс.
 * @param MessageTimeoutTicks - Величина таймаута при обмене сообщениями по CAN, в миллисекундах.
 * @param pTimer - Указатель на системный таймер, в миллисекундах.
 * @param NodeID - ID узла в CAN сети.
*/
void BCCIM_InitWithNodeID(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks,
		volatile Int64U *pTimer, Int16U NodeID)
{
	// Save parameters
	Interface->IOConfig = IOConfig;
	Interface->TimeoutValueTicks = MessageTimeoutTicks;
	Interface->pTimerCounter = pTimer;

	// Setup messages
	const Int32U MasterFilterID = (Int32U)NodeID << CAN_MASTER_NID_MPY;
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16,		MasterFilterID + CAN_ID_R_16,		2, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16_A,	MasterFilterID + CAN_ID_R_16 + 1,	4, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16,		MasterFilterID + CAN_ID_W_16,		4, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16_A,	MasterFilterID + CAN_ID_W_16 + 1,	2, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C,		MasterFilterID + CAN_ID_CALL,		2, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C_A,		MasterFilterID + CAN_ID_CALL + 1,	2, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_ERR_A,	MasterFilterID + CAN_ID_ERR,		4, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_16,	MasterFilterID + CAN_ID_RB_16,		2, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_16_A,	MasterFilterID + CAN_ID_RB_16 + 1,	8, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_WB_16,	MasterFilterID + CAN_ID_WB_16,		8, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_WB_16_A,	MasterFilterID + CAN_ID_WB_16 + 1,	2, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_F,		MasterFilterID + CAN_ID_R_F,		2, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_F_A,	MasterFilterID + CAN_ID_R_F + 1,	6, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_F,		MasterFilterID + CAN_ID_W_F,		6, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_F_A,	MasterFilterID + CAN_ID_W_F + 1,	2, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_F,	 	MasterFilterID + CAN_ID_RB_F,		2, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_F_A, 	MasterFilterID + CAN_ID_RB_F + 1,	8, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RLIM_F, 	MasterFilterID + CAN_ID_RLIM_F,		4, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RLIM_F_A,	MasterFilterID + CAN_ID_RLIM_F + 1, 6, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_BP,		MasterFilterID + CAN_ID_R_BP,		0, CAN_MBOX_TX, CAN_MASTER_NID_MASK);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_BP_A,		MasterFilterID + CAN_ID_A_BP,		0, CAN_MBOX_RX, CAN_MASTER_NID_MASK | CAN_FUNC_MASK);
}
// ----------------------------------------

/**
 * @brief Чтение 16-битного значения из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Register - Регистр блока из которого будет читаться значение.
 * @param Data - Указатель на буфер, в который будет записан результат.
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_Read16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, pInt16U Data)
{
	Int16U ret;
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_R_16_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Register;
	BCCIM_SendFrame(Interface, Master_MBOX_R_16, &message, Node);

	// Get response
	if ((ret = BCCIM_WaitResponse(Interface, Master_MBOX_R_16_A)) == ERR_NO_ERROR)
	{
		Interface->IOConfig->IO_GetMessage(Master_MBOX_R_16_A, &message);
		if (Data)
			*Data = message.HIGH.WORD.WORD_1;
	}

	return ret;
}
// ----------------------------------------

/**
 * @brief Чтение 32-битного float значения из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Register - Регистр блока из которого будет читаться значение.
 * @param Data - Указатель на буфер, в котором будет записан результат.
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_ReadFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float* Data)
{
	Int16U ret;
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_R_F_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Register;
	BCCIM_SendFrame(Interface, Master_MBOX_R_F, &message, Node);

	// Get response
	if ((ret = BCCIM_WaitResponse(Interface, Master_MBOX_R_F_A)) == ERR_NO_ERROR)
	{
		Interface->IOConfig->IO_GetMessage(Master_MBOX_R_F_A, &message);
		if (Data)
		{
			Int32U t_data = (Int32U)message.HIGH.WORD.WORD_1 << 16 | message.LOW.WORD.WORD_2;
			*Data = *(float *)(&t_data);
		}
	}

	return ret;
}
// ----------------------------------------

/**
 * @brief Чтение 32-битного float значения из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Register - Регистр блока из которого будет читаться значение.
 * @param ReadHighLimit - Если true - читается верхняя граница, если false - нижняя.
 * @param Data - Указатель на буфер, в котором будет записан результат.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
*/
Int16U BCCIM_ReadLimitFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Boolean ReadHighLimit, float* Data)
{
	Int16U ret;
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_RLIM_F_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Register;
	message.HIGH.WORD.WORD_1 = ReadHighLimit ? 1 : 0;
	BCCIM_SendFrame(Interface, Master_MBOX_RLIM_F, &message, Node);

	// Get response
	if ((ret = BCCIM_WaitResponse(Interface, Master_MBOX_RLIM_F_A)) == ERR_NO_ERROR)
	{
		Interface->IOConfig->IO_GetMessage(Master_MBOX_RLIM_F_A, &message);
		if (Data)
		{
			Int32U t_data = (Int32U)message.HIGH.WORD.WORD_1 << 16 | message.LOW.WORD.WORD_2;
			*Data = *(float *)(&t_data);
		}
	}

	return ret;
}
// ----------------------------------------

/**
 * @brief Запись 16-битного значения в регистр узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Register - Регистр блока в который будет записано значение.
 * @param Data - Значение, которое будет записано в регистр.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
*/
Int16U BCCIM_Write16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Int16U Data)
{
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_W_16_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Register;
	message.HIGH.WORD.WORD_1 = Data;
	BCCIM_SendFrame(Interface, Master_MBOX_W_16, &message, Node);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_W_16_A);
}
// ----------------------------------------

/**
 * @brief Запись 32-битного float значения в регистр узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Register - Регистр блока в котором будет записано значение.
 * @param Data - Значение, которое будет записано в регистр.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
*/
Int16U BCCIM_WriteFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float Data)
{
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_W_F_A, NULL);

	// Compose and send message
	Int32U t_data = *(pInt32U)(&Data);
	message.HIGH.WORD.WORD_0 = Register;
	message.HIGH.WORD.WORD_1 = t_data >> 16;
	message.LOW.WORD.WORD_2  = t_data & 0x0000FFFF;
	BCCIM_SendFrame(Interface, Master_MBOX_W_F, &message, Node);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_W_F_A);
}
// ----------------------------------------

/**
 * @brief Вызов функции узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Action - ID вызываемой функции.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
*/
Int16U BCCIM_Call(pBCCIM_Interface Interface, Int16U Node, Int16U Action)
{
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_C_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Action;
	BCCIM_SendFrame(Interface, Master_MBOX_C, &message, Node);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_C_A);
}
// ----------------------------------------

/**
 * @brief Чтение блока(массива) данных из узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Endpoint - Номер массива с данными.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
*/
Int16U BCCIM_ReadBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint)
{
	Int16U ret;
	Int64U timeout;
	BCCIM_ReadBlock16Subfunction(Interface, Node, Endpoint, TRUE);

	timeout = Interface->TimeoutValueTicks + *(Interface->pTimerCounter);
	while(*(Interface->pTimerCounter) < timeout)
	{
		// Get response
		if ((ret = BCCIM_WaitResponse(Interface, Master_MBOX_RB_16_A)) == ERR_NO_ERROR)
		{
			if (BCCIM_HandleReadBlock16(Interface))
				return ERR_NO_ERROR;
		}
		else
			return ret;
	}

	return ERR_TIMEOUT;
}
// ----------------------------------------

/**
 * @brief Запись блока(массива) данных в узел в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Endpoint - Номер массива с данными.
 * @param Data - Указатель на массив, который будет записан в узел.
 * @param DataLength - Размер записываемого массива.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
*/
Int16U BCCIM_WriteBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, pInt16U Data, Int16U DataLength)
{
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_WB_16_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = (Endpoint << 8) | DataLength;
	switch(DataLength)
	{
		case 3:
			message.LOW.WORD.WORD_3 = *(Data + 2);
		case 2:
			message.LOW.WORD.WORD_2 = *(Data + 1);
		case 1:
			message.HIGH.WORD.WORD_1 = *Data;
			break;
	}
	BCCIM_SendFrame(Interface, Master_MBOX_WB_16, &message, Node);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_WB_16_A);
}
// ----------------------------------------

/**
 * @brief Чтение блока(массива) данных в узле в CAN сети.
 * @details Если чтение происходит впервые, т. е. флаг Start = true, то производится.
 * очистка мейлбоксов ошибки и ответа, и запоминается переданный Endpoint, блок из которого идет чтение.
 * Если  Start = false, то производится чтение с Endpoint сохраненного в глобальной переменной ReadBlockSavedEndpoint.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Endpoint - Номер массива с данными.
 * @param Start - Флаг первого вызова функции.
*/
void BCCIM_ReadBlock16Subfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start)
{
	CANMessage message;

	if(Start)
	{
		// Clear input mailboxes
		Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
		Interface->IOConfig->IO_GetMessage(Master_MBOX_RB_16_A, NULL);

		ReadBlockSavedEndpoint = Endpoint;
		ReadBlockSavedNode = Node;
		BCCIM_ReadBlockBufferCounter = 0;
	}

	message.HIGH.WORD.WORD_0 = ReadBlockSavedEndpoint;
	BCCIM_SendFrame(Interface, Master_MBOX_RB_16, &message, ReadBlockSavedNode);
}
// ----------------------------------------

/**
 * @brief Обработка чтения блока(массива) данных из узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @return TRUE - если чтение производится впервые (CANInput.DLC = 0), FALSE - если мы получили Endpoint и размер считываемых данных.  
*/
Boolean BCCIM_HandleReadBlock16(pBCCIM_Interface Interface)
{
	CANMessage CANInput;
	Interface->IOConfig->IO_GetMessage(Master_MBOX_RB_16_A, &CANInput);

	if(BCCIM_ReadBlockBufferCounter >= READ_BLOCK_16_BUFFER_SIZE)
		return TRUE;

	switch(CANInput.DLC)
	{
		case 8:
			BCCIM_ReadBlock16Buffer[BCCIM_ReadBlockBufferCounter + 3] = CANInput.LOW.WORD.WORD_3;
		case 6:
			BCCIM_ReadBlock16Buffer[BCCIM_ReadBlockBufferCounter + 2] = CANInput.LOW.WORD.WORD_2;
		case 4:
			BCCIM_ReadBlock16Buffer[BCCIM_ReadBlockBufferCounter + 1] = CANInput.HIGH.WORD.WORD_1;
		case 2:
			BCCIM_ReadBlock16Buffer[BCCIM_ReadBlockBufferCounter] = CANInput.HIGH.WORD.WORD_0;
			BCCIM_ReadBlockBufferCounter += CANInput.DLC / 2;
			BCCIM_ReadBlock16Subfunction(Interface, 0, 0, FALSE);
			return FALSE;
		default:
			return TRUE;
	}
}
// ----------------------------------------

/**
 * @brief Чтение блока(массива) данных со значениями типа данных float из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Endpoint - Номер массива с данными.
 * @return ERR_NO_ERROR или ERR_TIMEOUT, если произошла ошибка таймаута.
*/
Int16U BCCIM_ReadBlockFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint)
{
	Int16U ret;
	Int64U timeout;
	BCCIM_ReadBlockFloatSubfunction(Interface, Node, Endpoint, TRUE);

	timeout = Interface->TimeoutValueTicks + *(Interface->pTimerCounter);
	while(*(Interface->pTimerCounter) < timeout)
	{
		// Get response
		if ((ret = BCCIM_WaitResponse(Interface, Master_MBOX_RB_F_A)) == ERR_NO_ERROR)
		{
			if (BCCIM_HandleReadBlockFloat(Interface))
				return ERR_NO_ERROR;
		}
		else
			return ret;
	}

	return ERR_TIMEOUT;
}
// ----------------------------------------

/**
 * @brief Чтение блока(массива) данных со значениями типа данных float из регистра узла в CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Node - ID узла в CAN сети.
 * @param Endpoint - Номер массива с данными.
*/
void BCCIM_ReadBlockFloatSubfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start)
{
	CANMessage message;

	if(Start)
	{
		// Clear input mailboxes
		Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
		Interface->IOConfig->IO_GetMessage(Master_MBOX_RB_F_A, NULL);

		ReadBlockSavedEndpoint = Endpoint;
		ReadBlockSavedNode = Node;
		BCCIM_ReadBlockBufferCounter = 0;
	}

	message.HIGH.WORD.WORD_0 = ReadBlockSavedEndpoint;
	BCCIM_SendFrame(Interface, Master_MBOX_RB_F, &message, ReadBlockSavedNode);
}
// ----------------------------------------

/**
 * @brief Обработка получения блока(массива) данных с значениями типа float из CAN сети.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @return TRUE, если чтение происходит впервые (CANInput.DLC = 0), FALSE в противном случае.
*/
Boolean BCCIM_HandleReadBlockFloat(pBCCIM_Interface Interface)
{
	CANMessage CANInput;
	Interface->IOConfig->IO_GetMessage(Master_MBOX_RB_F_A, &CANInput);

	if(BCCIM_ReadBlockBufferCounter >= READ_BLOCK_FLOAT_BUFFER_SIZE)
		return TRUE;

	pInt32U buffer = (pInt32U)BCCIM_ReadBlockFloatBuffer;
	Int16U length = CANInput.DLC / 4;
	switch(length)
	{
		case 2:
			buffer[BCCIM_ReadBlockBufferCounter + 1] = CANInput.LOW.DWORD_1;
		case 1:
			buffer[BCCIM_ReadBlockBufferCounter] = CANInput.HIGH.DWORD_0;

			BCCIM_ReadBlockBufferCounter += length;
			BCCIM_ReadBlockFloatSubfunction(Interface, 0, 0, FALSE);
			return FALSE;
		default:
			return TRUE;
	}
}
// ----------------------------------------

void BCCIM_ReadBlock16Load(pInt16U DataArray, Int16U DataSize, pInt16U DataRead)
{
	Int16U i, Counter = MIN(DataSize, BCCIM_ReadBlockBufferCounter);

	for (i = 0; i < Counter; ++i)
		DataArray[i] = BCCIM_ReadBlock16Buffer[i];

	*DataRead = Counter;
}
// ----------------------------------------

/**
 * @brief Отправка CAN сообщения.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Mailbox - ID мейлбокса внутри узла в который будет отправлено сообщение.
 * @param Message - Указатель на структуру CAN сообщения.
 * @param Node - ID узла в CAN сети.
*/
void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node)
{
	Message->MsgID.all = Node << CAN_SLAVE_NID_MPY;
	Interface->IOConfig->IO_SendMessageEx(Mailbox, Message, TRUE, FALSE);
}
// ----------------------------------------

/**
 * @brief Отправка широковещательного запроса
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param NodeArray - Указатель на массив в который будут записаны полученные Node ID узлов, ответивших на запрос
 * @param NodeArraySize - Указатель на переменную, в которую будет записан размер массива, после записи всех Node ID
*/
void BCCIM_SendBroadcastPing(pBCCIM_Interface Interface, pInt16U NodeArray, pInt16U NodeArraySize)
{
	CANMessage message;
	Interface->IOConfig->IO_SendMessageEx(Master_MBOX_BP, &message, FALSE, FALSE);
	BCCIM_WaitBroadcastResponse(Interface, NodeArray, NodeArraySize);
}
// ----------------------------------------

/**
 * @brief Ожидание ответа от узла, полученного по мейлбоксу Mailbox.
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param Mailbox - ID мейлбокса внутри узла, с которым собираемся общаться, в который будет отправлено сообщение.
 * @return Код ошибки, если она произошла, в ином случае ERR_NO_ERROR.
 * @retval ERR_NO_ERROR - Ответ получен.
 * @retval ERR_TIMEOUT - Тайм-аут.
*/
Int16U BCCIM_WaitResponse(pBCCIM_Interface Interface, Int16U Mailbox)
{
	Int64U timeout;
	CANMessage message;

	// Wait for response
	timeout = Interface->TimeoutValueTicks + *(Interface->pTimerCounter);
	while(*(Interface->pTimerCounter) < timeout)
	{
		// In case of error
		if (Interface->IOConfig->IO_IsMessageReceived(Master_MBOX_ERR_A, NULL))
		{
			Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, &message);
			SavedErrorDetails = message.HIGH.WORD.WORD_1;
			return message.HIGH.WORD.WORD_0;
		}
		else if (Interface->IOConfig->IO_IsMessageReceived(Mailbox, NULL))
			return ERR_NO_ERROR;

		IWDG_Refresh();
	}

	return ERR_TIMEOUT;
}
// ----------------------------------------

/**
 * @brief Ожидание ответа на широковещательный запрос
 * @param Interface - Указатель на структуру, хранящую параметры CAN-интерфейса (таймаут и функции обратного вызова).
 * @param NodeArray - Указатель на массив в который будут записаны полученные Node ID узлов, ответивших на запрос
 * @param NodeArraySize - Указатель на переменную, в которую будет записан размер массива, после записи всех Node ID
*/
void BCCIM_WaitBroadcastResponse(pBCCIM_Interface Interface, pInt16U NodeArray, pInt16U NodeArraySize)
{
	Int64U timeout;
	CANMessage message;
	Int16U currentNodeCount = 0;

	timeout = BR_TIMEOUT + *(Interface->pTimerCounter);

	while (*(Interface->pTimerCounter) < timeout)
	{
		if (Interface->IOConfig->IO_IsMessageReceived(Master_MBOX_BP_A, NULL))
		{
			Interface->IOConfig->IO_GetMessage(Master_MBOX_BP_A, &message);

			NodeArray[currentNodeCount++] = (message.MsgID.all & CAN_SLAVE_NID_MASK) >> 10;

			timeout = *(Interface->pTimerCounter) + BR_TIMEOUT;
		}
	}
	*NodeArraySize = currentNodeCount;
}
// ----------------------------------------

/**
 * @brief Возвращает код ошибки при чтении или записи.
*/
Int16U BCCIM_GetSavedErrorDetails()
{
	return SavedErrorDetails;
}
// ----------------------------------------

