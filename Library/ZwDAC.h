#ifndef __ZW_DAC_H
#define __ZW_DAC_H

// Includes
//
#include "stdinc.h"

// Definitions
//
#define DAC1ENABLE				DAC_CR_EN1
#define DAC2ENABLE				DAC_CR_EN2
#define DAC_DMA1UdIntENABLE		DAC_CR_DMAUDRIE1 
#define DAC_DMA1UdIntDISABLE	0
#define DAC_DMA2UdIntENABLE		DAC_CR_DMAUDRIE2 
#define DAC_DMA2UdIntDISABLE	0
#define DAC_DMA1ENABLE			DAC_CR_DMAEN1
#define DAC_DMA2ENABLE			DAC_CR_DMAEN2
#define WAVE1_NOISE				DAC_CR_WAVE1_0
#define WAVE1_TRIANGLE			DAC_CR_WAVE1_1 
#define WAVE2_NOISE				DAC_CR_WAVE2_0 
#define WAVE2_TRIANGLE			DAC_CR_WAVE2_1
#define TRIG1_TIMER6			0
#define TRIG1_TIMER15			(DAC_CR_TSEL1_0 | DAC_CR_TSEL1_1)
#define TRIG1_TIMER2			DAC_CR_TSEL1_2
#define TRIG1_TIMER4			(DAC_CR_TSEL1_0 | DAC_CR_TSEL1_2)
#define TRIG1_EXTI9				(DAC_CR_TSEL1_1 | DAC_CR_TSEL1_2)
#define TRIG1_SOFTWARE			(DAC_CR_TSEL1_0 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_2)
#define TRIG2_TIMER6			0
#define TRIG2_TIMER15			(DAC_CR_TSEL2_0 | DAC_CR_TSEL2_1)
#define TRIG2_TIMER2			DAC_CR_TSEL2_2
#define TRIG2_TIMER4			(DAC_CR_TSEL2_0 | DAC_CR_TSEL2_2)
#define TRIG2_EXTI9				(DAC_CR_TSEL2_1 | DAC_CR_TSEL2_2)
#define TRIG2_SOFTWARE			(DAC_CR_TSEL2_0 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_2)
#define TRIG1_ENABLE			DAC_CR_TEN1
#define TRIG2_ENABLE			DAC_CR_TEN2
#define SoftTrig1Enable			DAC_SWTRIGR_SWTRIG1
#define SoftTrig2Enable			DAC_SWTRIGR_SWTRIG2
#define BUFF1					DAC_CR_BOFF1
#define BUFF2					DAC_CR_BOFF2

// Сброс регистров ЦАПа
void DAC_Reset(DAC_TypeDef *DACx);

// Разрешение тактирования ЦАПа
void DAC_ClkEnable(DAC_TypeDef *DACx);

// Конфигурация триггера ЦАПа
void DAC_TriggerConfigCh1(DAC_TypeDef *DACx, uint32_t TrigMask, uint32_t TrigEnableBit);
void DAC_TriggerConfigCh2(DAC_TypeDef *DACx, uint32_t TrigMask, uint32_t TrigEnableBit);

// Включение ЦАП
void DAC_EnableCh1(DAC_TypeDef *DACx);
void DAC_EnableCh2(DAC_TypeDef *DACx);

// Вкл/выкл выходного буффера
void DAC_BufferCh1(DAC_TypeDef *DACx, bool Enable);
void DAC_BufferCh2(DAC_TypeDef *DACx, bool Enable);

// Установка значения
void DAC_SetValueCh1(DAC_TypeDef *DACx, uint16_t Value);
void DAC_SetValueCh2(DAC_TypeDef *DACx, uint16_t Value);

// Форсирование программного триггера
void DAC_ForceSWTrigCh1(DAC_TypeDef *DACx);
void DAC_ForceSWTrigCh2(DAC_TypeDef *DACx);

// Разрешение работы модуля DMA
void DAC_DMAConfigCh1(DAC_TypeDef *DACx, bool DMAEnable, bool DMAUnderrunIntEnable);
void DAC_DMAConfigCh2(DAC_TypeDef *DACx, bool DMAEnable, bool DMAUnderrunIntEnable);

//############## Устаревшие функции для работы только с DAC1 ###################
void DACx_Reset(void);
void DACx_Enable(uint32_t DACx);
void DACx_Clk_Enable(uint32_t DACx);
void DACx_DMA_Config(uint32_t DMAEnable, uint32_t DMAUdInt);
void DAC_Wave_Config(uint32_t MAMPx_Config, uint32_t WAVEx_Config);
void DAC_Trigger_Config(uint32_t TriggerSelect, uint32_t TriggerEnable);
bool DAC_SoftTrigEnable(uint32_t SoftTrigXEnable);
void DAC_Buff(uint32_t BUFFx, bool EN);
void DAC_CH1_SetValue(uint16_t Value);
void DAC_CH2_SetValue(uint16_t Value);

#endif // __ZW_DAC_H
