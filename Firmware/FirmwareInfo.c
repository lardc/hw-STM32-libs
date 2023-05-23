// Header
#include "FirmwareInfo.h"

// Definitions
#define COMMIT_LEN		7
#define DATE_LEN		19

// Includes
#include "git_info.h"

// Forward functions
void FWINF_AppendSymbol(pInt16U DataTable, Int16U Symbol, Int16U Counter);

// Functions
//
Int16U FWINF_Compose(pInt16U DataTable, Int16U MaxLength)
{
	Int16U BranchLen, i, counter = 0;
	
	// Compose commit info
	for(i = 0; i < COMMIT_LEN && counter < MaxLength; i++)
		FWINF_AppendSymbol(DataTable, git_commit[i], counter++);
	if(counter < MaxLength)
		FWINF_AppendSymbol(DataTable, ',', counter++);
	
	// Compose date info
	for(i = 0; i < DATE_LEN && counter < MaxLength; i++)
		FWINF_AppendSymbol(DataTable, git_date[i], counter++);
	if(counter < MaxLength)
		FWINF_AppendSymbol(DataTable, ',', counter++);
	
	// Compose date info
	BranchLen = sizeof(git_branch) / sizeof(char) - 1;
	for(i = 0; i < BranchLen && counter < MaxLength; i++)
		FWINF_AppendSymbol(DataTable, git_branch[i], counter++);
	
	// Align counter to even
	if(counter % 2 && counter < MaxLength)
		FWINF_AppendSymbol(DataTable, ' ', counter++);

	return counter;
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
