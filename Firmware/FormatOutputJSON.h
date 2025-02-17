#ifndef __FORMAT_OUTPUT_JSON_H
#define __FORMAT_OUTPUT_JSON_H

#include "stdinc.h"

extern const char TemplateJSON[];

void JSON_ResetStateMachine();
Int16U JSON_ReadSymbol();
void JSON_AssignPointer(Int16U Index, void* Pointer);

#endif // __FORMAT_OUTPUT_JSON_H
