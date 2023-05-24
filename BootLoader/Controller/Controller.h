// ----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
#include "stdinc.h"
//
#include "Global.h"
#include "DeviceObjectDictionary.h"


// Variables
//
extern volatile Int32U *ProgramAddressStart;
extern volatile Boolean WaitForFWUpload;
extern volatile Int64U CONTROL_TimeCounter;


// Functions
//
// Initialize controller
void CONTROL_Init();
// Do background idle operation
void CONTROL_Idle();


#endif // __CONTROLLER_H
