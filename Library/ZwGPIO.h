#ifndef __ZW_GPIO_H
#define __ZW_GPIO_H

// Includes
//
#include "stdinc.h"

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

void GPIO_Config(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Mode, uint32_t OutType, uint32_t Speed, uint32_t Pull);
void GPIO_Bit_Set(GPIO_TypeDef* GPIOx, uint32_t Pin);
void GPIO_Bit_Rst(GPIO_TypeDef* GPIOx, uint32_t Pin);
void GPIO_Bit_Toggle(GPIO_TypeDef* GPIOx, uint32_t Pin);
bool GPIO_Read_Bit(GPIO_TypeDef* GPIOx, uint32_t Pin);
void GPIO_PinAction(GPIO_TypeDef* GPIOx, uint32_t Pin, bool Enable);

#endif // __ZW_GPIO_H
