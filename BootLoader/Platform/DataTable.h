// -----------------------------------------
// Device data table
// ----------------------------------------

#ifndef __DATA_TABLE_H
#define __DATA_TABLE_H

// Include
#include "stdinc.h"


// Constants
//
#define DATA_TABLE_SIZE		24
#define DATA_TABLE_NV_SIZE	2
//
#define DATA_TABLE_NV_START 0
#define DATA_TABLE_WR_START 2
#define DATA_TABLE_WP_START 10

/*
 * DATA TABLE START 				------------- 0
 * 			[NON-VOLATILE AREA]
 *		END OF NON-VOLATILE AREA	------------- 1
 * 		START OF READ/WRITE AREA	------------- 2
 * 			[VOLATILE AREA]
 *		END OF READ/WRITE AREA		------------- 9
 * 		START OF READ-ONLY AREA		------------- 10
 * 			[VOLATILE R-O AREA]
 *		END OF READ-ONLY AREA		------------- 23
 * DATA TABLE END 					------------- [24]
 */

// Types
//
typedef void (*FUNC_SetDefaultValues)();
typedef void (*FUNC_EPROM_WriteValues)(Int32U EPROMAddress, pInt16U Buffer, Int16U BufferSize);
typedef void (*FUNC_EPROM_ReadValues)(Int32U EPROMAddress, pInt16U Buffer, Int16U BufferSize);
//
typedef struct __EPROMServiceConfig
{
	FUNC_EPROM_WriteValues WriteService;
	FUNC_EPROM_ReadValues ReadService;
} EPROMServiceConfig, *pERPOMServiceConfig;


// Variables
//
extern volatile Int16U DataTable[DATA_TABLE_SIZE];


// Functions
//
// Initialize data table
void DT_Init(EPROMServiceConfig EPROMService, Boolean NoRestore);
// Restore part of data table from non-volatile EPROM memory
void DT_RestoreNVPartFromEPROM();
// Save part of data table to non-volatile EPROM memory
void DT_SaveNVPartToEPROM();
// Reset non-volatile part of data table
void DT_ResetNVPart(FUNC_SetDefaultValues SetFunc);
// Reset volatile read-write part of data table
void DT_ResetWRPart(FUNC_SetDefaultValues SetFunc);


#endif // __DATA_TABLE_H
