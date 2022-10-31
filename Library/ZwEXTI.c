// Includes
//
#include "ZwEXTI.h"
#include "ZwGPIO.h"


// Функция для инициализации внешних прерываний
void EXTI_Init(EXTI_PortChannelSetting PortChannel, uint8_t TriggerType)
{
	EXTI_Config(PortChannel.Port, PortChannel.Channel, TriggerType, 0);

	switch(PortChannel.Channel)
	{
	case EXTI_0:
		EXTI_EnableInterrupt(EXTI0_IRQn, 0, true);
		break;

	case EXTI_1:
		EXTI_EnableInterrupt(EXTI1_IRQn, 0, true);
		break;

	case EXTI_2:
		EXTI_EnableInterrupt(EXTI2_TSC_IRQn, 0, true);
		break;

	case EXTI_3:
		EXTI_EnableInterrupt(EXTI3_IRQn, 0, true);
		break;

	case EXTI_4:
		EXTI_EnableInterrupt(EXTI4_IRQn, 0, true);
		break;

	case EXTI_5:
	case EXTI_6:
	case EXTI_7:
	case EXTI_8:
	case EXTI_9:
		EXTI_EnableInterrupt(EXTI9_5_IRQn, 0, true);
		break;

	case EXTI_10:
	case EXTI_11:
	case EXTI_12:
	case EXTI_13:
	case EXTI_14:
	case EXTI_15:
		EXTI_EnableInterrupt(EXTI15_10_IRQn, 0, true);
		break;
	}
}
// -----------------------------------------------------------------------------

// Настройка контроллера прерываний
void EXTI_Config(uint32_t EXTI_PORT, uint32_t EXTI_Channel, uint8_t EXTI_Trigger, uint32_t Priority)
{
	if (EXTI_Channel <= 15)
	{
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		SYSCFG->EXTICR[EXTI_Channel >> 2] = (SYSCFG->EXTICR[EXTI_Channel >> 2] & ~(0xF << ((EXTI_Channel & 0x3) << 2)));
		SYSCFG->EXTICR[EXTI_Channel >> 2] |= EXTI_PORT << ((EXTI_Channel & 0x3) << 2); // Указание источника прерывания
	}

	if (EXTI_Channel < 31)
	{
		EXTI->IMR |= 1 << EXTI_Channel;					// Разрешение прерывания
		EXTI->EMR |= 1 << EXTI_Channel;					// Разрешение события

		if (EXTI_Trigger == BOTH_TRIG)					// Прерыванием при обоих фронтах сигнала
		{
			EXTI->FTSR |= 1 << EXTI_Channel;
			EXTI->RTSR |= 1 << EXTI_Channel;
		}
		else if (EXTI_Trigger == RISE_TRIG)				// Прерывание при положительном фронте
		{
			EXTI->RTSR |= 1 << EXTI_Channel;
			EXTI->FTSR &= (uint32_t)~(1 << EXTI_Channel);
		}
		else if (EXTI_Trigger == FALL_TRIG)				// Прерывание при отрицательном фронте
		{
			EXTI->FTSR |= 1 << EXTI_Channel;
			EXTI->RTSR &= (uint32_t) ~(1 << EXTI_Channel);
		}
	}
	else
	{
		EXTI->IMR2 |= 1 << (EXTI_Channel - 32);			// Разрешение прерывания
		EXTI->EMR2 |= 1 << (EXTI_Channel - 32);			// Разрешение события

		if (EXTI_Trigger == BOTH_TRIG)					// Прерыванием при обоих фронтах сигнала
		{
			EXTI->FTSR2 |= 1 << (EXTI_Channel - 32);
			EXTI->RTSR2 |= 1 << (EXTI_Channel - 32);
		}
		else if (EXTI_Trigger == RISE_TRIG)				//Прерывание при положительном фронте
		{
			EXTI->RTSR2 |= 1 << (EXTI_Channel - 32);
			EXTI->FTSR2 &= (uint32_t) ~(1 << (EXTI_Channel - 32));
		}
		else if (EXTI_Trigger == FALL_TRIG)				// Прерывание при отрицательном фронте
		{
			EXTI->FTSR2 |= 1 << (EXTI_Channel - 32);
			EXTI->RTSR2 &= (uint32_t) ~(1 << (EXTI_Channel - 32));
		}
	}
}
// -----------------------------------------------------------------------------

// Разрешение прерывания от EXTI
void EXTI_EnableInterrupt(IRQn_Type EXTI_IRQ, uint16_t Prioriry, bool Enable)
{
	// Изменение приоритета по умолчанию не требуется
	//NVIC_SetPriority(EXTI_IRQ, Prioriry);

	Enable ? NVIC_EnableIRQ(EXTI_IRQ) : NVIC_DisableIRQ(EXTI_IRQ);
}
//------------------------------------------------------------------------------

// Сброс флага прерывания
void EXTI_FlagReset(uint32_t EXTI_Channel)
{
	if (EXTI_Channel <= 31)
		EXTI->PR |= 1 << EXTI_Channel;
	else
		EXTI->PR2 |= 1 << (EXTI_Channel - 32);
}
//------------------------------------------------------------------------------

// Проверка флага прерывания
bool EXTI_FlagCheck(uint32_t EXTI_Channel)
{
	if (EXTI_Channel <= 31)
	{
		return (EXTI->PR & (1 << EXTI_Channel));
	}
	else
	{
		return (EXTI->PR2 & (1 << (EXTI_Channel - 32)));
	}
}
//------------------------------------------------------------------------------
