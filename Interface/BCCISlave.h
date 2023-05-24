// -----------------------------------------
// BCCI-Slave communication interface
// ----------------------------------------

#ifndef __BCCIS_H
#define __BCCIS_H

// Include
#include "xCCICommon.h"
#include "BCCITypes.h"
#include "DataTable.h"


// Functions
//
// Init interface instance
void BCCI_Init(pBCCI_Interface Interface, pBCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig,
 			   pInt16U DataTable, Int16U DataTableSize, void *ArgumentForCallback);
void BCCI_InitWithNodeID(pBCCI_Interface Interface, pBCCI_IOConfig IOConfig, pxCCI_ServiceConfig ServiceConfig,
		pInt16U DataTable, Int16U DataTableSize, void *ArgumentForCallback, Int16U NodeID);
// Process packets
void BCCI_Process(pBCCI_Interface Interface, Boolean MaskStateChangeOperations);


// Internal functions
//
// Create protected area
Int16U  BCCI_AddProtectedArea(pBCCI_Interface Interface, Int16U StartAddress, Int16U EndAddress);
//
// Remove protected area
Boolean  BCCI_RemoveProtectedArea(pBCCI_Interface Interface, Int16U AreaIndex);
//
// Register read endpoint 16-bit source callback
Boolean  BCCI_RegisterReadEndpoint16(pBCCI_Interface Interface, Int16U Endpoint,
								    xCCI_FUNC_CallbackReadEndpoint16 ReadCallback);
//
Boolean BCCI_RegisterReadEndpointFloat(pBCCI_Interface Interface, Int16U Endpoint,
		xCCI_FUNC_CallbackReadEndpointFloat ReadCallback);
//
// Register write endpoint 16-bit destination callback
Boolean  BCCI_RegisterWriteEndpoint16(pBCCI_Interface Interface, Int16U Endpoint,
									 xCCI_FUNC_CallbackWriteEndpoint16 WriteCallback);

#endif // __BCCIS_H
