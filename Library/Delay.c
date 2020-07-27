// Header
#include "Delay.h"

// Includes
#include "SysConfig.h"

// Definitions
#define SYSCLK_MHZ		(SYSCLK / 1000000)

// Functions
//
void __attribute__((optimize("O0"))) __DELAY(uint32_t Delay)
{
	while (--Delay);
}
//-----------------------------------------------

void __attribute__((optimize("O3"))) DELAY_US(uint32_t Delay)
{
	// Исполнение одного цикла __DELAY занимает 11 тактов
	__DELAY(Delay * SYSCLK_MHZ / 11);
}
//-----------------------------------------------

void __attribute__((optimize("O3"))) DELAY_MS(uint32_t Delay)
{
	DELAY_US(Delay * 1000);
}
//-----------------------------------------------
