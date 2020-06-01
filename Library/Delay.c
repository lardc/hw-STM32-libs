// Header
//
#include "Delay.h"

// Definitions
#define ASM_NOP_x40 {__asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");\
					 __asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");\
					 __asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");\
					 __asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");}

// Functions
//
void __attribute__((optimize("O0"))) DELAY_US(uint32_t Delay)
{
	while (Delay--)
		ASM_NOP_x40;
}
//-----------------------------------------------
