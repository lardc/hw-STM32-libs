#ifndef __ZW_GPIO_H
#define __ZW_GPIO_H

// Includes
//
#include "stdinc.h"

// Definitions
//
//---------Pin----------
#define Pin_0  		0x00
#define Pin_1  		0x01
#define Pin_2  		0x02
#define Pin_3  		0x03
#define Pin_4  		0x04
#define Pin_5  		0x05
#define Pin_6  		0x06
#define Pin_7  		0x07
#define Pin_8  		0x08
#define Pin_9  		0x09
#define Pin_10 		0x0A
#define Pin_11 		0x0B
#define Pin_12 		0x0C
#define Pin_13 		0x0D
#define Pin_14 		0x0E
#define Pin_15		0x0F
//--------Mode----------
#define Input  		0x00
#define Output 		0x01
#define AltFn		0x02
#define Analog		0x03
//------OutType---------
#define PushPull	0x00
#define OpenDrain 	0x01
//-------Speed----------
#define LowSpeed 	0x00
#define MedSpeed 	0x01
#define HighSpeed 	0x03
//--------Pull----------
#define NoPull		0x00
#define Pull_Up		0x01
#define Pull_Down	0x02
//----------------------

//----Alternate func----
#define AltFn_0		0
#define AltFn_1		1
#define AltFn_2		2
#define AltFn_3		3
#define AltFn_4		4
#define AltFn_5		5
#define AltFn_6		6
#define AltFn_7		7
#define AltFn_8		8
#define AltFn_9		9
#define AltFn_10	10
#define AltFn_11	11
#define AltFn_12	12
#define AltFn_13	13
#define AltFn_14	14
#define AltFn_15	15
//----------------------

// Types
//
typedef struct __GPIO_PortPinSetting
{
	GPIO_TypeDef *Port;
	uint32_t Pin;
} GPIO_PortPinSetting;
#define GPIO_PortPinSettingMacro static const GPIO_PortPinSetting

// Functions
//
bool GPIO_GetState(GPIO_PortPinSetting PortPin);
void GPIO_SetState(GPIO_PortPinSetting PortPin, bool State);
void GPIO_Toggle(GPIO_PortPinSetting PortPin);
void GPIO_InitPushPullOutput(GPIO_PortPinSetting PortPin);
void GPIO_InitOpenDrainOutput(GPIO_PortPinSetting PortPin, uint32_t Pull);
void GPIO_InitAltFunction(GPIO_PortPinSetting PortPin, uint32_t AltFunc);
void GPIO_AltFn(GPIO_TypeDef * GPIOx, uint8_t Pin, uint8_t AltFn_Num);
void GPIO_InitInput(GPIO_PortPinSetting PortPin, uint32_t PullUpSetting);
void GPIO_InitAnalog(GPIO_PortPinSetting PortPin);

// Устаревшие функции
// Вместо GPIO_Config использовать функции серии GPIO_Init*
void GPIO_Config(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Mode, uint32_t OutType, uint32_t Speed, uint32_t Pull);
void GPIO_PinAction(GPIO_TypeDef* GPIOx, uint32_t Pin, bool Enable);	// Использовать GPIO_SetState
void GPIO_Bit_Set(GPIO_TypeDef* GPIOx, uint32_t Pin);					// Использовать GPIO_SetState
void GPIO_Bit_Rst(GPIO_TypeDef* GPIOx, uint32_t Pin);					// Использовать GPIO_SetState
bool GPIO_Read_Bit(GPIO_TypeDef* GPIOx, uint32_t Pin);					// Использовать GPIO_GetState
void GPIO_Bit_Toggle(GPIO_TypeDef* GPIOx, uint32_t Pin);				// Использовать GPIO_Toggle

#endif // __ZW_GPIO_H
