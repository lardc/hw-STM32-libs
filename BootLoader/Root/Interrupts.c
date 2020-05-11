// Include
//
#include "Interrupts.h"
//
#include "stdinc.h"
#include "Controller.h"
#include "BoardConfig.h"


// Functions
//
void USART1_IRQHandler()
{
	if (ZwSCI_RecieveCheck(USART1))
	{
		ZwSCI_RegisterToFIFO(USART1);
		ZwSCI_RecieveFlagClear(USART1);
	}
}
//--------------------------------------------

void TIM2_IRQHandler()
{
	static Int32U BlinkCounter = 0;

	if (TIM_StatusCheck(TIM2))
	{
		++BlinkCounter;
		++CONTROL_TimeCounter;

		if (BlinkCounter > TIMER2_BLINK_PERIOD)
		{
			GPIO_Read_Bit(LED_BLINK_PORT, LED_BLINK_PIN) ?
					GPIO_Bit_Rst(LED_BLINK_PORT, LED_BLINK_PIN) :
					GPIO_Bit_Set(LED_BLINK_PORT, LED_BLINK_PIN);
			BlinkCounter = 0;
		}

		TIM_StatusClear(TIM2);
	}
}
//--------------------------------------------

void USB_LP_CAN_RX0_IRQHandler()
{
	if (NCAN_RecieveCheck())
	{
		NCAN_RecieveData();
		NCAN_RecieveFlagReset();
	}
}
//-----------------------------------------


// Default system handlers
//
// This function handles NMI exception
void NMI_Handler() {}

// This function handles Hard Fault exception
void HardFault_Handler()
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {}
}

// This function handles Memory Manage exception
void MemManage_Handler()
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {}
}

// This function handles Bus Fault exception
void BusFault_Handler()
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {}
}

// This function handles Usage Fault exception
void UsageFault_Handler()
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {}
}

// This function handles SVCall exception
void SVC_Handler() {}

// This function handles Debug Monitor exception
void DebugMon_Handler() {}

// This function handles PendSVC exception
void PendSV_Handler() {}

// This function handles SysTick Handler
void SysTick_Handler() {}
//--------------------------------------------
