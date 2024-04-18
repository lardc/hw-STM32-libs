// ----------------------------------------
// BCCI-Master communication interface
// ----------------------------------------

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
typedef struct __BCCIM_Interface
{
	pBCCI_IOConfig IOConfig;
	Int32U TimeoutValueTicks;
	volatile Int64U *pTimerCounter;
} BCCIM_Interface, *pBCCIM_Interface;

// Functions
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer);
void BCCIM_InitWithNodeID(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks,
		volatile Int64U *pTimer, Int16U NodeID);
Int16U BCCIM_Read16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, pInt16U Data);
Int16U BCCIM_ReadFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float* Data);
Int16U BCCIM_ReadLimitFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Boolean ReadHighLimit, float* Data);
Int16U BCCIM_Write16(pBCCIM_Interface Interface, Int16U Node, Int16U Register, Int16U Data);
Int16U BCCIM_WriteFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Register, float Data);
Int16U BCCIM_Call(pBCCIM_Interface Interface, Int16U Node, Int16U Action);
Int16U BCCIM_ReadBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint);
Int16U BCCIM_WriteBlock16(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint, pInt16U Data, Int16U DataLength);
Int16U BCCIM_ReadBlockFloat(pBCCIM_Interface Interface, Int16U Node, Int16U Endpoint);
void BCCIM_ReadBlock16Load(pInt16U DataArray, Int16U DataSize, pInt16U DataRead);
Int16U BCCIM_GetSavedErrorDetails();

#endif // __BCCI_MASTER_H
