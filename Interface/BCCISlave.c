// ----------------------------------------
// BCCI-Slave communication interface
// ----------------------------------------

// Header
#include "BCCISlave.h"
//
// Includes
#include "BCCIxParams.h"
#include "CRC16.h"
#include "SysConfig.h"

// Definitions
typedef void (*xProcessFunction)(pBCCI_Interface Interface);

// Macro
//
#define BCCI_BLOCK_MAX_VAL_16_W		3
//
#define Slave_MBOX_W_16				21
#define Slave_MBOX_W_16_A			22
#define Slave_MBOX_W_32				23
#define Slave_MBOX_W_32_A			24
#define Slave_MBOX_R_16				25
#define Slave_MBOX_R_16_A			26
#define Slave_MBOX_R_32				27
#define Slave_MBOX_R_32_A			28
#define Slave_MBOX_C				29
#define Slave_MBOX_C_A				30
#define Slave_MBOX_ERR_A			31
#define Slave_MBOX_WB_16			32
#define Slave_MBOX_WB_16_A			33
#define Slave_MBOX_RB_16			34
#define Slave_MBOX_RB_16_A			35
#define Slave_MBOX_R_F				36
#define Slave_MBOX_R_F_A			37
#define Slave_MBOX_W_F				38
#define Slave_MBOX_W_F_A			39
#define Slave_MBOX_RB_F				40
#define Slave_MBOX_RB_F_A			41
#define Slave_MBOX_RLIM_F			42
#define Slave_MBOX_RLIM_F_A			43

// Forward functions
//
Boolean BCCI_ProcessX(pBCCI_Interface Interface, Boolean MaskStateChangeOperations, Int16U MBox, xProcessFunction Func);
//
void BCCI_SendErrorFrame(pBCCI_Interface Interface, CANMessage CANInput, Int16U ErrorCode, Int16U Details);
void BCCI_SendResponseFrame(pBCCI_Interface Interface, Int16U Mailbox, pCANMessage Message);
void BCCI_SendResponseFrameEx(pBCCI_Interface Interface, Int16U Mailbox, pCANMessage Message, Int16U MessageLength);
//
void BCCI_HandleRead16(pBCCI_Interface Interface);
void BCCI_HandleReadFloat(pBCCI_Interface Interface);
void BCCI_HandleWrite16(pBCCI_Interface Interface);
void BCCI_HandleWriteFloat(pBCCI_Interface Interface);
void BCCI_HandleReadLimitFloat(pBCCI_Interface Interface);
void BCCI_HandleCall(pBCCI_Interface Interface);
void BCCI_HandleReadBlock16(pBCCI_Interface Interface);
void BCCI_HandleReadBlockFloat(pBCCI_Interface Interface);
void BCCI_HandleWriteBlock16(pBCCI_Interface Interface);

// Functions
//
void BCCI_Init(pBCCI_Interface Interface, pBCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig, pInt16U DataTable,
		Int16U DataTableSize, void *ArgumentForCallback)
{
	Int16U i;
	
	// Reset fields
	Interface->ProtectionAndEndpoints.ProtectedAreasUsed = 0;
	
	for(i = 0; i < (xCCI_MAX_READ_ENDPOINTS + 1); ++i)
		Interface->ProtectionAndEndpoints.ReadEndpoints16[i] = NULL;
	
	for(i = 0; i < (xCCI_MAX_WRITE_ENDPOINTS + 1); ++i)
		Interface->ProtectionAndEndpoints.WriteEndpoints16[i] = NULL;
	
	// Save parameters
	Interface->IOConfig = IOConfig;
	Interface->ServiceConfig = ServiceConfig;
	Interface->DataTableAddress = DataTable;
	Interface->DataTableSize = DataTableSize;
	Interface->ArgForEPCallback = ArgumentForCallback;
	
	// Setup messages
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_W_16,		CAN_SLAVE_FILTER_ID + CAN_ID_W_16,		4);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_W_16_A,	CAN_SLAVE_FILTER_ID + CAN_ID_W_16 + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_R_16, 		CAN_SLAVE_FILTER_ID + CAN_ID_R_16,		2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_R_16_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_R_16 + 1,	4);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_C, 		CAN_SLAVE_FILTER_ID + CAN_ID_CALL,		2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_C_A, 		CAN_SLAVE_FILTER_ID + CAN_ID_CALL + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_ERR_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_ERR,		4);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_WB_16, 	CAN_SLAVE_FILTER_ID + CAN_ID_WB_16,		4);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_WB_16_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_WB_16 + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_RB_16, 	CAN_SLAVE_FILTER_ID + CAN_ID_RB_16,		2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_RB_16_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_RB_16 + 1,	8);
#ifdef USE_FLOAT_DT
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_W_F,		CAN_SLAVE_FILTER_ID + CAN_ID_W_F,		6);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_W_F_A,		CAN_SLAVE_FILTER_ID + CAN_ID_W_F + 1,	2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_R_F, 		CAN_SLAVE_FILTER_ID + CAN_ID_R_F,		2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_R_F_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_R_F + 1,	6);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_RB_F,	 	CAN_SLAVE_FILTER_ID + CAN_ID_RB_F,		2);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_RB_F_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_RB_F + 1,	8);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_RLIM_F, 	CAN_SLAVE_FILTER_ID + CAN_ID_RLIM_F,	4);
	Interface->IOConfig->IO_ConfigMailbox(Slave_MBOX_RLIM_F_A, 	CAN_SLAVE_FILTER_ID + CAN_ID_RLIM_F + 1, 6);
#endif
}
// ----------------------------------------

void BCCI_Process(pBCCI_Interface Interface, Boolean MaskStateChangeOperations)
{
	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_W_16, BCCI_HandleWrite16))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_R_16, BCCI_HandleRead16))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_C, BCCI_HandleCall))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_RB_16, BCCI_HandleReadBlock16))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_WB_16, BCCI_HandleWriteBlock16))
		return;
	
#ifdef USE_FLOAT_DT
	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_W_F, BCCI_HandleWriteFloat))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_R_F, BCCI_HandleReadFloat))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_RB_F, BCCI_HandleReadBlockFloat))
		return;

	if(BCCI_ProcessX(Interface, MaskStateChangeOperations, Slave_MBOX_RLIM_F, BCCI_HandleReadLimitFloat))
		return;
#endif
}
// ----------------------------------------

Boolean BCCI_ProcessX(pBCCI_Interface Interface, Boolean MaskStateChangeOperations, Int16U MBox, xProcessFunction Func)
{
	if(Interface->IOConfig->IO_IsMessageReceived(MBox, NULL))
	{
		if(MaskStateChangeOperations)
		{
			CANMessage CANInput;
			Interface->IOConfig->IO_GetMessage(MBox, &CANInput);
			BCCI_SendErrorFrame(Interface, CANInput, ERR_BLOCKED, 0);
		}
		else
			Func(Interface);

		return TRUE;
	}
	else
		return FALSE;
}
// ----------------------------------------

void BCCI_HandleRead16(pBCCI_Interface Interface)
{
	Int16U addr;
	CANMessage CANInput;
	
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_R_16, &CANInput);
	addr = CANInput.HIGH.WORD.WORD_0;
	
	if(addr >= Interface->DataTableSize)
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ADDESS, addr);
	}
	else
	{
		CANMessage CANOutput = CANInput;
		
		CANOutput.HIGH.WORD.WORD_0 = addr;
		CANOutput.HIGH.WORD.WORD_1 = Interface->DataTableAddress[addr];
		
		BCCI_SendResponseFrame(Interface, Slave_MBOX_R_16_A, &CANOutput);
	}
}
// ----------------------------------------

void BCCI_HandleReadFloat(pBCCI_Interface Interface)
{
	Int16U addr;
	CANMessage CANInput;

	Interface->IOConfig->IO_GetMessage(Slave_MBOX_R_F, &CANInput);
	addr = CANInput.HIGH.WORD.WORD_0;

	if(addr >= Interface->DataTableSize)
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ADDESS, addr);
	}
	else
	{
		CANMessage CANOutput = CANInput;
		Int32U data = ((pInt32U)(Interface->DataTableAddress))[addr];

		CANOutput.HIGH.WORD.WORD_0 = addr;
		CANOutput.HIGH.WORD.WORD_1 = data >> 16;
		CANOutput.LOW.WORD.WORD_2  = data & 0x0000FFFF;

		BCCI_SendResponseFrame(Interface, Slave_MBOX_R_F_A, &CANOutput);
	}
}
// ----------------------------------------

void BCCI_HandleWrite16(pBCCI_Interface Interface)
{
	Int16U addr, data;
	CANMessage CANInput;
	
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_W_16, &CANInput);
	addr = CANInput.HIGH.WORD.WORD_0;
	data = CANInput.HIGH.WORD.WORD_1;
	
	if(addr >= Interface->DataTableSize)
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ADDESS, addr);
	}
	else if(xCCI_InProtectedZone(&Interface->ProtectionAndEndpoints, addr))
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_PROTECTED, addr);
	}
	else if(Interface->ServiceConfig->ValidateCallback16 && !Interface->ServiceConfig->ValidateCallback16(addr, data))
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_VALIDATION, addr);
	}
	else
	{
		CANMessage CANOutput = CANInput;
		
		Interface->DataTableAddress[addr] = data;
		CANOutput.HIGH.WORD.WORD_0 = addr;
		
		BCCI_SendResponseFrame(Interface, Slave_MBOX_W_16_A, &CANOutput);
	}
}
// ----------------------------------------

void BCCI_HandleWriteFloat(pBCCI_Interface Interface)
{
	CANMessage CANInput;
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_W_F, &CANInput);

	Int16U addr = CANInput.HIGH.WORD.WORD_0;
	Int32U t_data = (Int32U)CANInput.HIGH.WORD.WORD_1 << 16 | CANInput.LOW.WORD.WORD_2;
	float data = *(float *)&t_data;

	if(addr >= Interface->DataTableSize)
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ADDESS, addr);
	}
	else if(xCCI_InProtectedZone(&Interface->ProtectionAndEndpoints, addr))
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_PROTECTED, addr);
	}
	else if(Interface->ServiceConfig->ValidateCallbackFloat
			&& !Interface->ServiceConfig->ValidateCallbackFloat(addr, data, NULL, NULL))
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_VALIDATION, addr);
	}
	else
	{
		CANMessage CANOutput = CANInput;

		((float *)Interface->DataTableAddress)[addr] = data;
		CANOutput.HIGH.WORD.WORD_0 = addr;

		BCCI_SendResponseFrame(Interface, Slave_MBOX_W_F_A, &CANOutput);
	}
}
// ----------------------------------------

void BCCI_HandleReadLimitFloat(pBCCI_Interface Interface)
{
	CANMessage CANInput;
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_RLIM_F, &CANInput);

	Int16U addr = CANInput.HIGH.WORD.WORD_0;
	Boolean UseHighLimit = CANInput.HIGH.WORD.WORD_1;
	float LowLimit = 0, HighLimit = 0;

	if(addr >= Interface->DataTableSize)
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ADDESS, addr);
	}
	else if(xCCI_InProtectedZone(&Interface->ProtectionAndEndpoints, addr))
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_PROTECTED, addr);
	}
	else if(Interface->ServiceConfig->ValidateCallbackFloat
			&& !Interface->ServiceConfig->ValidateCallbackFloat(addr, 0, &LowLimit, &HighLimit))
	{
		BCCI_SendErrorFrame(Interface, CANInput, ERR_VALIDATION, addr);
	}
	else
	{
		CANMessage CANOutput = CANInput;
		Int32U data = *(pInt32U)(UseHighLimit ? &HighLimit : &LowLimit);

		CANOutput.HIGH.WORD.WORD_0 = addr;
		CANOutput.HIGH.WORD.WORD_1 = data >> 16;
		CANOutput.LOW.WORD.WORD_2  = data & 0x0000FFFF;

		BCCI_SendResponseFrame(Interface, Slave_MBOX_RLIM_F_A, &CANOutput);
	}
}
// ----------------------------------------

void BCCI_HandleCall(pBCCI_Interface Interface)
{
	Int16U action;
	CANMessage CANInput;
	
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_C, &CANInput);
	action = CANInput.HIGH.WORD.WORD_0;
	
	if(Interface->ServiceConfig->UserActionCallback != NULL)
	{
		Int16U userError = 0;
		
		if(!Interface->ServiceConfig->UserActionCallback(action, &userError))
		{
			BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ACTION, action);
		}
		else if(userError != 0)
		{
			BCCI_SendErrorFrame(Interface, CANInput, ERR_USER, userError);
		}
		else
		{
			CANMessage CANOutput = CANInput;
			
			CANOutput.HIGH.WORD.WORD_0 = action;
			BCCI_SendResponseFrame(Interface, Slave_MBOX_C_A, &CANOutput);
		}
	}
	else
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ACTION, action);
}
// ----------------------------------------

void BCCI_HandleReadBlock16(pBCCI_Interface Interface)
{
	pInt16U src;
	Int16U epnt;
	CANMessage CANInput;
	
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_RB_16, &CANInput);
	epnt = CANInput.HIGH.WORD.WORD_0;
	
	if((epnt < xCCI_MAX_READ_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.ReadEndpoints16[epnt])
	{
		CANMessage CANOutput = CANInput;
		
		Int16U length = Interface->ProtectionAndEndpoints.ReadEndpoints16[epnt](epnt, &src, FALSE, FALSE,
				Interface->ArgForEPCallback, 4);
		
		switch(length)
		{
			case 4:
				CANOutput.LOW.WORD.WORD_3 = src[3];
			case 3:
				CANOutput.LOW.WORD.WORD_2 = src[2];
			case 2:
				CANOutput.HIGH.WORD.WORD_1 = src[1];
			case 1:
				CANOutput.HIGH.WORD.WORD_0 = src[0];
				BCCI_SendResponseFrameEx(Interface, Slave_MBOX_RB_16_A, &CANOutput, length);
				break;
			default:
				BCCI_SendResponseFrameEx(Interface, Slave_MBOX_RB_16_A, &CANOutput, 0);
				break;
		}
	}
	else
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ENDPOINT, epnt);
}
// ----------------------------------------

void BCCI_HandleReadBlockFloat(pBCCI_Interface Interface)
{
	CANMessage CANInput;
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_RB_F, &CANInput);

	Int16U epnt = CANInput.HIGH.WORD.WORD_0;
	if((epnt < xCCI_MAX_READ_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.ReadEndpointsFloat[epnt])
	{
		pInt32U src;
		CANMessage CANOutput = CANInput;

		Int16U length = Interface->ProtectionAndEndpoints.ReadEndpointsFloat[epnt](epnt, (float**)&src,
				Interface->ArgForEPCallback, 2);

		switch(length)
		{
			case 2:
				CANOutput.LOW.DWORD_1 = src[1];
			case 1:
				CANOutput.HIGH.DWORD_0 = src[0];
				BCCI_SendResponseFrameEx(Interface, Slave_MBOX_RB_F_A, &CANOutput, length);
				break;
			default:
				BCCI_SendResponseFrameEx(Interface, Slave_MBOX_RB_F_A, &CANOutput, 0);
				break;
		}
	}
	else
		BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ENDPOINT, epnt);
}
// ----------------------------------------

void BCCI_HandleWriteBlock16(pBCCI_Interface Interface)
{
	CANMessage CANInput;
	Int16U epnt, length;
	Int16U Data[3];
	
	Interface->IOConfig->IO_GetMessage(Slave_MBOX_WB_16, &CANInput);
	
	epnt = CANInput.HIGH.WORD.WORD_0 >> 8;
	length = CANInput.HIGH.WORD.WORD_0 & 0xFF;
	Data[0] = CANInput.HIGH.WORD.WORD_1;
	Data[1] = CANInput.LOW.WORD.WORD_2;
	Data[2] = CANInput.LOW.WORD.WORD_3;
	
	if(length <= BCCI_BLOCK_MAX_VAL_16_W)
	{
		if((epnt < xCCI_MAX_WRITE_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.WriteEndpoints16[epnt])
		{
			CANMessage CANOutput = CANInput;
			CANOutput.HIGH.WORD.WORD_0 = epnt << 8;
			
			Interface->ProtectionAndEndpoints.WriteEndpoints16[epnt](epnt, &Data[0], FALSE, length,
					Interface->ArgForEPCallback);
			
			BCCI_SendResponseFrameEx(Interface, Slave_MBOX_WB_16_A, &CANOutput, length);
		}
		else
			BCCI_SendErrorFrame(Interface, CANInput, ERR_INVALID_ENDPOINT, epnt);
	}
	else
		BCCI_SendErrorFrame(Interface, CANInput, ERR_ILLEGAL_SIZE, length);
}
// ----------------------------------------

void BCCI_SendResponseFrameEx(pBCCI_Interface Interface, Int16U Mailbox, pCANMessage Message, Int16U MessageLength)
{
	Message->DLC = MessageLength * 2;
	Interface->IOConfig->IO_SendMessageEx(Mailbox, Message, FALSE, TRUE);
}
// ----------------------------------------

void BCCI_SendResponseFrame(pBCCI_Interface Interface, Int16U Mailbox, pCANMessage Message)
{
	Interface->IOConfig->IO_SendMessage(Mailbox, Message);
}
// ----------------------------------------

void BCCI_SendErrorFrame(pBCCI_Interface Interface, CANMessage CANInput, Int16U ErrorCode, Int16U Details)
{
	CANMessage CANOutput = CANInput;
	
	CANOutput.HIGH.WORD.WORD_0 = ErrorCode;
	CANOutput.HIGH.WORD.WORD_1 = Details;
	
	Interface->IOConfig->IO_SendMessage(Slave_MBOX_ERR_A, &CANOutput);
}
// ----------------------------------------

Int16U BCCI_AddProtectedArea(pBCCI_Interface Interface, Int16U StartAddress, Int16U EndAddress)
{
	return xCCI_AddProtectedArea(&(Interface->ProtectionAndEndpoints), StartAddress, EndAddress);
}
// ----------------------------------------

Boolean BCCI_RemoveProtectedArea(pBCCI_Interface Interface, Int16U AreaIndex)
{
	return xCCI_RemoveProtectedArea(&(Interface->ProtectionAndEndpoints), AreaIndex);
}
// ----------------------------------------

Boolean BCCI_RegisterReadEndpoint16(pBCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpoint16 ReadCallback)
{
	return xCCI_RegisterReadEndpoint16(&(Interface->ProtectionAndEndpoints), Endpoint, ReadCallback);
}
// ----------------------------------------

Boolean BCCI_RegisterReadEndpointFloat(pBCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpointFloat ReadCallback)
{
	return xCCI_RegisterReadEndpointFloat(&(Interface->ProtectionAndEndpoints), Endpoint, ReadCallback);
}
// ----------------------------------------

Boolean BCCI_RegisterWriteEndpoint16(pBCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackWriteEndpoint16 WriteCallback)
{
	return xCCI_RegisterWriteEndpoint16(&(Interface->ProtectionAndEndpoints), Endpoint, WriteCallback);
}
// ----------------------------------------
