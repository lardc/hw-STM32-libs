#ifndef __RCC_H
#define __RCC_H

// Includes
//
#include "stdinc.h"
#include "ZwGPIO.h"

// ������������� TrueSTUDIO � IAR
#ifndef RCC_APB1ENR_CAN1EN
	#define RCC_APB1ENR_CAN1EN RCC_APB1ENR_CANEN
#endif
#ifndef RCC_APB1RSTR_CAN1RST
	#define RCC_APB1RSTR_CAN1RST RCC_APB1RSTR_CANRST
#endif
#ifndef RCC_CFGR_PLLSRC_PREDIV1
	#define RCC_CFGR_PLLSRC_PREDIV1 RCC_CFGR_PLLSRC
#endif


#define PORTA			RCC_AHBENR_GPIOAEN
#define PORTB			RCC_AHBENR_GPIOBEN
#define PORTC			RCC_AHBENR_GPIOCEN
#define PORTD			RCC_AHBENR_GPIODEN
#define PORTE			RCC_AHBENR_GPIOEEN
#define PORTF			RCC_AHBENR_GPIOFEN

#define TIM_1			RCC_APB2ENR_TIM1EN
#define TIM_2			RCC_APB1ENR_TIM2EN
#define TIM_3			RCC_APB1ENR_TIM3EN
#define TIM_4			RCC_APB1ENR_TIM4EN
#define TIM_6			RCC_APB1ENR_TIM6EN
#define TIM_7			RCC_APB1ENR_TIM7EN
#define TIM_8			RCC_APB2ENR_TIM8EN
#define TIM_15			RCC_APB2ENR_TIM15EN
#define TIM_16			RCC_APB2ENR_TIM16EN
#define TIM_17			RCC_APB2ENR_TIM17EN

#define ADC_12_ClkEN	RCC_AHBENR_ADC12EN
#define ADC_34_ClkEN	RCC_AHBENR_ADC34EN
#define CAN_1_ClkEN		RCC_APB1ENR_CAN1EN
#define USART_1_ClkEN	RCC_APB2ENR_USART1EN
#define USART_2_ClkEN	RCC_APB1ENR_USART2EN
#define USART_3_ClkEN	RCC_APB1ENR_USART3EN
#define DAC_1_ClkEN		RCC_APB1ENR_DAC1EN
#define DAC_2_ClkEN		RCC_APB1ENR_DAC2EN
#define DMA_ClkEN		RCC_AHBENR_DMA1EN	// ���������� �����������
#define DMA1_ClkEN		RCC_AHBENR_DMA1EN
#define DMA2_ClkEN		RCC_AHBENR_DMA2EN
#define WWDG_ClkEN		RCC_APB1ENR_WWDGEN

#define PLL_2			RCC_CFGR_PLLMUL2
#define PLL_3			RCC_CFGR_PLLMUL3
#define PLL_4			RCC_CFGR_PLLMUL4
#define PLL_5			RCC_CFGR_PLLMUL5
#define PLL_6			RCC_CFGR_PLLMUL6
#define PLL_7			RCC_CFGR_PLLMUL7
#define PLL_8			RCC_CFGR_PLLMUL8
#define PLL_9			RCC_CFGR_PLLMUL9
#define PLL_10			RCC_CFGR_PLLMUL10
#define PLL_11			RCC_CFGR_PLLMUL11
#define PLL_12			RCC_CFGR_PLLMUL12
#define PLL_13			RCC_CFGR_PLLMUL13
#define PLL_14			RCC_CFGR_PLLMUL14
#define PLL_15			RCC_CFGR_PLLMUL15
#define PLL_16			RCC_CFGR_PLLMUL16

#define PREDIV_1		RCC_CFGR2_PREDIV1_DIV1
#define PREDIV_2		RCC_CFGR2_PREDIV1_DIV2
#define PREDIV_3		RCC_CFGR2_PREDIV1_DIV3
#define PREDIV_4		RCC_CFGR2_PREDIV1_DIV4
#define PREDIV_5		RCC_CFGR2_PREDIV1_DIV5
#define PREDIV_6		RCC_CFGR2_PREDIV1_DIV6
#define PREDIV_7		RCC_CFGR2_PREDIV1_DIV7
#define PREDIV_8		RCC_CFGR2_PREDIV1_DIV8
#define PREDIV_9		RCC_CFGR2_PREDIV1_DIV9
#define PREDIV_10		RCC_CFGR2_PREDIV1_DIV10
#define PREDIV_11		RCC_CFGR2_PREDIV1_DIV11
#define PREDIV_12		RCC_CFGR2_PREDIV1_DIV12
#define PREDIV_13		RCC_CFGR2_PREDIV1_DIV13
#define PREDIV_14		RCC_CFGR2_PREDIV1_DIV14
#define PREDIV_15		RCC_CFGR2_PREDIV1_DIV15
#define PREDIV_16		RCC_CFGR2_PREDIV1_DIV16

#define PLLSRC_HSIDIV2		RCC_CFGR_PLLSRC_HSI_DIV2
#define PLLSRC_HSEPREDIV	RCC_CFGR_PLLSRC_HSE_PREDIV

#define AltFn_0		0
#define AltFn_1		1
#define AltFn_2		2
#define AltFn_3		3
#define AltFn_4		4
#define AltFn_5		5
#define AltFn_6		6
#define AltFn_7		7
#define AltFn_8		8
#define AltFn_9		9
#define AltFn_10	10
#define AltFn_11	11
#define AltFn_12	12
#define AltFn_13	13
#define AltFn_14	14
#define AltFn_15	15


void RCC_GPIO_Clk_EN(uint32_t GPIOx);
void RCC_TIM_Clk_EN(uint32_t TIM_x);
void RCC_ADC_Clk_EN(uint32_t ADC_xx);
Boolean RCC_PLL_HSI_Config(uint32_t Pll_Mull);
void RCC_HSI_Config();
void RCC_HSE_Config();
void RCC_SysCfg_Clk_EN();
void RCC_CAN_Clk_EN(uint32_t CAN_x);
void GPIO_AltFn(GPIO_TypeDef * GPIOx, uint8_t Pin, uint8_t AltFn_Num);
void RCC_USART_Clk_EN(uint32_t UART_x);
void RCC_DAC_Clk_EN(uint32_t DACx);
void RCC_DMA_Clk_EN(uint32_t DMA_ClockEnable);
Boolean RCC_PLL_HSE_Config(uint32_t Fhse, uint32_t PreDiv, uint32_t PllMull);
void RCC_WWDG_Clk_EN(uint32_t WWDG_ClockEnable);
void RCC_LSI_Config();

#endif
