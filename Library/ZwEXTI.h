#ifndef __ZW_EXTI_H
#define __ZW_EXTI_H

// Includes
//
#include "stdinc.h"

// Types
//
typedef struct __EXTI_PortChennelSetting
{
	uint8_t Port;
	uint32_t Channel;
} EXTI_PortChannelSetting;
#define EXTI_PortChannelSettingMacro static const EXTI_PortChannelSetting

// Definitions
//
#define RISE_TRIG	0x01
#define FALL_TRIG	0x02
#define BOTH_TRIG	0x03

#define EXTI_0		0
#define EXTI_1		1
#define EXTI_2		2
#define EXTI_3		3
#define EXTI_4		4
#define EXTI_5		5
#define EXTI_6		6
#define EXTI_7		7
#define EXTI_8		8
#define EXTI_9		9
#define EXTI_10		10
#define EXTI_11		11
#define EXTI_12		12
#define EXTI_13		13
#define EXTI_14		14
#define EXTI_15		15
#define EXTI_16		16
#define EXTI_17		17
#define EXTI_18		18
#define EXTI_19		19
#define EXTI_20		20
#define EXTI_21		21
#define EXTI_22		22
#define EXTI_23		23
#define EXTI_24		24
#define EXTI_25		25
#define EXTI_26		26
#define EXTI_27		27
#define EXTI_28		28
#define EXTI_29		29
#define EXTI_30		30
#define EXTI_31		31
#define EXTI_32		32
#define EXTI_33		33
#define EXTI_34		34
#define EXTI_35		35

#define EXTI_PA		0
#define EXTI_PB		1
#define EXTI_PC		2
#define EXTI_PD		3
#define EXTI_PE		4
#define EXTI_PF		5
#define EXTI_PG		6
#define EXTI_Inter	0

void EXTI_EnableInterrupt(IRQn_Type EXTI_IRQ, uint16_t Prioriry, bool Enable);
void EXTI_FlagReset(uint32_t EXTI_Channel);
bool EXTI_FlagCheck(uint32_t EXTI_Channel);
void EXTI_Init(EXTI_PortChannelSetting PortChannel, uint8_t TriggerType);

// Устаревшая функция инициализации
void EXTI_Config(uint32_t EXTI_PORT, uint32_t EXTI_Channel, uint8_t EXTI_Trigger, uint32_t Priority);

#endif // __ZW_EXTI_H
