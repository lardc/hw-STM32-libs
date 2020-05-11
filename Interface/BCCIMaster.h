// -----------------------------------------
// BCCI master communication interface
// ----------------------------------------

#ifndef __BCCI_MASTER_H
#define __BCCI_MASTER_H


// Include
#include "stdinc.h"
//
#include "BCCITypes.h"


// Types
//
// BCCI instance state
typedef struct __BCCIM_Interface
{
	pBCCI_IOConfig IOConfig;
	Int32U TimeoutValueTicks;
	volatile Int64U *pTimerCounter;
} BCCIM_Interface, *pBCCIM_Interface;


// Functions
//
void BCCIM_Init(pBCCIM_Interface Interface, pBCCI_IOConfig IOConfig, Int32U MessageTimeoutTicks, volatile Int64U *pTimer);
Int16U BCCIM_Read16(pBCCIM_Interface Interface, Int16U Node, Int16U Address, pInt16U Data);
Int16U BCCIM_Write16(pBCCIM_Interface Interface, Int16U Node, Int16U Address, Int16U Data);
Int16U BCCIM_Call(pBCCIM_Interface Interface, Int16U Node, Int16U Action);


#endif // __BCCI_MASTER_H
