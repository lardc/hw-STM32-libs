// -----------------------------------------
// BCCI communication interface
// ----------------------------------------

// Header
#include "BCCIMaster.h"
//
// Includes
#include "BCCIxParams.h"
#include "CRC16.h"
#include "SysConfig.h"
#include "DeviceProfile.h"


// Definitions
//
#define Master_MBOX_W_16			0
#define Master_MBOX_W_16_A			1
#define Master_MBOX_R_16			2
#define Master_MBOX_R_16_A			3
#define Master_MBOX_C				4
#define Master_MBOX_C_A				5
#define Master_MBOX_ERR_A			6
#define Master_MBOX_RB_16			7
#define Master_MBOX_RB_16_A			8
#define Master_MBOX_R_F				9
#define Master_MBOX_R_F_A			10
#define Master_MBOX_W_F				11
#define Master_MBOX_W_F_A			12
#define Master_MBOX_RB_F			13
#define Master_MBOX_RB_F_A			14
#define Master_MBOX_RLIM_F			15
#define Master_MBOX_RLIM_F_A		16

#define MIN(a, b) 					(((a) < (b)) ? (a) : (b))


// Forward functions
//
void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node);
Int16U BCCIM_WaitResponse(pBCCIM_Interface Interface, Int16U Mailbox);
void BCCIM_ReadBlock16Subfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start);
Boolean BCCIM_HandleReadBlock16(pBCCIM_Interface Interface);


// Variables
//
static Int16U ReadBlock16Buffer[READ_BLOCK_16_BUFFER_SIZE];
static Int16U ReadBlock16BufferCounter, ReadBlock16SavedEndpoint, ReadBlock16SavedNode;
static Int16U SavedErrorDetails = 0;


// Functions
//
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer)
{
	// Save parameters
	Interface->IOConfig = IOConfig;
	Interface->TimeoutValueTicks = MessageTimeoutTicks;
	Interface->pTimerCounter = pTimer;

	// Setup messages
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16,		CAN_MASTER_FILTER_ID + CAN_ID_R_16,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16_A,	CAN_MASTER_FILTER_ID + CAN_ID_R_16 + 1,	4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16,		CAN_MASTER_FILTER_ID + CAN_ID_W_16,		4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16_A,	CAN_MASTER_FILTER_ID + CAN_ID_W_16 + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C,		CAN_MASTER_FILTER_ID + CAN_ID_CALL,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C_A,		CAN_MASTER_FILTER_ID + CAN_ID_CALL + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_ERR_A,	CAN_MASTER_FILTER_ID + CAN_ID_ERR,		4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_16,	CAN_MASTER_FILTER_ID + CAN_ID_RB_16,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_RB_16_A,	CAN_MASTER_FILTER_ID + CAN_ID_RB_16 + 1,8);
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

void BCCIM_ReadBlock16Subfunction(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, Boolean Start)
{
	CANMessage message;

	if(Start)
	{
		// Clear input mailboxes
		Interface->IOConfig->IO_GetMessage(Master_MBOX_ERR_A, NULL);
		Interface->IOConfig->IO_GetMessage(Master_MBOX_RB_16_A, NULL);

		ReadBlock16SavedEndpoint = Endpoint;
		ReadBlock16SavedNode = Node;
		ReadBlock16BufferCounter = 0;
	}

	message.HIGH.WORD.WORD_0 = ReadBlock16SavedEndpoint;
	BCCIM_SendFrame(Interface, Master_MBOX_RB_16, &message, ReadBlock16SavedNode);
}
// ----------------------------------------

Boolean BCCIM_HandleReadBlock16(pBCCIM_Interface Interface)
{
	CANMessage CANInput;
	Interface->IOConfig->IO_GetMessage(Master_MBOX_RB_16_A, &CANInput);

	if(ReadBlock16BufferCounter >= READ_BLOCK_16_BUFFER_SIZE)
		return TRUE;

	switch(CANInput.DLC)
	{
		case 8:
			ReadBlock16Buffer[ReadBlock16BufferCounter + 3] = CANInput.LOW.WORD.WORD_3;
		case 6:
			ReadBlock16Buffer[ReadBlock16BufferCounter + 2] = CANInput.LOW.WORD.WORD_2;
		case 4:
			ReadBlock16Buffer[ReadBlock16BufferCounter + 1] = CANInput.HIGH.WORD.WORD_1;
		case 2:
			ReadBlock16Buffer[ReadBlock16BufferCounter] = CANInput.HIGH.WORD.WORD_0;
			ReadBlock16BufferCounter += CANInput.DLC / 2;
			BCCIM_ReadBlock16Subfunction(Interface, 0, 0, FALSE);
			return FALSE;
		default:
			return TRUE;
	}
}
// ----------------------------------------

void BCCIM_ReadBlock16Load(pInt16U DataArray, Int16U DataSize, pInt16U DataRead)
{
	Int16U i, Counter = MIN(DataSize, ReadBlock16BufferCounter);

	for (i = 0; i < Counter; ++i)
		DataArray[i] = ReadBlock16Buffer[i];

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
	}

	return ERR_TIMEOUT;
}
// ----------------------------------------

Int16U BCCIM_GetSavedErrorDetails()
{
	return SavedErrorDetails;
}
// ----------------------------------------
