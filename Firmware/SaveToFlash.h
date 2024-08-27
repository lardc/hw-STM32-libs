#ifndef __SAVE_TO_FLASH_H
#define __SAVE_TO_FLASH_H

// Include
#include "stdinc.h"
#include "SaveToFlashConfig.h"
#include "ZwNFLASH.h"

// Definition
#define MAX_DESCRIPTION_LEN			64

// Types
typedef enum __DataType
{
	DT_Char = 0,
	DT_Int8U,
	DT_Int8S,
	DT_Int16U,
	DT_Int16S,
	DT_Int32U,
	DT_Int32S,
	DT_Float
} DataType;

typedef struct __RecordDescription
{
	char Description[MAX_DESCRIPTION_LEN];
	DataType Type;
	Int16U Length;
} RecordDescription;

// Functions
void STF_AssignPointer(Int16U Index, Int32U Pointer);
void STF_SaveDiagData();
void STF_EraseDataSector();

#endif // __SAVE_TO_FLASH_H
