#ifndef __FORMAT_OUTPUT_JSON_H
#define __FORMAT_OUTPUT_JSON_H

#include "Global.h"
#include "JSONDescription.h"
#include "stdinc.h"

#define PARAMETER_SIZE 32

extern const char TemplateJSON[];

void JSON_ResetStateMachine();
Int16U JSON_ReadSymbol();
Int16U itoa(char* dest, Int16U i);

#endif // __FORMAT_OUTPUT_JSON_H
