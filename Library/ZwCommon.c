// Header
#include "ZwCommon.h"

// Includes
#include "SysConfig.h"
#include "ZwIWDG.h"

// Functions
//
void COM_InterruptPrioritySet(IRQn_Type EXTI_IRQ, uint16_t Priority)
{
	NVIC_SetPriority(EXTI_IRQ, Priority);
}
//-----------------------------------------------
