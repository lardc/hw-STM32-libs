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
#define Master_MBOX_W_16			1
#define Master_MBOX_W_16_A			2
#define Master_MBOX_W_32			3
#define Master_MBOX_W_32_A			4
#define Master_MBOX_R_16			5
#define Master_MBOX_R_16_A			6
#define Master_MBOX_R_32			7
#define Master_MBOX_R_32_A			8
#define Master_MBOX_C				9
#define Master_MBOX_C_A				10
#define Master_MBOX_ERR_A			11


// Forward functions
//
void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node, Int16U Command);
Int16U BCCIM_WaitResponse(pBCCIM_Interface Interface, Int16U Mailbox);


// Functions
//
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer)
{
	// Save parameters
	Interface->IOConfig = IOConfig;
	Interface->TimeoutValueTicks = MessageTimeoutTicks;
	Interface->pTimerCounter = pTimer;

	// Setup messages
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16,		(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_R_16,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_R_16_A,	(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_R_16 + 1,	4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16,		(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_W_16,		4);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_W_16_A,	(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_W_16 + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C,		(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_CALL,		2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_C_A,		(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_CALL + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Master_MBOX_ERR_A,	(CAN_MASTER_NID << CAN_SLAVE_NID_MPY) + CAN_ID_ERR,			4);
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
	BCCIM_SendFrame(Interface, Master_MBOX_R_16, &message, Node, CAN_ID_R_16);

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
	BCCIM_SendFrame(Interface, Master_MBOX_W_16, &message, Node, CAN_ID_W_16);

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
	BCCIM_SendFrame(Interface, Master_MBOX_C, &message, Node, CAN_ID_CALL);

	// Get response
	return BCCIM_WaitResponse(Interface, Master_MBOX_C_A);
}
// ----------------------------------------

void BCCIM_SendFrame(pBCCIM_Interface Interface, Int16U Mailbox, pCANMessage Message, Int32U Node, Int16U Command)
{
	Message->MsgID.all = (CAN_MASTER_NID << CAN_MASTER_NID_MPY) | (Node << CAN_SLAVE_NID_MPY) | Command;
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
			return message.HIGH.WORD.WORD_0;
		}
		else if (Interface->IOConfig->IO_IsMessageReceived(Mailbox, NULL))
			return ERR_NO_ERROR;
	}

	return ERR_TIMEOUT;
}
// ----------------------------------------
