// ----------------------------------------
// SCCI-Slave communication interface
// ---------------------------------------- 

#ifndef __SCCIS_H
#define __SCCIS_H

// Include
#include "stdinc.h"
//
#include "SCCIParams.h"
#include "xCCICommon.h"
#include "SCCITypes.h"
#include "DataTable.h"

// Functions
//
void SCCI_Init(pSCCI_Interface Interface, pSCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig, pInt16U DataTable,
		Int16U DataTableSize, Int32U MessageTimeoutTicks, void *ArgumentForCallback);
void SCCI_Process(pSCCI_Interface Interface, Int64U CurrentTickCount, Boolean MaskStateChangeOperations);

Int16U SCCI_AddProtectedArea(pSCCI_Interface Interface, Int16U StartAddress, Int16U EndAddress);
Boolean SCCI_RemoveProtectedArea(pSCCI_Interface Interface, Int16U AreaIndex);

Boolean SCCI_RegisterReadEndpoint16(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpoint16 ReadCallback);
Boolean SCCI_RegisterReadEndpointFloat(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpointFloat ReadCallback);
Boolean SCCI_RegisterWriteEndpoint16(pSCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackWriteEndpoint16 WriteCallback);

void MemZero16(uint16_t *Data, uint16_t Number);
void MemCopy16(uint16_t *Source, uint16_t *Destination, uint16_t Length);

#endif // __SCCIS_H
