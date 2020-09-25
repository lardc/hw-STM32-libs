// Includes
//
#include "ZwRCC.h"
#include "ZwIWDG.h"

void IWDG_Config()
{
	// Configure independent clock
	RCC_LSI_Config();

	// Disable IWDG if core is halted
	DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;

	// Configure timing
	IWDG->KR = 0x00005555;
	IWDG->PR |= IWDG_PR_PR_1;
	IWDG->RLR = 0xFFF;

	// Start watchdog
	IWDG->KR = 0x0000CCCC;
	while (IWDG->SR);
}
// -----------------------------------------------

void IWDG_Refresh()
{
	if (!(IWDG->SR)) IWDG->KR = 0x0000AAAA;
}
// -----------------------------------------------

void IWDG_ConfigureFastUpdate()
{
	while (IWDG->SR);

	// Re-configure timing to ~200ms refresh rate
	IWDG->KR = 0x00005555;
	IWDG->RLR = 0x1FF;

	while (IWDG->SR);
	IWDG_Refresh();
}
// -----------------------------------------------

void IWDG_ConfigureSlowUpdate()
{
	while (IWDG->SR);

	// Re-configure timing to ~1600ms refresh rate
	IWDG->KR = 0x00005555;
	IWDG->RLR = 0xFFF;

	while (IWDG->SR);
	IWDG_Refresh();
}
// -----------------------------------------------

