#ifndef CONTROLLER_TRM10_H_
#define CONTROLLER_TRM10_H_

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
// Address = 0 в Modbus является broadcast-адресом на линии
float TRM10_ReadRegFloat(Int8U Address, Int16U RegAddress, pTRMError error);
Boolean TRM10_WriteRegFloat(Int8U Address, Int16U RegAddress, float Value, pTRMError error);
Int16U TRM10_ReadUint16(Int8U Address, Int16U RegAddress, pTRMError error);
Boolean TRM10_WriteUint16(Int8U Address, Int16U RegAddress, Int16U Value, pTRMError error);
float TRM10_ReadTemp(Int8U Address, pTRMError error);
float TRM10_ReadPower(Int8U Address, pTRMError error);
void TRM10_SetTemp(Int8U Address, float Temperature, pTRMError error);
void TRM10_Start(Int8U Address, pTRMError error);
void TRM10_Stop(Int8U Address, pTRMError error);

#endif /* CONTROLLER_TRM10_H_ */
