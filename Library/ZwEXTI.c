// Includes
//
#include "ZwEXTI.h"


// ��������� ����������� ����������
void EXTI_Config(uint32_t EXTI_PORT, uint32_t EXTI_Channel, uint8_t EXTI_Trigger, uint32_t Priority)
{
	if (EXTI_Channel <= 15)
	{
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		SYSCFG->EXTICR[EXTI_Channel >> 2] = (SYSCFG->EXTICR[EXTI_Channel >> 2] & ~(0xF << ((EXTI_Channel & 0x3) << 2)));
		SYSCFG->EXTICR[EXTI_Channel >> 2] |= EXTI_PORT << ((EXTI_Channel & 0x3) << 2); // �������� ��������� ����������
	}

	if (EXTI_Channel < 31)
	{
		EXTI->IMR |= 1 << EXTI_Channel;					// ���������� ����������
		EXTI->EMR |= 1 << EXTI_Channel;					// ���������� �������

		if (EXTI_Trigger == BOTH_TRIG)					// ����������� ��� ����� ������� �������
		{
			EXTI->FTSR |= 1 << EXTI_Channel;
			EXTI->RTSR |= 1 << EXTI_Channel;
		}
		else if (EXTI_Trigger == RISE_TRIG)				// ���������� ��� ������������� ������
		{
			EXTI->RTSR |= 1 << EXTI_Channel;
		}
		else if (EXTI_Trigger == FALL_TRIG)				// ���������� ��� ������������� ������
		{
			EXTI->FTSR |= 1 << EXTI_Channel;
		}
	}
	else
	{
		EXTI->IMR2 |= 1 << (EXTI_Channel - 32);			// ���������� ����������
		EXTI->EMR2 |= 1 << (EXTI_Channel - 32);			// ���������� �������

		if (EXTI_Trigger == BOTH_TRIG)					// ����������� ��� ����� ������� �������
		{
			EXTI->FTSR2 |= 1 << (EXTI_Channel - 32);
			EXTI->RTSR2 |= 1 << (EXTI_Channel - 32);
		}
		else if (EXTI_Trigger == RISE_TRIG)				//���������� ��� ������������� ������
		{
			EXTI->RTSR2 |= 1 << (EXTI_Channel - 32);
		}
		else if (EXTI_Trigger == FALL_TRIG)				// ���������� ��� ������������� ������
		{
			EXTI->FTSR2 |= 1 << (EXTI_Channel - 32);
		}
	}
}
// -----------------------------------------------------------------------------

// ���������� ���������� �� EXTI
void EXTI_EnableInterrupt(IRQn_Type EXTI_IRQ, uint16_t Prioriry, bool Enable)
{
	// ��������� ���������� �� ��������� �� ���������
	//NVIC_SetPriority(EXTI_IRQ, Prioriry);

	Enable ? NVIC_EnableIRQ(EXTI_IRQ) : NVIC_DisableIRQ(EXTI_IRQ);
}
//------------------------------------------------------------------------------

// ����� ����� ����������
void EXTI_FlagReset(uint32_t EXTI_Channel)
{
	if (EXTI_Channel <= 31)
		EXTI->PR |= 1 << EXTI_Channel;
	else
		EXTI->PR2 |= 1 << (EXTI_Channel - 32);
}
//------------------------------------------------------------------------------

// �������� ����� ����������
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
