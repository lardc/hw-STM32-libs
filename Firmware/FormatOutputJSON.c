#include "JSONDescription.h"
#include "FormatOutputJSON.h"
#include "DataTable.h"
#include "stdinc.h"

// Definitions
#define PARAMETER_SIZE			32
#define JSON_POINTERS_SIZE		32

typedef enum __JSONReadStateMachine
{
	JRSM_Read = 0,
	JRSM_Replace
} JSONReadStateMachine;

// Variables
static JSONReadStateMachine CurrentState = JRSM_Read;
static char Parameter[PARAMETER_SIZE];
static Int16U TemplatePosition, ParameterPosition, Index;
static void* JSONPointers[JSON_POINTERS_SIZE] = {0};

// Forward functions
Int16U JSON_GetIntByPointer(void *Pointer);
Int16U itoa(char* dest, Int16U i);

// Functions
//
void JSON_ResetStateMachine()
{
	CurrentState = JRSM_Read;
	TemplatePosition = 0;
	ParameterPosition = 0;
	Index = 0;
}
// ----------------------------------------

Int16U JSON_ReadSymbol()
{
	switch (CurrentState)
	{
		case JRSM_Read:
			if (TemplateJSON[TemplatePosition] == '\0')
				return 0;

			if (TemplateJSON[TemplatePosition] == '$')
			{
				CurrentState = JRSM_Replace;
				TemplatePosition++;

				ParameterPosition = 0;
				itoa(Parameter, JSON_GetIntByPointer(JSONPointers[Index++]));
			}
			else
				return TemplateJSON[TemplatePosition++];

		case JRSM_Replace:
			if (Parameter[ParameterPosition + 1] == '\0')
				CurrentState = JRSM_Read;
			return Parameter[ParameterPosition++];

		default:
			return 0;
	}
}
// ----------------------------------------

Int16U JSON_GetIntByPointer(void *Pointer)
{
#ifdef USE_FLOAT_DT
	return (Int16U)(*((pFloat32)Pointer));
#else
	return *((pInt16U)Pointer);
#endif
}
// ----------------------------------------

Int16U itoa(char *dest, Int16U i)
{
	if (i < 0)
		return 0;

	char *start = dest;
	char buffer[12];
	char *p = buffer;

	do
	{
		*p++ = '0' + (i % 10);
		i /= 10;
	} while (i > 0);

	while (p != buffer)
		*dest++ = *--p;

	*dest = '\0';
	return dest - start;
}
// ----------------------------------------

void JSON_AssignPointer(Int16U Index, void* Pointer)
{
	if (Index < JSON_POINTERS_SIZE)
		JSONPointers[Index] = Pointer;
}
// ----------------------------------------
