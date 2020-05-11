// Includes
//
#include "ZwTIM.h"
#include "ZwRCC.h"

// ��������� �������
void TIM_Config(TIM_TypeDef* TIMx, float SystemClock, float Period)
{
	uint32_t PSC_TEMP = (uint32_t)(SystemClock / 1000000 * Period / 65536);
	uint32_t ARR_TEMP = (uint32_t)((SystemClock / ((PSC_TEMP + 1) * 1000000)) * Period);

	TIMx->PSC = PSC_TEMP;	//�������� ������������
	TIMx->ARR = ARR_TEMP;	//�������� �� �������� ������� �������
	TIMx->EGR |= TIM_EGR_UG;
	TIMx->SR &= ~TIM_SR_UIF;
}
//-----------------------------------------------

void TIM15_16_17_PWM_CH1_Config(TIM_TypeDef* TIMx, float SystemClock, float Period)
{
	uint32_t PSC_TEMP = (uint32_t)(SystemClock / 1000000 * Period / 65536);
	uint32_t ARR_TEMP = (uint32_t)((SystemClock / ((PSC_TEMP + 1) * 1000000)) * Period);

	TIMx->PSC = PSC_TEMP;	//�������� ������������
	TIMx->ARR = ARR_TEMP;	//�������� �� �������� ������� �������
	TIMx->EGR |= TIM_EGR_UG;
	TIMx->SR &= ~TIM_SR_UIF;

	TIMx->CCR1 = 0;
	TIMx->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P;
	TIMx->BDTR |= TIM_BDTR_MOE;
	TIMx->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
}
//-----------------------------------------------

void TIM15_16_17_PWM_CH1_SetValue(TIM_TypeDef* TIMx, uint16_t PWM_Value)
{
	TIMx->CCR1 = PWM_Value;
}
//-----------------------------------------------

// ��������� ������������ �������
void TIM_Clock_En(uint32_t TIM_x)
{
	RCC_TIM_Clk_EN(TIM_x);
}
//-----------------------------------------------

// ��������� ������ �������
void TIM_MasterMode(TIM_TypeDef* TIMx, uint32_t MMS)
{
	TIMx->CR2 = MMS;
}
//-----------------------------------------------

// ��������� ���������� �������
void TIM_Interupt(TIM_TypeDef* TIMx, uint8_t Priority, bool EN)
{
	IRQn_Type TIMXinterupt;

	if (TIMx == TIM3)	TIMXinterupt = TIM3_IRQn;
	if (TIMx == TIM2)	TIMXinterupt = TIM2_IRQn;
	if (TIMx == TIM4)	TIMXinterupt = TIM4_IRQn;
	if (TIMx == TIM6)	TIMXinterupt = TIM6_DAC_IRQn;
	if (TIMx == TIM7)	TIMXinterupt = TIM7_IRQn;
	if (TIMx == TIM15)	TIMXinterupt = TIM1_BRK_TIM15_IRQn;

	if (EN)
	{
		TIMx->DIER |= TIM_DIER_UIE;
		// ��������� ���������� �� ��������� �� ���������
		// NVIC_SetPriority(TIMXinterupt, Priority);
		NVIC_EnableIRQ(TIMXinterupt);
	}
	else
	{
		TIMx->DIER &= ~TIM_DIER_UIE;
		NVIC_DisableIRQ(TIMXinterupt);
	}
}
//-----------------------------------------------

// ���������� ������ DMA
void TIM_DMA(TIM_TypeDef* TIMx, uint32_t DMA_MODE)
{
	TIMx->DIER |= DMA_MODE;
}
//-----------------------------------------------

// ����� �������� �������
void TIM_Reset(TIM_TypeDef* TIMx)
{
	TIMx->CNT = 0;
	if (TIM_StatusCheck(TIMx)) TIM_StatusClear(TIMx);
}
//-----------------------------------------------

// ������ ����
void TIM_Stop(TIM_TypeDef* TIMx)
{
	TIMx->CR1 &= ~TIM_CR1_CEN;
}
//-----------------------------------------------

// ������ �����
void TIM_Start(TIM_TypeDef* TIMx)
{
	TIMx->CR1 |= TIM_CR1_CEN;
	__asm("nop");
}
//-----------------------------------------------

// ������ �������
bool TIM_StatusCheck(TIM_TypeDef* TIMx)
{
	return (TIMx->SR & TIM_SR_UIF);
}
//-----------------------------------------------

// ������� �������
void TIM_StatusClear(TIM_TypeDef* TIMx)
{
	TIMx->SR &= ~TIM_SR_UIF;
}
//-----------------------------------------------
