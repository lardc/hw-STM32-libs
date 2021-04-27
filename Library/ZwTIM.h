﻿#ifndef __ZW_TIM_H
#define __ZW_TIM_H

// Includes
//
#include "stdinc.h"

#define MMS_RESET               ~TIM_CR2_MMS
#define MMS_ENABLE              TIM_CR2_MMS_0
#define MMS_UPDATE              TIM_CR2_MMS_1
#define DMAEN					TIM_DIER_UDE
//
#define TIMx_CHANNEL1			1
#define TIMx_CHANNEL2			2
#define TIMx_CHANNEL3			3
#define TIMx_CHANNEL4			4

void TIM_Config(TIM_TypeDef * TIMx, float SystemClock, float Period);
void TIM_Interupt(TIM_TypeDef * TIMx, uint8_t Priority, bool EN);
void TIM_Reset(TIM_TypeDef * TIMx);
void TIM_Stop(TIM_TypeDef * TIMx);
void TIM_Start(TIM_TypeDef * TIMx);
bool TIM_StatusCheck(TIM_TypeDef * TIMx);
void TIM_StatusClear(TIM_TypeDef * TIMx);
void TIM_MasterMode(TIM_TypeDef * TIMx, uint32_t MMS);
void TIM_DMA(TIM_TypeDef * TIMx, uint32_t DMA_MODE);
void TIM_Clock_En(uint32_t TIM_x);
void TIM15_16_17_PWM_CH1_Config(TIM_TypeDef* TIMx, float SystemClock, float Period);
void TIM15_16_17_PWM_CH1_SetValue(TIM_TypeDef* TIMx, uint16_t PWM_Value);
void TIM_OnePulseMode(TIM_TypeDef* TIMx, bool State);
void TIMx_PWM_SetValue(TIM_TypeDef* TIMx, uint32_t Channel, uint32_t Value);
void TIMx_PWM_ConfigChannel(TIM_TypeDef* TIMx, uint32_t Channel);
void TIM_InterruptEventConfig(TIM_TypeDef* TIMx, uint32_t Event, bool Enable);

#endif // __ZW_TIM_H
