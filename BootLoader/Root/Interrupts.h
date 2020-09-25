#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H


void USART1_IRQHandler();
void TIM6_DAC1_IRQHandler();
void TIM2_IRQHandler();
void USB_LP_CAN1_RX0_IRQHandler();

void NMI_Handler();
void HardFault_Handler();
void MemManage_Handler();
void BusFault_Handler();
void UsageFault_Handler();
void SVC_Handler();
void DebugMon_Handler();
void PendSV_Handler();
void SysTick_Handler();


#endif // __INTERRUPTS_H
