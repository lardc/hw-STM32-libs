// Includes
//
#include "ZwRCC.h"
#include "ZwDAC.h"


// Сброс регистров ЦАПа
void DAC_Reset(DAC_TypeDef *DACx)
{
	DACx->CR = 0;
}
//-----------------------------------------------

// Разрешение тактирования ЦАПа
void DAC_ClkEnable(DAC_TypeDef *DACx)
{
	if (DACx == DAC1)
		RCC_DAC_Clk_EN(RCC_APB1ENR_DAC1EN);
	else if (DACx == DAC2)
		RCC_DAC_Clk_EN(RCC_APB1ENR_DAC2EN);
}
//-----------------------------------------------

// Конфигурация триггера ЦАПа канал 1
void DAC_TriggerConfigCh1(DAC_TypeDef *DACx, uint32_t TrigMask, uint32_t TrigEnableBit)
{
	DACx->CR &= ~DAC_CR_TSEL1;
	DACx->CR |= TrigMask & DAC_CR_TSEL1;

	DACx->CR &= ~DAC_CR_TEN1;
	DACx->CR |= TrigEnableBit & DAC_CR_TEN1;
}
//-----------------------------------------------

// Конфигурация триггера ЦАПа канал 2
void DAC_TriggerConfigCh2(DAC_TypeDef *DACx, uint32_t TrigMask, uint32_t TrigEnableBit)
{
	DACx->CR &= ~DAC_CR_TSEL2;
	DACx->CR |= TrigMask & DAC_CR_TSEL2;

	DACx->CR &= ~DAC_CR_TEN2;
	DACx->CR |= TrigEnableBit & DAC_CR_TEN2;
}
//-----------------------------------------------

// Включение ЦАП канал 1
void DAC_EnableCh1(DAC_TypeDef *DACx)
{
	DACx->CR |= DAC_CR_EN1;
}
//-----------------------------------------------

// Включение ЦАП канал 2
void DAC_EnableCh2(DAC_TypeDef *DACx)
{
	DACx->CR |= DAC_CR_EN2;
}
//-----------------------------------------------

// Вкл/выкл выходного буффера канал 1
void DAC_BufferCh1(DAC_TypeDef *DACx, bool Enable)
{
	if (Enable)
		DACx->CR &= ~DAC_CR_BOFF1;
	else
		DACx->CR |= DAC_CR_BOFF1;
}
//-----------------------------------------------

// Вкл/выкл выходного буффера канал 2
void DAC_BufferCh2(DAC_TypeDef *DACx, bool Enable)
{
	if (Enable)
		DACx->CR &= ~DAC_CR_BOFF2;
	else
		DACx->CR |= DAC_CR_BOFF2;
}
//-----------------------------------------------

// Форсирование программного триггера канала 1 ЦАПа
void DAC_ForceSWTrigCh1(DAC_TypeDef *DACx)
{
	DACx->SWTRIGR = DAC_SWTRIGR_SWTRIG1;
}
//-----------------------------------------------

// Форсирование программного триггера канала 2 ЦАПа
void DAC_ForceSWTrigCh2(DAC_TypeDef *DACx)
{
	DACx->SWTRIGR = DAC_SWTRIGR_SWTRIG2;
}
//-----------------------------------------------

// Установка значения канала 1 ЦАПа
void DAC_SetValueCh1(DAC_TypeDef *DACx, uint16_t Value)
{
	DACx->DHR12R1 = Value;
}
//-----------------------------------------------

// Установка значения канала 2 ЦАПа
void DAC_SetValueCh2(DAC_TypeDef *DACx, uint16_t Value)
{
	DACx->DHR12R2 = Value;
}
//-----------------------------------------------

// Разрешение работы модуля DMA на канале 1
void DAC_DMAConfigCh1(DAC_TypeDef *DACx, bool DMAEnable, bool DMAUnderrunIntEnable)
{
	if (DMAEnable)
	{
		SYSCFG->CFGR1 |= SYSCFG_CFGR1_TIM6DAC1Ch1_DMA_RMP;
		DAC->CR |= DAC_CR_DMAEN1;
	}
	else
	{
		SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_TIM6DAC1Ch1_DMA_RMP;
		DAC->CR &= ~DAC_CR_DMAEN1;
	}

	if (DMAUnderrunIntEnable)
		DAC->CR |= DAC_CR_DMAUDRIE1;
	else
		DAC->CR &= ~DAC_CR_DMAUDRIE1;
}
//-----------------------------------------------

// Разрешение работы модуля DMA на канале 2
void DAC_DMAConfigCh2(DAC_TypeDef *DACx, bool DMAEnable, bool DMAUnderrunIntEnable)
{
	if (DMAEnable)
	{
		SYSCFG->CFGR1 |= SYSCFG_CFGR1_TIM7DAC1Ch2_DMA_RMP;
		DAC->CR |= DAC_CR_DMAEN2;
	}
	else
	{
		SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_TIM7DAC1Ch2_DMA_RMP;
		DAC->CR &= ~DAC_CR_DMAEN2;
	}

	if (DMAUnderrunIntEnable)
		DAC->CR |= DAC_CR_DMAUDRIE2;
	else
		DAC->CR &= ~DAC_CR_DMAUDRIE2;
}
//-----------------------------------------------


//############## Устаревшие функции для работы только с DAC1 ###################


// Сброс регистров ЦАПа
void DACx_Reset(void)
{
	DAC->CR = 0;
}
//-----------------------------------------------

// Режим источника сигналов
void DAC_Wave_Config(uint32_t MAMPx_Config, uint32_t WAVEx_Config)
{
	DAC->CR |= MAMPx_Config;
	DAC->CR |= WAVEx_Config;
}
//-----------------------------------------------

// Конфигурация триггера ЦАПа
void DAC_Trigger_Config(uint32_t TriggerSelect, uint32_t TriggerEnable)
{
	DAC->CR |= TriggerSelect;
	DAC->CR |= TriggerEnable;
}
//-----------------------------------------------

// Разрешение работы модуля DMA
void DACx_DMA_Config(uint32_t DMAEnable, uint32_t DMAUdInt)
{
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_TIM6DAC1Ch1_DMA_RMP;
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_TIM7DAC1Ch2_DMA_RMP;

	DAC->CR |= DMAUdInt;
	DAC->CR |= DMAEnable;
}
//-----------------------------------------------

// Разрешение тактирования ЦАПа
void DACx_Clk_Enable(uint32_t DACx)
{
	RCC_DAC_Clk_EN(DACx);
}
//-----------------------------------------------

// Включение ЦАП 1 или 2
void DACx_Enable(uint32_t DACx)
{
	DAC->CR |= DACx;
}
//-----------------------------------------------

// Включение программного триггера
bool DAC_SoftTrigEnable(uint32_t SoftTrigXEnable)
{
	DAC->SWTRIGR |= SoftTrigXEnable;
	while (DAC->SWTRIGR & SoftTrigXEnable);

	return true;
}
//-----------------------------------------------

// Вкл/выкл выходного буффера
void DAC_Buff(uint32_t BUFFx, bool EN)
{
	if (EN == true)
		DAC->CR &= ~EN;
	else
		DAC->CR |= EN;
}
//-----------------------------------------------

// Установка значения канала 1 ЦАПа
void DAC_CH1_SetValue(uint16_t Value)
{
	DAC->DHR12R1 = Value;
}
//-----------------------------------------------

// Установка значения канала 2 ЦАПа
void DAC_CH2_SetValue(uint16_t Value)
{
	DAC->DHR12R2 = Value;
}
//-----------------------------------------------
