// Includes
//
#include "ZwRCC.h"
#include "ZwWWDG.h"

void WWDG_Config(float SystemClock, float Time_uS)
{
	RCC_WWDG_Clk_EN(WWDG_ClkEN);

	WWDG->CFR |= 3 << 7;
	WWDG->CR |= WWDG_CR_WDGA;
	WWDG_Refresh();
}

void WWDG_Refresh(void)
{
	WWDG->CR |= WWDG_CR_T;
}
