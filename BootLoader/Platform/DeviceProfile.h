// -----------------------------------------
// Device profile
// ----------------------------------------

#ifndef __DEV_PROFILE_H
#define __DEV_PROFILE_H

// Include
#include "stdinc.h"
//
#include "SCCISlave.h"
#include "BCCISlave.h"


// Functions
//
// Initialize device profile engine
void DEVPROFILE_Init(xCCI_FUNC_CallbackAction SpecializedDispatch, volatile Boolean *MaskChanges);
// Initialize endpoint service
void DEVPROFILE_InitEPReadService(pInt16U Indexes, pInt16U Sizes, pInt16U *Counters, pInt16U *Datas);
void DEVPROFILE_InitEPWriteService(pInt16U Indexes, pInt16U Sizes, pInt16U *Counters, pInt16U *Datas);
// Process user interface requests
void DEVPROFILE_ProcessRequests();
// Reset EP counters
void DEVPROFILE_ResetEPReadState();
// Reset user control (WR) section of data table
void DEVPROFILE_ResetControlSection();
// Reset scopes
void DEVPROFILE_ResetReadScopes(Int16U ResetPosition, Int16U ScopeMask);
void DEVPROFILE_ResetWriteScopes(Int16U ResetPosition, Int16U ScopeMask);

// Read 32-bit value from data table
Int32U DEVPROFILE_ReadValue32(pInt16U pTable, Int16U Index);
// Write 32-bit value to data table
void DEVPROFILE_WriteValue32(pInt16U pTable, Int16U Index, Int32U Data);

#endif // __DEV_PROFILE_H
