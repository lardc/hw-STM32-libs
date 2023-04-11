﻿// Header
//
#include "BCCIMHighLevel.h"

// Variables
//
static BHLError BHL_Error = {ERR_NO_ERROR, 0, 0, 0, 0};
static pBCCIM_Interface MASTER_CAN_Interface;


// Forward functions
//
void BHL_LoadError(Int16U Code, Int16U NodeID, Int16U Func, Int16U ExtData);


// Functions
//
void BHL_Init(pBCCIM_Interface MasterInterface)
{
	MASTER_CAN_Interface = MasterInterface;
}
// ----------------------------------------

bool BHL_Call(uint16_t NodeID, uint16_t Command)
{
	Int16U err;
	err = BCCIM_Call(MASTER_CAN_Interface, NodeID, Command);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		BHL_LoadError(err, NodeID, FUNCTION_CALL, Command);
		return FALSE;
	}
}
// ----------------------------------------

bool BHL_ReadRegister(uint16_t NodeID, uint16_t Address, uint16_t* Data)
{
	Int16U err;
	err = BCCIM_Read16(MASTER_CAN_Interface, NodeID, Address, Data);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		BHL_LoadError(err, NodeID, FUNCTION_READ, Address);
		return FALSE;
	}
}
// ----------------------------------------

bool BHL_ReadRegisterFloat(uint16_t NodeID, uint16_t Address, float* Data)
{
	Int16U err;
	err = BCCIM_ReadFloat(MASTER_CAN_Interface, NodeID, Address, Data);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		BHL_LoadError(err, NodeID, FUNCTION_READ, Address);
		return FALSE;
	}
}
// ----------------------------------------

bool BHL_WriteRegister(uint16_t NodeID, uint16_t Address, uint16_t Data)
{
	Int16U err;
	err = BCCIM_Write16(MASTER_CAN_Interface, NodeID, Address, Data);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		BHL_LoadError(err, NodeID, FUNCTION_WRITE, Address);
		return FALSE;
	}
}
// ----------------------------------------

bool BHL_WriteRegisterFloat(uint16_t NodeID, uint16_t Address, float Data)
{
	Int16U err;
	err = BCCIM_WriteFloat(MASTER_CAN_Interface, NodeID, Address, Data);

	if (err == ERR_NO_ERROR)
		return TRUE;
	else
	{
		BHL_LoadError(err, NodeID, FUNCTION_WRITE, Address);
		return FALSE;
	}
}
// ----------------------------------------

bool BHL_ReadArray16(uint16_t Node, uint16_t Endpoint, uint16_t* Data, uint16_t DataSize, uint16_t* DataRead)
{
	uint16_t err;
	err = BCCIM_ReadBlock16(MASTER_CAN_Interface, Node, Endpoint);

	if (err == ERR_NO_ERROR)
	{
		BCCIM_ReadBlock16Load(Data, DataSize, DataRead);
		return TRUE;
	}
	else
	{
		*DataRead = 0;
		BHL_LoadError(err, Node, FUNCTION_CALL, Endpoint);
		return FALSE;
	}
}
// ----------------------------------------

BHLError BHL_GetError()
{
	return BHL_Error;
}
// ----------------------------------------

void BHL_LoadError(Int16U Code, Int16U NodeID, Int16U Func, Int16U ExtData)
{
	BHL_Error.ErrorCode = Code;
	BHL_Error.Device = NodeID;
	BHL_Error.Func = Func;
	BHL_Error.ExtData = ExtData;
	BHL_Error.Details = BCCIM_GetSavedErrorDetails();
}
// ----------------------------------------

void BHL_ResetError()
{
	BHL_Error.ErrorCode = ERR_NO_ERROR;
	BHL_Error.Device = 0;
	BHL_Error.Func = 0;
	BHL_Error.ExtData = 0;
	BHL_Error.Details = 0;
}
// ----------------------------------------
