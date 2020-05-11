// Includes
//
#include "ZwGPIO.h"

// Настройка порта
void GPIO_Config(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Mode, uint32_t OutType, uint32_t Speed, uint32_t Pull)
{
	// Настройка порта на вход или выход
	GPIOx->MODER = (GPIOx->MODER & (~((uint32_t)0x3 << (Pin << 1)))) | (Mode << (Pin << 1));
	
	// PushPull, OpenDrain
	GPIOx->OTYPER = (GPIOx->OTYPER & (~((uint32_t)0x1 << Pin))) | (OutType << Pin);
	
	// Настройка скорости тактирования порта
	GPIOx->OSPEEDR = (GPIOx->OSPEEDR & (~((uint32_t)0x1 << (Pin << 1)))) | (Speed << (Pin << 1));
	
	// No Pull, Pull-Up, Pull-Down
	GPIOx->PUPDR = (GPIOx->PUPDR & (~((uint32_t)0x1 << (Pin << 1)))) | (Pull << (Pin << 1));
}
//-----------------------------------------------

// Установки бита порта в 1
void GPIO_Bit_Set(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
	GPIOx->ODR |= ((uint32_t)0x1 << Pin);
}
//-----------------------------------------------

// Сброс бита порта в 0
void GPIO_Bit_Rst(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
	GPIOx->ODR &= ~((uint32_t)0x1 << Pin);
}
//-----------------------------------------------

// Изменение текущего состояния выхода на противоположное
void GPIO_Bit_Toggle(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
	if (GPIOx->ODR & ((uint32_t)0x1 << Pin))
		GPIOx->ODR &= ~((uint32_t)0x1 << Pin);
	else
		GPIOx->ODR |= ((uint32_t)0x1 << Pin);
}
//-----------------------------------------------

// Чтение бита порта
bool GPIO_Read_Bit(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
	return (bool)(GPIOx->IDR & ((uint32_t)0x1 << Pin));
}
//-----------------------------------------------

// Операция над выходным пином
void GPIO_PinAction(GPIO_TypeDef* GPIOx, uint32_t Pin, bool Enable)
{
  	Enable ? GPIO_Bit_Set(GPIOx, Pin) : GPIO_Bit_Rst(GPIOx, Pin);
}
//-----------------------------------------------
