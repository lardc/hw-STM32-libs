// Includes
//
#include "ZwADC.h"


// Совместимость TrueSTUDIO и IAR
#ifndef ADC1_2
	#define ADC1_2 ADC1_2_COMMON
#endif
#ifndef ADC3_4
	#define ADC3_4 ADC3_4_COMMON
#endif


// Functions
//
void ADC_WaitEndOfOperation(ADC_TypeDef* ADCx)
{
	while (ADCx->CR & ADC_CR_ADSTART);
}
//-----------------------------------------------

// Read ADC data
uint16_t ADC_Read(ADC_TypeDef* ADCx)
{
	return ADCx->DR;
}
//-----------------------------------------------

// Start ADC sampling
void ADC_SamplingStart(ADC_TypeDef* ADCx)
{
	ADCx->CR |= ADC_CR_ADSTART;
}
//-----------------------------------------------

// Stop ADC sampling
void ADC_SamplingStop(ADC_TypeDef* ADCx)
{
	if (ADCx->CR & ADC_CR_ADSTART)
	{
		ADCx->CR |= ADC_CR_ADSTP;
		while (ADCx->CR & ADC_CR_ADSTP);
	}
}
//-----------------------------------------------

// Start of ADC conversion
uint16_t ADC_Measure(ADC_TypeDef* ADCx, uint32_t ChannelNumber)
{
	ADC_WaitEndOfOperation(ADCx);
	ADCx->SQR1 = (0x1F & ChannelNumber) << 6;
	ADCx->CR |= ADC_CR_ADSTART;
	ADC_WaitEndOfOperation(ADCx);

	return ADCx->DR;
}
//-----------------------------------------------

// ADC calibration
void ADC_Calibration(ADC_TypeDef* ADCx)
{
	if ((ADCx == ADC1) || (ADCx == ADC2))
		ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
	else
		ADC3_4->CCR |= ADC12_CCR_CKMODE_0;

	// Отключение АЦП
	ADC_Disable(ADCx);

	// Проверка работы регулятора напряжения
	if ((ADCx->CR & ADC_CR_ADVREGEN_0) == 0)
	{
		// Включение регулятора
		ADCx->CR &= ~ADC_CR_ADVREGEN;
		ADCx->CR |= ADC_CR_ADVREGEN_0;

		// После запуска регулятора необходимо выждать не менее 10мкс
		// ожидаем с запасом
		uint32_t counter = 10000;
		while (--counter);
	}

	// Запуск калибровки
	ADCx->CR |= ADC_CR_ADCAL;
	while (ADCx->CR & ADC_CR_ADCAL);
}
//-----------------------------------------------

// Конфигурация АЦП (режим программного запуска)
void ADC_SoftTrigConfig(ADC_TypeDef* ADCx)
{
	ADC_TrigConfig(ADCx, 0, 0);
}
//-----------------------------------------------

void ADC_WatchDogThreshold(uint32_t Threshold)
{
	ADC1->TR1 = Threshold;
}
//-----------------------------------------------

void ADC_WatchDogStatusClear(ADC_TypeDef* ADCx)
{
	ADCx->ISR |= ADC_ISR_AWD1;
}
//-----------------------------------------------

void ADC_TrigConfig(ADC_TypeDef* ADCx, uint32_t Trigger, uint32_t Edge)
{
	ADC_WaitEndOfOperation(ADCx);
	ADCx->CFGR &= ~(ADC_CFGR_EXTSEL | ADC_CFGR_EXTEN);

	if ((Trigger | Edge) != ADCxx_SOFT_TRIG)
		ADCx->CFGR |= Trigger | Edge;
}
//-----------------------------------------------

void ADC_ChannelSeqLen(ADC_TypeDef* ADCx, uint32_t Length)
{
	ADC_WaitEndOfOperation(ADCx);
	ADCx->SQR1 = (~ADC_SQR1_L & ADCx->SQR1) | ((Length - 1) & ADC_SQR1_L);
}
//-----------------------------------------------

void ADC_ChannelSeqReset(ADC_TypeDef* ADCx)
{
	ADC_WaitEndOfOperation(ADCx);

	ADCx->SQR1 = 0;
	ADCx->SQR2 = 0;
	ADCx->SQR3 = 0;
	ADCx->SQR4 = 0;
}
//-----------------------------------------------

// Устаревшая функция
void ADC_ChannelSet_Sequence1_4(ADC_TypeDef* ADCx, uint32_t Channel, uint32_t Sequence)
{
	// Ограничение значения, чтобы не превышало размер поля
	Channel &= 0x1F;
	ADC_WaitEndOfOperation(ADCx);

	switch (Sequence)
	{
		case 1: ADCx->SQR1 |= Channel << 6;		break;
		case 2: ADCx->SQR1 |= Channel << 12;	break;
		case 3: ADCx->SQR1 |= Channel << 18;	break;
		case 4: ADCx->SQR1 |= Channel << 24;	break;
	}
}
//-----------------------------------------------

void ADC_ChannelSet_Sequence(ADC_TypeDef* ADCx, uint32_t Channel, uint32_t Sequence)
{
	// Ограничение значения, чтобы не превышало размер поля
	Channel &= 0x1F;
	ADC_WaitEndOfOperation(ADCx);

	switch (Sequence)
	{
		case 1:  ADCx->SQR1 |= Channel << 6;	break;
		case 2:  ADCx->SQR1 |= Channel << 12;	break;
		case 3:  ADCx->SQR1 |= Channel << 18;	break;
		case 4:  ADCx->SQR1 |= Channel << 24;	break;

		case 5:  ADCx->SQR2 |= Channel;			break;
		case 6:  ADCx->SQR2 |= Channel << 6;	break;
		case 7:  ADCx->SQR2 |= Channel << 12;	break;
		case 8:  ADCx->SQR2 |= Channel << 18;	break;
		case 9:  ADCx->SQR2 |= Channel << 24;	break;

		case 10: ADCx->SQR3 |= Channel;			break;
		case 11: ADCx->SQR3 |= Channel << 6;	break;
		case 12: ADCx->SQR3 |= Channel << 12;	break;
		case 13: ADCx->SQR3 |= Channel << 18;	break;
		case 14: ADCx->SQR3 |= Channel << 24;	break;

		case 15: ADCx->SQR4 |= Channel;			break;
		case 16: ADCx->SQR4 |= Channel << 6;	break;
	}
}
//-----------------------------------------------

void ADC_ChannelSet_SampleTime(ADC_TypeDef* ADCx, uint32_t Channel, uint32_t Time)
{
	// Ограничение значения, чтобы не превышало размер поля
	Time &= 0x7;
	ADC_WaitEndOfOperation(ADCx);

	if (Channel <= 9)
		ADCx->SMPR1 |= Time << (Channel * 3);
	else if (Channel <= 18)
		ADCx->SMPR2 |= Time << ((Channel - 10) * 3);
}
//-----------------------------------------------

void ADC_DMAConfig(ADC_TypeDef* ADCx)
{
	ADC_WaitEndOfOperation(ADCx);

	ADCx->CFGR |= ADC_CFGR_DMACFG;
	ADCx->CFGR |= ADC_CFGR_DMAEN;
}
//-----------------------------------------------

void ADC_DMAEnable(ADC_TypeDef* ADCx, bool Enable)
{
	ADC_WaitEndOfOperation(ADCx);

	if (Enable)
		ADCx->CFGR |= ADC_CFGR_DMAEN;
	else
		ADCx->CFGR &= ~ADC_CFGR_DMAEN;
}
//-----------------------------------------------

void ADC_Enable(ADC_TypeDef* ADCx)
{
	if (!(ADCx->CR & ADC_CR_ADEN))
	{
		ADCx->CR |= ADC_CR_ADEN;
		while (!(ADCx->ISR & ADC_ISR_ADRD));
	}
}
//-----------------------------------------------

void ADC_Disable(ADC_TypeDef* ADCx)
{
	if (ADCx->CR & ADC_CR_ADEN)
	{
		// Ожидание окончания текущей операции
		ADC_WaitEndOfOperation(ADCx);

		ADCx->CR |= ADC_CR_ADDIS;
		while (ADCx->CR & ADC_CR_ADDIS);

		// Очистка флага
		if (ADCx->ISR & ADRDY) ADCx->ISR |= ADRDY;
	}
}
//-----------------------------------------------

void ADC_Interrupt(ADC_TypeDef* ADCx, uint32_t InterruptSource, uint32_t Priority, bool Enable)
{
	ADC_WaitEndOfOperation(ADCx);

	if (Enable)
		ADCx->IER |= InterruptSource;
	else
		ADCx->IER &= ~InterruptSource;

	// Изменение приоритета по умолчанию не требуется
	if ((ADCx == ADC1) || (ADCx == ADC2))
	{
		// NVIC_SetPriority(ADC1_2_IRQn, Priority);
		NVIC_EnableIRQ(ADC1_2_IRQn);
	}

	if (ADCx == ADC3)
	{
		// NVIC_SetPriority(ADC3_IRQn, Priority);
		NVIC_EnableIRQ(ADC3_IRQn);
	}

	if (ADCx == ADC4)
	{
		// NVIC_SetPriority(ADC4_IRQn, Priority);
		NVIC_EnableIRQ(ADC4_IRQn);
	}
}
//-----------------------------------------------

void ADC_InterruptClearFlag(ADC_TypeDef* ADCx, uint32_t Flag)
{
	ADCx->ISR |= Flag;
}
//-----------------------------------------------

void ADC1_2_SetDualMode(bool Enable)
{
	// Отключение обоих АЦП
	ADC_Disable(ADC1);
	ADC_Disable(ADC2);

	ADC1_2->CCR &= ~ADC12_CCR_MULTI;
	if (Enable)
		ADC1_2->CCR |= ADC12_CCR_MULTI_1 | ADC12_CCR_MULTI_2;
}
//-----------------------------------------------

void ADC3_4_SetDualMode(bool Enable)
{
	// Отключение обоих АЦП
	ADC_Disable(ADC3);
	ADC_Disable(ADC4);

	ADC3_4->CCR &= ~ADC34_CCR_MULTI;
	if (Enable)
		ADC3_4->CCR |= ADC34_CCR_MULTI_1 | ADC34_CCR_MULTI_2;
}
//-----------------------------------------------

void ADC_SetContinuousMode(ADC_TypeDef* ADCx)
{
	ADCx->CFGR |= ADC_CFGR_CONT;
}
//-----------------------------------------------

void ADC_ResetConfig(ADC_TypeDef* ADCx)
{
	ADC_SamplingStop(ADCx);
	ADCx->CR = 0;
	ADCx->CFGR = 0;
}
//-----------------------------------------------
