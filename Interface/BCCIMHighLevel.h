#ifndef __BCCIM_HIGH_LEVEL_H
#define __BCCIM_HIGH_LEVEL_H

// Includes
//
#include "stdinc.h"
#include "BCCIMaster.h"

// Types
//
typedef struct __BHLError
{
	uint16_t ErrorCode;
	uint16_t Device;
	uint16_t Func;
	uint16_t ExtData;
} BHLError, *pBHLError;

// Functions
//
void BHL_Init(pBCCIM_Interface MasterInterface);
bool BHL_Call(uint16_t NodeID, uint16_t Command);
bool BHL_ReadRegister(uint16_t NodeID, uint16_t Address, uint16_t* Data);
bool BHL_WriteRegister(uint16_t NodeID, uint16_t Address, uint16_t Data);
bool HLI_CAN_ReadArray16(uint16_t Node, uint16_t Endpoint, uint16_t* Data, uint16_t DataSize, uint16_t* DataRead);
//
BHLError BHL_GetError();
void BHL_ResetError();

#endif // __BCCIM_HIGH_LEVEL_H
