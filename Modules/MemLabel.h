#ifndef CONTROLLER_MEMLABEL_H_
#define CONTROLLER_MEMLABEL_H_

// Includes
#include "stdinc.h"
#include "DS2431.h"

// Defines
#define MEM_LABEL_LABEL_SIZE		3
#define MEM_LABEL_MAX_LABELS		(DS2431_EEPROM_SIZE / MEM_LABEL_LABEL_SIZE)
// Types
typedef enum __MemLabelType
{
	ML_Device = 		0,
	ML_Version,
	ML_SerialNumber,
	ML_AdapterCode,
	ML_MaxVoltage,
	ML_MaxCurrent,
	ML_ClampHeight,

	ML_None =			0xFF
} MemLabelType;

typedef struct __MemLabelEntry
{
	Int8U Type;
	Int16U Value;
} MemLabelEntry;

// Functions
Int8U MemLabel_Read(Int8U deviceIndex, MemLabelEntry *Labels, Int8U MaxLabels);
Boolean MemLabel_AddOne(Int8U deviceIndex, MemLabelEntry Label);
Boolean MemLabel_AddArray(Int8U deviceIndex, const MemLabelEntry *Labels, Int8U LabelCount);
Boolean MemLabel_Update(Int8U deviceIndex, MemLabelEntry Label);
Boolean MemLabel_EraseAll(Int8U deviceIndex);

#endif /* CONTROLLER_MEMLABEL_H_ */







