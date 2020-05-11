#ifndef __ZW_ADC_H
#define __ZW_ADC_H

// Includes
//
#include "stdinc.h"

// Definitions
//
#define RISE					ADC_CFGR_EXTEN_0
#define FALL					ADC_CFGR_EXTEN_1
#define BOTH					ADC_CFGR_EXTEN
//
#define ADCxx_SOFT_TRIG			0
//
#define ADC12_TIM1_CC1			0
#define ADC12_TIM1_CC2			ADC_CFGR_EXTSEL_0
#define ADC12_TIM1_CC3			ADC_CFGR_EXTSEL_1
#define ADC12_TIM2_CC2			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1)
#define ADC12_TIM3_TRGO			ADC_CFGR_EXTSEL_2
#define ADC12_TIM4_CC4			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_2)
#define ADC12_EXTI11			(ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2)
#define ADC12_TIM8_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2)
#define ADC12_TIM8_TRGO2		ADC_CFGR_EXTSEL_3
#define ADC12_TIM1_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_3)
#define ADC12_TIM1_TRGO2		(ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_3)
#define ADC12_TIM2_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_3)
#define ADC12_TIM4_TRGO			(ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
#define ADC12_TIM6_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
#define ADC12_TIM15_TRGO		(ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
#define ADC12_TIM3_CC4			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
//
#define ADC34_TIM3_CC1			0
#define ADC34_TIM2_CC3			ADC_CFGR_EXTSEL_0
#define ADC34_TIM1_CC3			ADC_CFGR_EXTSEL_1
#define ADC34_TIM8_CC1			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1)
#define ADC34_TIM8_TRGO			ADC_CFGR_EXTSEL_2
#define ADC34_EXTI2				(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_2)
#define ADC34_TIM4_CC1			(ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2)
#define ADC34_TIM2_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2)
#define ADC34_TIM8_TRGO2		ADC_CFGR_EXTSEL_3
#define ADC34_TIM1_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_3)
#define ADC34_TIM1_TRGO2		(ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_3)
#define ADC34_TIM3_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_3)
#define ADC34_TIM4_TRGO			(ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
#define ADC34_TIM7_TRGO			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
#define ADC34_TIM15_TRGO		(ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
#define ADC34_TIM2_CC1			(ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3)
//
// ADC sampling time in ADC clock cycles
#define ADC_SMPL_TIME_1_5		0x0
#define ADC_SMPL_TIME_2_5		0x1
#define ADC_SMPL_TIME_4_5		0x2
#define ADC_SMPL_TIME_7_5		0x3
#define ADC_SMPL_TIME_19_5		0x4
#define ADC_SMPL_TIME_61_5		0x5
#define ADC_SMPL_TIME_181_5		0x6
#define ADC_SMPL_TIME_601_5		0x7
//
// ADC interrupt source
#define JQOVFIE					BIT10
#define AWD3IE					BIT9
#define AWD2IE					BIT8
#define AWD1IE					BIT7
#define JEOSIE					BIT6
#define JEOCIE					BIT5
#define OVRIE					BIT4
#define EOSIE					BIT3
#define EOCIE					BIT2
#define EOSMPIE					BIT1
#define ADRDYIE					BIT0
//
// ADCn interrupt clear flag
#define JQOVF					BIT10
#define AWD3					BIT9
#define AWD2					BIT8
#define AWD1					BIT7
#define JEOS					BIT6
#define JEOC					BIT5
#define OVR						BIT4
#define EOS						BIT3
#define EOC						BIT2
#define EOSMP					BIT1
#define ADRDY					BIT0

// Functions
//
uint16_t ADC_Measure(ADC_TypeDef * ADCx, uint32_t ChannelNumber);
void ADC_WaitEndOfOperation(ADC_TypeDef* ADCx);
void ADC_SamplingTrig(ADC_TypeDef* ADCx);
void ADC_SamplingStop(ADC_TypeDef* ADCx);
void ADC_SoftTrigConfig(ADC_TypeDef * ADCx);
void ADC_Calibration(ADC_TypeDef * ADCx);
void ADC_WatchDogThreshold(uint32_t Threshold);
void ADC_WatchDogStatusClear(ADC_TypeDef *ADCx);
void ADC_TrigConfig(ADC_TypeDef *ADCx, uint32_t Trigger, uint32_t Edge);
void ADC_ChannelSeqLen(ADC_TypeDef *ADCx,uint32_t Length);
void ADC_ChannelSeqReset(ADC_TypeDef* ADCx);
void ADC_ChannelSet_Sequence1_4(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t Sequence); // Устаревшая функция
void ADC_ChannelSet_Sequence(ADC_TypeDef* ADCx, uint32_t Channel, uint32_t Sequence);
void ADC_ChannelSet_SampleTime(ADC_TypeDef* ADCx, uint32_t Channel, uint32_t Time);
void ADC_DMAConfig(ADC_TypeDef *ADCx);
void ADC_DMAEnable(ADC_TypeDef* ADCx, bool Enable);
void ADC_Enable(ADC_TypeDef *ADCx);
void ADC_Disable(ADC_TypeDef* ADCx);
float ADC_TemperatureMeasuring(ADC_TypeDef * ADCx);
void ADC_Interrupt(ADC_TypeDef *ADCx, uint32_t InterruptSource, uint32_t Priority, bool EN);
void ADC_InterruptClearFlag(ADC_TypeDef *ADCx, uint32_t Flag);
uint16_t ADC_Read(ADC_TypeDef * ADCx);
void ADC1_2_SetDualMode(bool Enable);

#endif // __ZW_ADC_H
