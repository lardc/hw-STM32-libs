// Header
#include "FirmwareInfo.h"

// Includes
#include "git_info.h"
#include <string.h>

// Forward functions
void FWINF_AppendSymbol(pInt16U DataTable, Int16U Symbol, Int16U Counter);
void FWINF_AppendString(pInt16U DataTable, const char *String, Int16U StringLen, pInt16U pCounter, Int16U MaxLength, bool IsLastString);

// Functions
//
Int16U FWINF_Compose(pInt16U DataTable, Int16U MaxLength)
{
	Int16U counter = 0;

	FWINF_AppendString(DataTable, git_commit,	strlen(git_commit),	&counter, MaxLength, false);
	FWINF_AppendString(DataTable, git_date,		strlen(git_date),	&counter, MaxLength, false);
#ifdef GIT_INF_USE_PROJ
	FWINF_AppendString(DataTable, git_branch,	strlen(git_branch),	&counter, MaxLength, false);
	FWINF_AppendString(DataTable, git_proj,		strlen(git_proj),	&counter, MaxLength, true);
#else
	FWINF_AppendString(DataTable, git_branch,	strlen(git_branch),	&counter, MaxLength, true);
#endif

	return counter;
}
// ----------------------------------------

void FWINF_AppendString(pInt16U DataTable, const char *String, Int16U StringLen, pInt16U pCounter, Int16U MaxLength, bool IsLastString)
{
	Int16U i;

	// Добавление строки
	for(i = 0; i < StringLen && *pCounter < MaxLength; i++)
		FWINF_AppendSymbol(DataTable, String[i], (*pCounter)++);
	if(*pCounter < MaxLength)
	{
		// Выравнивание размера данных
		if(IsLastString)
		{
			if(*pCounter % 2)
				FWINF_AppendSymbol(DataTable, ' ', (*pCounter)++);
		}
		// или добавление разделителя-запятой
		else
			FWINF_AppendSymbol(DataTable, ',', (*pCounter)++);
	}
}
// ----------------------------------------

void FWINF_AppendSymbol(pInt16U DataTable, Int16U Symbol, Int16U Counter)
{
	if(Counter % 2)
		*(DataTable + Counter / 2) |= Symbol & 0xFF;
	else
		*(DataTable + Counter / 2) = Symbol << 8;
}
// ----------------------------------------
