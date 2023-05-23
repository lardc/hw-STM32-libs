// Header
#include "FirmwareLabel.h"

// Include
#include "LabelDescription.h"
#include "ZwIWDG.h"
#include "ZwNFLASH.h"

// Definitions
#define LABEL_WRITE_AREA		((LABEL_NAME_MAX_LENGTH >> 1) + 2)

// Variables
static SelectorIndex LabelSelector = SID_None;

// Forward functions
Boolean FWLB_AreNamesEqual(char *a, char *b);

// Functions
//
void FWLB_LoadBoardLabel()
{
	// Чтение сохранённой метки
	Int16U i;
	char CurrentLabel[LABEL_NAME_MAX_LENGTH] = {0};
	for(i = 0; i < (LABEL_NAME_MAX_LENGTH >> 1); i++)
	{
		Int16U Symbol = *(pInt16U)(i + LABEL_START_ADDRESS + 2);

		CurrentLabel[i * 2] = ((Symbol >> 8) == 0xFF) ? 0 : Symbol >> 8;
		CurrentLabel[i * 2 + 1] = ((Symbol & 0xFF) == 0xFF) ? 0 : Symbol & 0xFF;
	}

	// Определение индекса метки
	for(i = 0; i < BoardLabelsSize; i++)
	{
		if(FWLB_AreNamesEqual(CurrentLabel, (char *)BoardLabels[i].Name))
		{
			LabelSelector = BoardLabels[i].Index;
			break;
		}
	}
}
// ----------------------------------------

void FWLB_WriteBoardLabel(Int16U Index)
{
	// Проверка на превышение индекса
	if(Index >= BoardLabelsSize)
		return;

	// Проверка на нестёртую память
	Int16U i;
	for(i = 0; i < LABEL_WRITE_AREA; i++)
	{
		if(*(pInt16U)(i + LABEL_START_ADDRESS) != 0xFFFF)
			return;
	}

	Int16U tmp[LABEL_WRITE_AREA] = {0};
	for(i = 0; i < LABEL_WRITE_AREA - 2; i++)
	{
		tmp[i + 2] = (Int16U)BoardLabels[Index].Name[i * 2] << 8;
		tmp[i + 2] |= BoardLabels[Index].Name[i * 2 + 1];
	}

	tmp[0] = 0;							// Строковые данные
	tmp[1] = LABEL_NAME_MAX_LENGTH;		// Длина строки

	NFLASH_WriteArray16(LABEL_START_ADDRESS, tmp, LABEL_WRITE_AREA);
}
// ----------------------------------------

Boolean FWLB_AreNamesEqual(char *a, char *b)
{
	Int16U i;
	for(i = 0; i < LABEL_NAME_MAX_LENGTH; i++)
	{
		if(a[i] != b[i])
			return FALSE;
	}

	return TRUE;
}
// ----------------------------------------

SelectorIndex FWLB_GetSelector()
{
	return LabelSelector;
}
// ----------------------------------------
