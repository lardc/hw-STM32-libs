// Includes
//
#include "ZwRCC.h"


// Разрешение татирования WWDG
void RCC_WWDG_Clk_EN(uint32_t WWDG_ClockEnable)
{
	RCC->APB1ENR |= WWDG_ClockEnable;
}
//-------------------------------------------------

// Разрешение тактирования DMA
void RCC_DMA_Clk_EN(uint32_t DMA_ClockEnable)
{
	RCC->AHBENR |= DMA_ClockEnable;
}
//-------------------------------------------------

// Разрешение тактирования ЦАПа
void RCC_DAC_Clk_EN(uint32_t DACx)
{
	RCC->APB1ENR |= DACx;
}
//-------------------------------------------------

// Разрешение тактирования порта
void RCC_GPIO_Clk_EN(uint32_t GPIOx)
{
	RCC->AHBENR |= GPIOx;
}
//-------------------------------------------------

// Разрешение тактирования CAN модуля
void RCC_CAN_Clk_EN(uint32_t CAN_x)
{
	RCC->APB1ENR |= CAN_x;
}
//-------------------------------------------------

// Разрешение тактирования UART модуля
void RCC_USART_Clk_EN(uint32_t USART_x)
{
	switch (USART_x)
	{
		case USART_1_ClkEN:
			RCC->APB2ENR |= USART_1_ClkEN;
			break;
		
		case USART_2_ClkEN:
			RCC->APB1ENR |= USART_2_ClkEN;
			break;
		
		case USART_3_ClkEN:
			RCC->APB1ENR |= USART_3_ClkEN;
			break;
	}
}
//-------------------------------------------------

// Разрешение тактирование модуля SYSCONFIG
void RCC_SysCfg_Clk_EN()
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}
//-------------------------------------------------

// Разрешение тактирования таймеров
void RCC_TIM_Clk_EN(uint32_t TIM_x)
{
	switch (TIM_x)
	{
		case TIM_1:
			RCC->APB2ENR |= TIM_1;
			break;
		
		case TIM_2:
			RCC->APB1ENR |= TIM_2;
			break;
		
		case TIM_3:
			RCC->APB1ENR |= TIM_3;
			break;
		
		case TIM_4:
			RCC->APB1ENR |= TIM_4;
			break;
		
		case TIM_6:
			RCC->APB1ENR |= TIM_6;
			break;
		
		case TIM_7:
			RCC->APB1ENR |= TIM_7;
			break;
		
		case TIM_15:
			RCC->APB2ENR |= TIM_15;
			break;
		
		case TIM_16:
			RCC->APB2ENR |= TIM_16;
			break;
		
		case TIM_17:
			RCC->APB2ENR |= TIM_17;
			break;
	}
}
//-------------------------------------------------

// Разрешение тактирования АЦП
void RCC_ADC_Clk_EN(uint32_t ADC_xx)
{
	RCC->AHBENR |= ADC_xx;
}
//-------------------------------------------------

Boolean RCC_PLL_HSI_Config(uint32_t Pll_Mull)
{
	RCC->CR |= RCC_CR_HSION; //Включение внутренний генератор на 8МГц
	while (!(RCC->CR & RCC_CR_HSIRDY)); //Ожидание включения внутреннего генератора

	RCC->CFGR = 0; //Сброс кsонфигурации
	RCC->CFGR |= Pll_Mull; //Коэффициент умножения 6
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;
	RCC->CR |= RCC_CR_PLLON; //Включаем PLL
	while (!(RCC->CR & RCC_CR_PLLRDY)); //Ждем готовность PLL

	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	if (Pll_Mull < PLL_3)
		FLASH->ACR = FLASH_ACR_LATENCY_0;
	else if (Pll_Mull < PLL_6)
		FLASH->ACR = FLASH_ACR_LATENCY_1;

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; //Переключаемся на PLL
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //Ожидание переключения на PLL

	return TRUE;
}
//-------------------------------------------------

Boolean RCC_PLL_HSE_Config(uint32_t Fhse, uint32_t PreDiv, uint32_t PllMull)
{
	float Fsys = (((float)Fhse) / (PreDiv + 1)) * ((PllMull >> 18) + 2);
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY)); //Ожидание включения внешнего генератора

	//Сброс кsонфигурации
	RCC->CFGR = 0;
	RCC->CFGR2 = 0;

	RCC->CFGR |= PllMull; //Коэффициент умножения
	RCC->CFGR |= RCC_CFGR_PLLSRC_PREDIV1;
	RCC->CFGR2 |= PreDiv;
	RCC->CR |= RCC_CR_PLLON; //Включаем PLL
	while (!(RCC->CR & RCC_CR_PLLRDY)); //Ждем готовность PLL

	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	if ((Fsys > 24000000) && (Fsys <= 48000000))
		FLASH->ACR |= FLASH_ACR_LATENCY_0;
	if ((Fsys > 48000000) && (Fsys <= 72000000))
		FLASH->ACR |= FLASH_ACR_LATENCY_1;

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; //Переключаемся на PLL
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //Ожидание переключения на PLL

	return TRUE;
}
//-------------------------------------------------

void RCC_HSI_Config()
{
	RCC->CR = 0;
	RCC->CR |= RCC_CR_HSION; //Включение внутренний генератор на 8МГц
	while (!(RCC->CR & RCC_CR_HSIRDY)); //Ожидание включения внутреннего генератора
	
	RCC->CFGR = 0;
	RCC->CFGR |= RCC_CFGR_SW_HSI; //Переключение системы тактирования на внутренний генератор
	while ((RCC->CFGR & ~(RCC_CFGR_SWS)) != RCC_CFGR_SWS_HSI); //Ожидание переключения
}
//-------------------------------------------------

void RCC_HSE_Config()
{
	RCC->CR = 0;
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));
	
	RCC->CFGR = 0;
	RCC->CFGR |= RCC_CFGR_SW_HSE;
	while (!(RCC->CFGR & RCC_CFGR_SWS_HSE));
}
//-------------------------------------------------

// Включение LSI генератора
void RCC_LSI_Config()
{
	RCC->CSR |= RCC_CSR_LSION;
	while ((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY);
}
//-------------------------------------------------
