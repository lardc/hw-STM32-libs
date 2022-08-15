﻿// ----------------------------------------
// SCCI-Slave communication interface
// ----------------------------------------

// Header
#include "SCCISlave.h"
//
// Includes
#include "CRC16.h"
#include "SysConfig.h"


// Constants
//
enum DispID
{
	DISP_NONE			=	0,
	DISP_R_16			=	1,
	DISP_R_32			=	2,
	DISP_R_16_2			=	3,
	DISP_W_16			=	4,
	DISP_W_32			=	5,
	DISP_W_16_2			=	6,
	DISP_RB_16			=	7,
	DISP_RRB_16			=	8,
	DISP_WB_16			=	9,
	DISP_C				=	10,
	DISP_RBF_16			=	11,
	DISP_RRBF_16		=	12,
	DISP_RB_32			=	13,
	DISP_RRB_32			=	14,
	DISP_R_F			=	15,
	DISP_W_F			=	16,
	DISP_RBF_F			=	17,
	DISP_RLIM_F			=	18
};


// Forward functions
//
void SCCI_DispatchHeader(pSCCI_Interface Interface);
void SCCI_DispatchBody(pSCCI_Interface Interface, Boolean MaskStateChangeOperations);
void SCCI_SendErrorFrame(pSCCI_Interface Interface, Int16U ErrorCode, Int16U Details);
void SCCI_SendErrorFrameEx(pSCCI_Interface Interface, Int16U Node, Int16U ErrorCode, Int16U Details);
void SCCI_SendResponseFrame(pSCCI_Interface Interface, Int16U FrameSize);
void SCCI_SendResponseFrameEx(pSCCI_Interface Interface, Int16U Node, Int16U Function, Int16U SubFunction,
		Int16U FrameSize);
//
void SCCI_HandleRead16(pSCCI_Interface Interface);
void SCCI_HandleReadFloat(pSCCI_Interface Interface);
void SCCI_HandleWrite16(pSCCI_Interface Interface);
void SCCI_HandleWriteFloat(pSCCI_Interface Interface);
void SCCI_HandleCall(pSCCI_Interface Interface);
void SCCI_HandleReadBlock16(pSCCI_Interface Interface, Boolean Repeat);
void SCCI_HandleWriteBlock16(pSCCI_Interface Interface);
void SCCI_HandleReadBlockFast16(pSCCI_Interface Interface, Boolean Repeat);
void SCCI_HandleReadBlockFastFloat(pSCCI_Interface Interface);
void SCCI_HandleReadLimitFloat(pSCCI_Interface Interface);
//
void SCCI_AnswerRead16(pSCCI_Interface Interface, Int16U Node, Int16U Address, Int16U Value);
void SCCI_AnswerWrite16(pSCCI_Interface Interface, Int16U Node, Int16U Address);
void SCCI_AnswerCall(pSCCI_Interface Interface, Int16U Node, Int16U Action);
void SCCI_AnswerError(pSCCI_Interface Interface, Int16U Node, Int16U ErrorCode, Int16U Details);


// Functions
//
void SCCI_Init(pSCCI_Interface Interface, pSCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig,
				pInt16U DataTable, Int16U DataTableSize, Int32U MessageTimeoutTicks,
				void *ArgumentForCallback)
{
	Int16U i;
	
	// Reset fields
	Interface->State = SCCI_STATE_WAIT_HEADER;
	Interface->DispID = DISP_NONE;
	Interface->ExpectedBodyLength = 0;
	Interface->LastTimestampTicks = 0;
	Interface->ProtectionAndEndpoints.ProtectedAreasUsed = 0;
	
	for(i = 0; i < xCCI_BUFFER_SIZE; ++i)
		Interface->MessageBuffer[i] = 0;

	for(i = 0; i < (xCCI_MAX_READ_ENDPOINTS + 1); ++i)
		Interface->ProtectionAndEndpoints.ReadEndpoints16[i] = NULL;

	for(i = 0; i < (xCCI_MAX_WRITE_ENDPOINTS + 1); ++i)
		Interface->ProtectionAndEndpoints.WriteEndpoints16[i] = NULL;

	for(i = 0; i < (xCCI_MAX_READ_ENDPOINTS + 1); ++i)
		Interface->ProtectionAndEndpoints.ReadEndpointsFloat[i] = NULL;

	// Save parameters
	Interface->IOConfig = IOConfig;
	Interface->ServiceConfig = ServiceConfig;
	Interface->DataTableAddress = DataTable;
	Interface->DataTableSize = DataTableSize;
	Interface->TimeoutValueTicks = MessageTimeoutTicks;
	Interface->ArgForEPCallback = ArgumentForCallback;
}
// ----------------------------------------

Int16U SCCI_AddProtectedArea(pSCCI_Interface Interface, Int16U StartAddress, Int16U EndAddress)
{
	return xCCI_AddProtectedArea(&(Interface->ProtectionAndEndpoints), StartAddress, EndAddress);
}
// ----------------------------------------

Boolean	SCCI_RemoveProtectedArea(pSCCI_Interface Interface, Int16U AreaIndex)
{
	return xCCI_RemoveProtectedArea(&(Interface->ProtectionAndEndpoints), AreaIndex);
}
// ----------------------------------------

Boolean SCCI_RegisterReadEndpoint16(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpoint16 ReadCallback)
{
	return xCCI_RegisterReadEndpoint16(&(Interface->ProtectionAndEndpoints), Endpoint, ReadCallback);
}
// ----------------------------------------

Boolean SCCI_RegisterReadEndpointFloat(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpointFloat ReadCallback)
{
	return xCCI_RegisterReadEndpointFloat(&(Interface->ProtectionAndEndpoints), Endpoint, ReadCallback);
}
// ----------------------------------------

Boolean SCCI_RegisterWriteEndpoint16(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackWriteEndpoint16 WriteCallback)
{
	return xCCI_RegisterWriteEndpoint16(&(Interface->ProtectionAndEndpoints), Endpoint, WriteCallback);
}
// ----------------------------------------

void SCCI_Process(pSCCI_Interface Interface, Int64U CurrentTickCount, Boolean MaskStateChangeOperations)
{
	switch(Interface->State)
	{
		case SCCI_STATE_WAIT_HEADER:
			if(Interface->IOConfig->IO_GetBytesToReceive() >= 2 * 2)
			{
				Int16U startByte = Interface->IOConfig->IO_ReceiveByte();

				if(startByte == START_BYTE)
				{
					Int16U nextByte = Interface->IOConfig->IO_ReceiveByte();
					Interface->MessageBuffer[0] = nextByte | (startByte << 8);

					Interface->IOConfig->IO_ReceiveArray16(Interface->MessageBuffer + 1, 1);
					SCCI_DispatchHeader(Interface);

					if(Interface->State == SCCI_STATE_WAIT_BODY)
						Interface->LastTimestampTicks = CurrentTickCount + Interface->TimeoutValueTicks;
				}
			}
			break;

		case SCCI_STATE_WAIT_BODY:
			if(Interface->IOConfig->IO_GetBytesToReceive() >= Interface->ExpectedBodyLength)
			{
				Interface->IOConfig->IO_ReceiveArray16(Interface->MessageBuffer + 2, Interface->ExpectedBodyLength);
				SCCI_DispatchBody(Interface, MaskStateChangeOperations);
			}
			else if(Interface->TimeoutValueTicks && (CurrentTickCount > Interface->LastTimestampTicks))
				SCCI_SendErrorFrame(Interface, ERR_TIMEOUT, CurrentTickCount - Interface->LastTimestampTicks);
			break;
	}
}
// ----------------------------------------

void SCCI_DispatchHeader(pSCCI_Interface Interface)
{
	Int16U fnc = Interface->MessageBuffer[1] >> 8;
	Int16U sfunc = fnc & FUNCTION_SCODE_MASK;

	if((Interface->MessageBuffer[0] & 0xFF) == DEVICE_SCCI_ADDRESS)
	{
		switch((fnc & FUNCTION_CODE_MASK) >> 3)
		{
			case FUNCTION_WRITE:
				switch(sfunc)
				{
					case SFUNC_16:
						Interface->ExpectedBodyLength = 3;
						Interface->State = SCCI_STATE_WAIT_BODY;
						Interface->DispID = DISP_W_16;	
						break;
#ifdef USE_FLOAT_DT
					case SFUNC_FLOAT:
						Interface->ExpectedBodyLength = 4;
						Interface->State = SCCI_STATE_WAIT_BODY;
						Interface->DispID = DISP_W_F;
						break;
#endif
					default:
						SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, sfunc);
						break;
				}
				break;
			case FUNCTION_READ:
				switch(sfunc)
				{
					case SFUNC_16:
						Interface->ExpectedBodyLength = 2;
						Interface->State = SCCI_STATE_WAIT_BODY;	
						Interface->DispID = DISP_R_16;	
						break;
#ifdef USE_FLOAT_DT
					case SFUNC_FLOAT:
						Interface->ExpectedBodyLength = 2;
						Interface->State = SCCI_STATE_WAIT_BODY;
						Interface->DispID = DISP_R_F;
						break;
#endif
					default:
						SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, sfunc);
						break;
				}
				break;
			case FUNCTION_WRITE_BLOCK:
				switch(sfunc)
				{
					case SFUNC_16:
						Interface->ExpectedBodyLength = 5;
						Interface->State = SCCI_STATE_WAIT_BODY;	
						Interface->DispID = DISP_WB_16;	
						break;
					default:
						SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, sfunc);
						break;
				}
				break;
			case FUNCTION_READ_BLOCK:
				switch(sfunc)
				{
					case SFUNC_16:
						Interface->ExpectedBodyLength = 2;
						Interface->State = SCCI_STATE_WAIT_BODY;	
						Interface->DispID = DISP_RB_16;	
						break;
					default:
						SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, sfunc);
						break;
				}
				break;
			case FUNCTION_CALL:
				if(sfunc == 0)
				{
					Interface->ExpectedBodyLength = 2;
					Interface->State = SCCI_STATE_WAIT_BODY;
					Interface->DispID = DISP_C;	
				}
				else
					SCCI_SendErrorFrame(Interface, ERR_INVALID_SFUNCTION, sfunc);
				break;
			case FUNCTION_FAST_READ_BLK:
				switch(sfunc)
				{
					case SFUNC_16:
						Interface->ExpectedBodyLength = 2;
						Interface->State = SCCI_STATE_WAIT_BODY;
						Interface->DispID = DISP_RBF_16;
						break;
#ifdef USE_FLOAT_DT
					case SFUNC_FLOAT:
						Interface->ExpectedBodyLength = 2;
						Interface->State = SCCI_STATE_WAIT_BODY;
						Interface->DispID = DISP_RBF_F;
						break;
#endif
					default:
						SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, sfunc);
						break;
				}
				break;
			case FUNCTION_GET_LIMIT:
				switch(sfunc)
				{
#ifdef USE_FLOAT_DT
					case SFUNC_FLOAT:
						Interface->ExpectedBodyLength = 3;
						Interface->State = SCCI_STATE_WAIT_BODY;
						Interface->DispID = DISP_RLIM_F;
						break;
#endif
					default:
						SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, sfunc);
						break;
				}
				break;
			default:
				SCCI_SendErrorFrame(Interface, ERR_INVALID_FUNCTION, fnc & FUNCTION_CODE_MASK);
				break;
		}
	}
}
// ----------------------------------------

void SCCI_DispatchBody(pSCCI_Interface Interface, Boolean MaskStateChangeOperations)
{
	Int16U crc = CRC16_ComputeCRC(Interface->MessageBuffer, Interface->ExpectedBodyLength + 1);
	if(Interface->MessageBuffer[Interface->ExpectedBodyLength + 1] != crc)
	{
		SCCI_SendErrorFrame(Interface, ERR_CRC, crc);
		return;
	}

	if(MaskStateChangeOperations &&
	   (Interface->DispID == DISP_W_16 ||
	    Interface->DispID == DISP_WB_16))
	{
		SCCI_SendErrorFrame(Interface, ERR_BLOCKED, 0);
		return;
	}

	switch(Interface->DispID)
	{
		case DISP_R_16:
			SCCI_HandleRead16(Interface);
			break;
		case DISP_W_16:
			SCCI_HandleWrite16(Interface);
			break;
		case DISP_C:
			SCCI_HandleCall(Interface);
			break;
		case DISP_WB_16:
			SCCI_HandleWriteBlock16(Interface);
			break;
		case DISP_RB_16:
			SCCI_HandleReadBlock16(Interface, FALSE);
			break;
		case DISP_RBF_16:
			SCCI_HandleReadBlockFast16(Interface, FALSE);
			break;
		case DISP_R_F:
			SCCI_HandleReadFloat(Interface);
			break;
		case DISP_W_F:
			SCCI_HandleWriteFloat(Interface);
			break;
		case DISP_RBF_F:
			SCCI_HandleReadBlockFastFloat(Interface);
			break;
		case DISP_RLIM_F:
			SCCI_HandleReadLimitFloat(Interface);
			break;
		default:
			SCCI_SendErrorFrame(Interface, ERR_NOT_SUPPORTED, 0);
			break;
	}
}
// ----------------------------------------

void SCCI_HandleRead16(pSCCI_Interface Interface)
{
	Int16U node = Interface->MessageBuffer[0] & 0xFF;
	Int16U addr = Interface->MessageBuffer[2];

	if(node == DEVICE_SCCI_ADDRESS)
	{
		if(addr >= Interface->DataTableSize)
		{
			SCCI_SendErrorFrame(Interface, ERR_INVALID_ADDESS, addr);
		}
		else
		{
#ifdef USE_FLOAT_DT
			Int32S t_data = (Int32S)(((float *)Interface->DataTableAddress)[addr]);
			Int16U data = (Int16U)((Int16S)t_data);
#else
			Int16U data = Interface->DataTableAddress[addr];
#endif
			Interface->MessageBuffer[3] = data;
			SCCI_SendResponseFrame(Interface, 5);
		}
	}
#ifdef CAN_BRIDGE
	else
	{
		Int16U data;
		Int16U err = BCCIM_Read16(&MASTER_DEVICE_CAN_Interface, node, addr, &data);

		if(err == ERR_NO_ERROR)
			SCCI_AnswerRead16(Interface, node, addr, data);
		else
			SCCI_AnswerError(Interface, node, err, BCCIM_GetSavedErrorDetails());
	}
#endif
}
// ----------------------------------------

void SCCI_HandleReadFloat(pSCCI_Interface Interface)
{
	Int16U addr = Interface->MessageBuffer[2];

	if(addr >= Interface->DataTableSize)
	{
		SCCI_SendErrorFrame(Interface, ERR_INVALID_ADDESS, addr);
	}
	else
	{
		Int32U data = ((pInt32U)Interface->DataTableAddress)[addr];
		Interface->MessageBuffer[3] = data >> 16;
		Interface->MessageBuffer[4] = data & 0x0000FFFF;
		SCCI_SendResponseFrame(Interface, 6);
	}
}
// ----------------------------------------

void SCCI_HandleReadLimitFloat(pSCCI_Interface Interface)
{
	float LowLimit = 0, HighLimit = 0;
	Int16U addr = Interface->MessageBuffer[2];
	Boolean UseHighLimit = Interface->MessageBuffer[3];

	if(addr >= Interface->DataTableSize)
	{
		SCCI_SendErrorFrame(Interface, ERR_INVALID_ADDESS, addr);
	}
	else if(Interface->ServiceConfig->ValidateCallbackFloat
			&& !Interface->ServiceConfig->ValidateCallbackFloat(addr, 0, &LowLimit, &HighLimit))
	{
		SCCI_SendErrorFrame(Interface, ERR_VALIDATION, addr);
	}
	else
	{
		Int32U data = *(pInt32U)(UseHighLimit ? &HighLimit : &LowLimit);
		Interface->MessageBuffer[3] = data >> 16;
		Interface->MessageBuffer[4] = data & 0x0000FFFF;
		SCCI_SendResponseFrame(Interface, 6);
	}
}
// ----------------------------------------

void SCCI_HandleWrite16(pSCCI_Interface Interface)
{
	Int16U node = Interface->MessageBuffer[0] & 0xFF;
	Int16U addr = Interface->MessageBuffer[2];
	Int16U data = Interface->MessageBuffer[3];

	if(node == DEVICE_SCCI_ADDRESS)
	{
		if(addr >= Interface->DataTableSize)
		{
			SCCI_SendErrorFrame(Interface, ERR_INVALID_ADDESS, addr);
		}
		else if(xCCI_InProtectedZone(&Interface->ProtectionAndEndpoints, addr))
		{
			SCCI_SendErrorFrame(Interface, ERR_PROTECTED, addr);
		}
		else if(Interface->ServiceConfig->ValidateCallback16
				&& !Interface->ServiceConfig->ValidateCallback16(addr, data))
		{
			SCCI_SendErrorFrame(Interface, ERR_VALIDATION, addr);
		}
		else
		{
#ifdef USE_FLOAT_DT
			((float *)Interface->DataTableAddress)[addr] = (float)data;
#else
			Interface->DataTableAddress[addr] = data;
#endif
			SCCI_SendResponseFrame(Interface, 4);
		}
	}
#ifdef CAN_BRIDGE
	else
	{
		Int16U err = BCCIM_Write16(&MASTER_DEVICE_CAN_Interface, node, addr, data);

		if(err == ERR_NO_ERROR)
			SCCI_AnswerWrite16(Interface, node, addr);
		else
			SCCI_AnswerError(Interface, node, err, BCCIM_GetSavedErrorDetails());
	}
#endif
}
// ----------------------------------------

void SCCI_HandleWriteFloat(pSCCI_Interface Interface)
{
	Int16U addr = Interface->MessageBuffer[2];
	Int32U t_data = (Int32U)Interface->MessageBuffer[3] << 16 | Interface->MessageBuffer[4];
	float data = *(float *)&t_data;

	if(addr >= Interface->DataTableSize)
	{
		SCCI_SendErrorFrame(Interface, ERR_INVALID_ADDESS, addr);
	}
	else if(xCCI_InProtectedZone(&Interface->ProtectionAndEndpoints, addr))
	{
		SCCI_SendErrorFrame(Interface, ERR_PROTECTED, addr);
	}
	else if(Interface->ServiceConfig->ValidateCallbackFloat
			&& !Interface->ServiceConfig->ValidateCallbackFloat(addr, data, NULL, NULL))
	{
		SCCI_SendErrorFrame(Interface, ERR_VALIDATION, addr);
	}
	else
	{
		((float *)Interface->DataTableAddress)[addr] = data;
		SCCI_SendResponseFrame(Interface, 4);
	}
}
// ----------------------------------------

void SCCI_HandleReadBlock16(pSCCI_Interface Interface, Boolean Repeat)
{
	pInt16U src;
	Int16U length;
	Int16U epnt = Interface->MessageBuffer[2] >> 8;

	if((epnt < xCCI_MAX_READ_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.ReadEndpoints16[epnt])
	{
		length = Interface->ProtectionAndEndpoints.ReadEndpoints16[epnt](epnt, &src, FALSE, Repeat,
														Interface->ArgForEPCallback, SCCI_BLOCK_MAX_VAL_16_R);
		MemZero16(&Interface->MessageBuffer[3], SCCI_BLOCK_MAX_VAL_16_R);

		if(!length || (length > SCCI_BLOCK_MAX_VAL_16_R))
		{
			Interface->MessageBuffer[2] &= 0xFF00;
		}
		else
		{
			Interface->MessageBuffer[2] = (epnt << 8) | length;
			MemCopy16(src, &Interface->MessageBuffer[3], length);
		}

		SCCI_SendResponseFrame(Interface, SCCI_BLOCK_MAX_VAL_16_R + 4);
	}
	else
		SCCI_SendErrorFrame(Interface, ERR_INVALID_ENDPOINT, epnt);
}
// ----------------------------------------

void SCCI_HandleWriteBlock16(pSCCI_Interface Interface)
{
	Int16U epnt = Interface->MessageBuffer[2] >> 8;
	Int16U length = Interface->MessageBuffer[2] & 0xFF;

	if(length <= SCCI_BLOCK_MAX_VAL_16_W)
	{
		if((epnt < xCCI_MAX_WRITE_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.WriteEndpoints16[epnt])
		{
			if(Interface->ProtectionAndEndpoints.WriteEndpoints16[epnt](epnt, &Interface->MessageBuffer[3], FALSE,
																	    length, Interface->ArgForEPCallback))
			{
				Interface->MessageBuffer[2] &= 0xFF00;
				SCCI_SendResponseFrame(Interface, 4);
			}
			else
				SCCI_SendErrorFrame(Interface, ERR_TOO_LONG, epnt);
		}
		else
			SCCI_SendErrorFrame(Interface, ERR_INVALID_ENDPOINT, epnt);
	}
	else
		SCCI_SendErrorFrame(Interface, ERR_ILLEGAL_SIZE, length);
}
// ----------------------------------------

void SCCI_HandleReadBlockFast16(pSCCI_Interface Interface, Boolean Repeat)
{
	pInt16U src;
	Int16U length;
	Int16U epnt = Interface->MessageBuffer[2] >> 8;
	Int16U ZeroBuffer[xCCI_BUFFER_SIZE] = {0};

	if((epnt < xCCI_MAX_READ_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.ReadEndpoints16[epnt])
	{
		length = Interface->ProtectionAndEndpoints.ReadEndpoints16[epnt](epnt, &src, TRUE, Repeat,
																		Interface->ArgForEPCallback, 0);
		Interface->MessageBuffer[2] = (epnt << 8) | (SCCI_USE_CRC_IN_STREAM ? 1 : 0);

		if(!length || (length > xCCI_BLOCK_STM_MAX_VAL))
			length = 0;

		Interface->MessageBuffer[3] = length;

		if(SCCI_USE_CRC_IN_STREAM)
			Interface->MessageBuffer[4] = CRC16_ComputeCRC(src, length);

		SCCI_SendResponseFrame(Interface, 6);

		Interface->IOConfig->IO_SendArray16(src, length);
		Interface->IOConfig->IO_SendArray16(ZeroBuffer, (8 - length % 8) % 8);
	}
	else
		SCCI_SendErrorFrame(Interface, ERR_INVALID_ENDPOINT, epnt);
}
// ----------------------------------------

void SCCI_HandleReadBlockFastFloat(pSCCI_Interface Interface)
{
	float* src;
	Int16U epnt = Interface->MessageBuffer[2] >> 8;
	Int16U ZeroBuffer[xCCI_BUFFER_SIZE] = {0};

	if((epnt < xCCI_MAX_READ_ENDPOINTS + 1) && Interface->ProtectionAndEndpoints.ReadEndpointsFloat[epnt])
	{
		Interface->MessageBuffer[2] = (epnt << 8) | (SCCI_USE_CRC_IN_STREAM ? 1 : 0);

		Int16U length = Interface->ProtectionAndEndpoints.ReadEndpointsFloat[epnt](epnt, &src,
				Interface->ArgForEPCallback, 0);

		// Process float array as short
		length *= 2;
		pInt16U short_src = (pInt16U)src;

		if(length > xCCI_BLOCK_STM_MAX_VAL)
			length = 0;
		Interface->MessageBuffer[3] = length;

		if(SCCI_USE_CRC_IN_STREAM)
			Interface->MessageBuffer[4] = CRC16_ComputeCRC(short_src, length);

		SCCI_SendResponseFrame(Interface, 6);

		Interface->IOConfig->IO_SendArray16(short_src, length);
		Interface->IOConfig->IO_SendArray16(ZeroBuffer, (8 - length % 8) % 8);
	}
	else
		SCCI_SendErrorFrame(Interface, ERR_INVALID_ENDPOINT, epnt);
}
// ----------------------------------------

void SCCI_HandleCall(pSCCI_Interface Interface)
{
	Int16U node = Interface->MessageBuffer[0] & 0xFF;
	Int16U action = Interface->MessageBuffer[2];

	if(node == DEVICE_SCCI_ADDRESS)
	{
		if(Interface->ServiceConfig->UserActionCallback != NULL)
		{
			Int16U userError = 0;

			if(!Interface->ServiceConfig->UserActionCallback(action, &userError))
			{
				SCCI_SendErrorFrame(Interface, ERR_INVALID_ACTION, action);
			}
			else if(userError != 0)
			{
				SCCI_SendErrorFrame(Interface, ERR_USER, userError);
			}
			else
				SCCI_SendResponseFrame(Interface, 4);
		}
		else
			SCCI_SendErrorFrame(Interface, ERR_INVALID_ACTION, action);
	}
#ifdef CAN_BRIDGE
	else
	{
		Int16U err = BCCIM_Call(&MASTER_DEVICE_CAN_Interface, node, action);

		if(err == ERR_NO_ERROR)
			SCCI_AnswerCall(Interface, node, action);
		else
			SCCI_AnswerError(Interface, node, err, BCCIM_GetSavedErrorDetails());
	}
#endif
}
// ----------------------------------------

void SCCI_SendResponseFrame(pSCCI_Interface Interface, Int16U FrameSize)
{
	Interface->State = SCCI_STATE_WAIT_HEADER;

	Interface->MessageBuffer[0] = (START_BYTE << 8) | DEVICE_SCCI_ADDRESS;
	Interface->MessageBuffer[1] |= RESPONSE_MASK << 8;
	Interface->MessageBuffer[FrameSize - 1] = CRC16_ComputeCRC(Interface->MessageBuffer, FrameSize - 1);

	Interface->IOConfig->IO_SendArray16(Interface->MessageBuffer, FrameSize);
}
// ----------------------------------------

void SCCI_SendResponseFrameEx(pSCCI_Interface Interface, Int16U Node, Int16U Function, Int16U SubFunction,
		Int16U FrameSize)
{
	Interface->State = SCCI_STATE_WAIT_HEADER;

	Interface->MessageBuffer[0] = (START_BYTE << 8) | Node;
	Interface->MessageBuffer[1] = (RESPONSE_MASK | (Function << 3) | SubFunction) << 8;
	Interface->MessageBuffer[FrameSize - 1] = CRC16_ComputeCRC(Interface->MessageBuffer, FrameSize - 1);

	Interface->IOConfig->IO_SendArray16(Interface->MessageBuffer, FrameSize);
}
// ----------------------------------------

void SCCI_SendErrorFrame(pSCCI_Interface Interface, Int16U ErrorCode, Int16U Details)
{
	SCCI_SendErrorFrameEx(Interface, DEVICE_SCCI_ADDRESS, ErrorCode, Details);
}
// ----------------------------------------

void SCCI_SendErrorFrameEx(pSCCI_Interface Interface, Int16U Node, Int16U ErrorCode, Int16U Details)
{
	Interface->State = SCCI_STATE_WAIT_HEADER;

	Interface->MessageBuffer[0] = (START_BYTE << 8) | Node;
	Interface->MessageBuffer[1] = (RESPONSE_MASK | (FUNCTION_ERROR << 3)) << 8;
	Interface->MessageBuffer[2] = ErrorCode;
	Interface->MessageBuffer[3] = Details;
	Interface->MessageBuffer[4] = CRC16_ComputeCRC(Interface->MessageBuffer, 4);

	Interface->IOConfig->IO_SendArray16(Interface->MessageBuffer, 5);
}
// ----------------------------------------

void MemZero16(uint16_t *Data, uint16_t Number)
{
	uint16_t i;
	for (i = 0; i < Number; ++i)
		*(Data + i) = 0;
}
//-----------------------------------------

void MemCopy16(uint16_t *Source, uint16_t *Destination, uint16_t Length)
{
	uint16_t i, Temp;

	for (i = 0; i < Length; ++i)
	{
		Temp = *(Source + i);
		*(Destination + i) = Temp;
	}
}
//-----------------------------------------

void SCCI_AnswerRead16(pSCCI_Interface Interface, Int16U Node, Int16U Address, Int16U Value)
{
	Interface->MessageBuffer[2] = Address;
	Interface->MessageBuffer[3] = Value;

	SCCI_SendResponseFrameEx(Interface, Node, FUNCTION_READ, SFUNC_16, 5);
}
//-----------------------------------------

void SCCI_AnswerWrite16(pSCCI_Interface Interface, Int16U Node, Int16U Address)
{
	Interface->MessageBuffer[2] = Address;

	SCCI_SendResponseFrameEx(Interface, Node, FUNCTION_WRITE, SFUNC_16, 4);
}
//-----------------------------------------

void SCCI_AnswerCall(pSCCI_Interface Interface, Int16U Node, Int16U Action)
{
	Interface->MessageBuffer[2] = Action;

	SCCI_SendResponseFrameEx(Interface, Node, FUNCTION_CALL, SFUNC_NONE, 4);
}
//-----------------------------------------

void SCCI_AnswerError(pSCCI_Interface Interface, Int16U Node, Int16U ErrorCode, Int16U Details)
{
	SCCI_SendErrorFrameEx(Interface, Node, ErrorCode, Details);
}
// ----------------------------------------
