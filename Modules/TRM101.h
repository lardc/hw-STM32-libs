#ifndef __TRM101_H
#define __TRM101_H

// Include
#include "stdinc.h"

// Types
typedef enum __TRMError
{
	TRME_None				= 0,
	TRME_ResponseTimeout	= 1,
	TRME_InputBufferOverrun	= 2,
	TRME_CheckSumError		= 3,
	TRME_WrongResponse		= 4
} TRMError, *pTRMError;

// Functions
float TRM_ReadTemp(Int16U Address, pTRMError error);
float TRM_ReadPower(Int16U Address, pTRMError error);
void TRM_SetTemp(Int16U Address, float Temperature, pTRMError error);
void TRM_Start(Int16U Address, pTRMError error);
void TRM_Stop(Int16U Address, pTRMError error);

#endif // __TRM101_H
