// ----------------------------------------
// BCCI communication interface
// ----------------------------------------

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

#define MIN(a, b) 					(((a) < (b)) ? (a) : (b))

// Forward functions
void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node);
Int16U BCCIM_WaitResponse(pBCCIM_Interface Interface, Int16U Mailbox);
void BCCIM_ReadBlock16Subfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start);
Boolean BCCIM_HandleReadBlock16(pBCCIM_Interface Interface);
void BCCIM_ReadBlockFloatSubfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start);
Boolean BCCIM_HandleReadBlockFloat(pBCCIM_Interface Interface);

// Variables
Int16U BCCIM_ReadBlockBufferCounter;
float BCCIM_ReadBlockFloatBuffer[READ_BLOCK_FLOAT_BUFFER_SIZE];
pInt16U BCCIM_ReadBlock16Buffer = (pInt16U)BCCIM_ReadBlockFloatBuffer;
static Int16U ReadBlockSavedEndpoint, ReadBlockSavedNode;
static Int16U SavedErrorDetails = 0;

// Functions
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks,
		volatile Int64U *pTimer)
{
#ifdef CAN_NID
	const Int16U NodeID = CAN_NID;
#else
	const Int16U NodeID = CAN_MASTER_NID;
#endif

	BCCIM_InitWithNodeID(Interface, IOConfig, MessageTimeoutTicks, pTimer, NodeID);
}
// ----------------------------------------

void BCCIM_InitWithNodeID(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks,
		volatile Int64U *pTimer, Int16U NodeID)
{
	// Save parameters
	Interface->IOConfig = IOConfig;
	Interface->TimeoutValueTicks = MessageTimeoutTicks;
	Interface->pTimerCounter = pTimer;

	// Setup messages
	Int32U MasterFilterID = (Int32U)NodeID << CAN_MASTER_NID_MPY;
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16,		MasterFilterID + CAN_ID_R_16,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16_A,	MasterFilterID + CAN_ID_R_16 + 1,	4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16,		MasterFilterID + CAN_ID_W_16,		4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16_A,	MasterFilterID + CAN_ID_W_16 + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C,		MasterFilterID + CAN_ID_CALL,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C_A,		MasterFilterID + CAN_ID_CALL + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_ERR_A,	MasterFilterID + CAN_ID_ERR,		4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_16,	MasterFilterID + CAN_ID_RB_16,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_16_A,	MasterFilterID + CAN_ID_RB_16 + 1,	8);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_WB_16,	MasterFilterID + CAN_ID_WB_16,		8);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_WB_16_A,	MasterFilterID + CAN_ID_WB_16 + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_F,		MasterFilterID + CAN_ID_R_F,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_F_A,	MasterFilterID + CAN_ID_R_F + 1,	6);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_F,		MasterFilterID + CAN_ID_W_F,		6);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_F_A,	MasterFilterID + CAN_ID_W_F + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_F,	 	MasterFilterID + CAN_ID_RB_F,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_F_A, 	MasterFilterID + CAN_ID_RB_F + 1,	8);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RLIM_F, 	MasterFilterID + CAN_ID_RLIM_F,		4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RLIM_F_A,	MasterFilterID + CAN_ID_RLIM_F + 1, 6);
}
// ----------------------------------------

Int16U BCCIM_Read16(pBCCIM_Interface Interface, Int16U Node, Int16U Address, pInt16U Data)
{
	Int16U ret;
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_R_16_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Address;
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

Int16U BCCIM_ReadFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Address, float* Data)
{
	Int16U ret;
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_R_F_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Address;
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

Int16U BCCIM_ReadLimitFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Address, Boolean ReadHighLimit, float* Data)
{
	Int16U ret;
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_RLIM_F_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Address;
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

Int16U BCCIM_Write16(pBCCIM_Interface Interface, Int16U Node, Int16U Address, Int16U Data)
{
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_W_16_A, NULL);

	// Compose and send message
	message.HIGH.WORD.WORD_0 = Address;
	message.HIGH.WORD.WORD_1 = Data;
	BCCIM_SendFrame(Interface, Master_MBOX_W_16, &message, Node);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_W_16_A);
}
// ----------------------------------------

Int16U BCCIM_WriteFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Address, float Data)
{
	CANMessage message;

	// Clear input mailboxes
	Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
	Interface->IOConfig->IO_GetMessage(Master_MBOX_W_F_A, NULL);

	// Compose and send message
	Int32U t_data = *(pInt32U)(&Data);
	message.HIGH.WORD.WORD_0 = Address;
	message.HIGH.WORD.WORD_1 = t_data >> 16;
	message.LOW.WORD.WORD_2  = t_data & 0x0000FFFF;
	BCCIM_SendFrame(Interface, Master_MBOX_W_F, &message, Node);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_W_F_A);
}
// ----------------------------------------

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

void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node)
{
	Message->MsgID.all = Node << CAN_SLAVE_NID_MPY;
	Interface->IOConfig->IO_SendMessageEx(Mailbox, Message, TRUE, FALSE);
}
// ----------------------------------------

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

Int16U BCCIM_GetSavedErrorDetails()
{
	return SavedErrorDetails;
}
// ----------------------------------------
