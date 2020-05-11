// Includes
//
#include "ZwRCC.h"
#include "ZwDAC.h"


// ����� ��������� ����
void DAC_Reset(DAC_TypeDef *DACx)
{
	DACx->CR = 0;
}
//-----------------------------------------------

// ���������� ������������ ����
void DAC_ClkEnable(DAC_TypeDef *DACx)
{
	if (DACx == DAC1)
		RCC_DAC_Clk_EN(RCC_APB1ENR_DAC1EN);
	else if (DACx == DAC2)
		RCC_DAC_Clk_EN(RCC_APB1ENR_DAC2EN);
}
//-----------------------------------------------

// ������������ �������� ���� ����� 1
void DAC_TriggerConfigCh1(DAC_TypeDef *DACx, uint32_t TrigMask, uint32_t TrigEnableBit)
{
	DACx->CR &= ~DAC_CR_TSEL1;
	DACx->CR |= TrigMask & DAC_CR_TSEL1;

	DACx->CR &= ~DAC_CR_TEN1;
	DACx->CR |= TrigEnableBit & DAC_CR_TEN1;
}
//-----------------------------------------------

// ������������ �������� ���� ����� 2
void DAC_TriggerConfigCh2(DAC_TypeDef *DACx, uint32_t TrigMask, uint32_t TrigEnableBit)
{
	DACx->CR &= ~DAC_CR_TSEL2;
	DACx->CR |= TrigMask & DAC_CR_TSEL2;

	DACx->CR &= ~DAC_CR_TEN2;
	DACx->CR |= TrigEnableBit & DAC_CR_TEN2;
}
//-----------------------------------------------

// ��������� ��� ����� 1
void DAC_EnableCh1(DAC_TypeDef *DACx)
{
	DACx->CR |= DAC_CR_EN1;
}
//-----------------------------------------------

// ��������� ��� ����� 2
void DAC_EnableCh2(DAC_TypeDef *DACx)
{
	DACx->CR |= DAC_CR_EN2;
}
//-----------------------------------------------

// ���/���� ��������� ������� ����� 1
void DAC_BufferCh1(DAC_TypeDef *DACx, bool Enable)
{
	if (Enable)
		DACx->CR &= ~DAC_CR_BOFF1;
	else
		DACx->CR |= DAC_CR_BOFF1;
}
//-----------------------------------------------

// ���/���� ��������� ������� ����� 2
void DAC_BufferCh2(DAC_TypeDef *DACx, bool Enable)
{
	if (Enable)
		DACx->CR &= ~DAC_CR_BOFF2;
	else
		DACx->CR |= DAC_CR_BOFF2;
}
//-----------------------------------------------

// ������������ ������������ �������� ������ 1 ����
void DAC_ForceSWTrigCh1(DAC_TypeDef *DACx)
{
	DACx->SWTRIGR = DAC_SWTRIGR_SWTRIG1;
}
//-----------------------------------------------

// ������������ ������������ �������� ������ 2 ����
void DAC_ForceSWTrigCh2(DAC_TypeDef *DACx)
{
	DACx->SWTRIGR = DAC_SWTRIGR_SWTRIG2;
}
//-----------------------------------------------

// ��������� �������� ������ 1 ����
void DAC_SetValueCh1(DAC_TypeDef *DACx, uint16_t Value)
{
	DACx->DHR12R1 = Value;
}
//-----------------------------------------------

// ��������� �������� ������ 2 ����
void DAC_SetValueCh2(DAC_TypeDef *DACx, uint16_t Value)
{
	DACx->DHR12R2 = Value;
}
//-----------------------------------------------

// ���������� ������ ������ DMA �� ������ 1
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

// ���������� ������ ������ DMA �� ������ 2
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


//############## ���������� ������� ��� ������ ������ � DAC1 ###################


// ����� ��������� ����
void DACx_Reset(void)
{
	DAC->CR = 0;
}
//-----------------------------------------------

// ����� ��������� ��������
void DAC_Wave_Config(uint32_t MAMPx_Config, uint32_t WAVEx_Config)
{
	DAC->CR |= MAMPx_Config;
	DAC->CR |= WAVEx_Config;
}
//-----------------------------------------------

// ������������ �������� ����
void DAC_Trigger_Config(uint32_t TriggerSelect, uint32_t TriggerEnable)
{
	DAC->CR |= TriggerSelect;
	DAC->CR |= TriggerEnable;
}
//-----------------------------------------------

// ���������� ������ ������ DMA
void DACx_DMA_Config(uint32_t DMAEnable, uint32_t DMAUdInt)
{
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_TIM6DAC1Ch1_DMA_RMP;
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_TIM7DAC1Ch2_DMA_RMP;

	DAC->CR |= DMAUdInt;
	DAC->CR |= DMAEnable;
}
//-----------------------------------------------

// ���������� ������������ ����
void DACx_Clk_Enable(uint32_t DACx)
{
	RCC_DAC_Clk_EN(DACx);
}
//-----------------------------------------------

// ��������� ��� 1 ��� 2
void DACx_Enable(uint32_t DACx)
{
	DAC->CR |= DACx;
}
//-----------------------------------------------

// ��������� ������������ ��������
bool DAC_SoftTrigEnable(uint32_t SoftTrigXEnable)
{
	DAC->SWTRIGR |= SoftTrigXEnable;
	while (DAC->SWTRIGR & SoftTrigXEnable);

	return true;
}
//-----------------------------------------------

// ���/���� ��������� �������
void DAC_Buff(uint32_t BUFFx, bool EN)
{
	if (EN == true)
		DAC->CR &= ~EN;
	else
		DAC->CR |= EN;
}
//-----------------------------------------------

// ��������� �������� ������ 1 ����
void DAC_CH1_SetValue(uint16_t Value)
{
	DAC->DHR12R1 = Value;
}
//-----------------------------------------------

// ��������� �������� ������ 2 ����
void DAC_CH2_SetValue(uint16_t Value)
{
	DAC->DHR12R2 = Value;
}
//-----------------------------------------------
