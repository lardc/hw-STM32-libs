// Includes
//
#include "ZwRCC.h"
#include "ZwDMA.h"

// Разрешение тактирования DMA
void DMA_Clk_Enable(uint32_t DMA_ClockEnable)
{
	RCC_DMA_Clk_EN(DMA_ClockEnable);
}
//-----------------------------------------------

// Сброс параметров DMA
void DMA_Reset(DMA_Channel_TypeDef* DMA_ChannelX)
{
	DMA_ChannelX->CCR = 0;
}
//-----------------------------------------------

// Прерывание DMA
void DMA_Interrupt(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t Interrupt, uint8_t Priority, bool Enable)
{
	IRQn_Type Channelx;

	// DMA1
	if (DMA_ChannelX == DMA1_Channel1) {Channelx = DMA1_Channel1_IRQn;}
	if (DMA_ChannelX == DMA1_Channel2) {Channelx = DMA1_Channel2_IRQn;}
	if (DMA_ChannelX == DMA1_Channel3) {Channelx = DMA1_Channel3_IRQn;}
	if (DMA_ChannelX == DMA1_Channel4) {Channelx = DMA1_Channel4_IRQn;}
	if (DMA_ChannelX == DMA1_Channel5) {Channelx = DMA1_Channel5_IRQn;}
	if (DMA_ChannelX == DMA1_Channel6) {Channelx = DMA1_Channel6_IRQn;}
	if (DMA_ChannelX == DMA1_Channel7) {Channelx = DMA1_Channel7_IRQn;}

	// DMA2
	if (DMA_ChannelX == DMA2_Channel1) {Channelx = DMA2_Channel1_IRQn;}
	if (DMA_ChannelX == DMA2_Channel2) {Channelx = DMA2_Channel2_IRQn;}
	if (DMA_ChannelX == DMA2_Channel3) {Channelx = DMA2_Channel3_IRQn;}
	if (DMA_ChannelX == DMA2_Channel4) {Channelx = DMA2_Channel4_IRQn;}
	if (DMA_ChannelX == DMA2_Channel5) {Channelx = DMA2_Channel5_IRQn;}

	if (Enable)
	{
		DMA_ChannelX->CCR |= Interrupt;
		// Изменение приоритета по умолчанию не требуется
		//NVIC_SetPriority(Channelx, Priority);
		NVIC_EnableIRQ(Channelx);
	}
	else
	{
		DMA_ChannelX->CCR &= ~Interrupt;
		NVIC_DisableIRQ(Channelx);
	}
}
//-----------------------------------------------

// Настройка DMA
void DMAChannelX_Config(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t M2M, uint32_t PriorityLvl,
	uint32_t MemorySize, uint32_t PeriphSize, uint32_t MemoryIncerment, uint32_t PeriphIncrement,
	uint32_t CircularMode, uint32_t DataDirect)
{
	DMA_ChannelX->CCR |= (M2M | PriorityLvl | MemorySize | PeriphSize | MemoryIncerment
		| PeriphIncrement | CircularMode | DataDirect);
}
//-----------------------------------------------

// Источник и приемник данных
void DMAChannelX_DataConfig(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t MemAddr, uint32_t PeripheralAddr, uint16_t Number)
{
	DMA_ChannelX->CMAR = MemAddr;
	DMA_ChannelX->CPAR = PeripheralAddr;
	DMA_ChannelX->CNDTR = Number;
}
//-----------------------------------------------

// Чтение количества данных
uint16_t DMA_ReadDataCount(DMA_Channel_TypeDef* DMA_ChannelX)
{
	return DMA_ChannelX->CNDTR;
}
//-----------------------------------------------

// Проверка окончания транзакции
bool DMA_IsTransferComplete(DMA_TypeDef* DMA, uint32_t Channel)
{
	if (DMA->ISR & Channel)
		return true;
	else
		return false;
}
//-----------------------------------------------

// Сброс флага окончания транзакции
void DMA_TransferCompleteReset(DMA_TypeDef* DMA, uint32_t Channel)
{
	DMA->IFCR |= Channel;
}
//-----------------------------------------------

// Активация - деактивация канала
void DMA_ChannelEnable(DMA_Channel_TypeDef* DMA_ChannelX, bool Enable)
{
	if (Enable)
		DMA_ChannelX->CCR |= DMA_CCR_EN;
	else
		DMA_ChannelX->CCR &= ~DMA_CCR_EN;
}
//-----------------------------------------------

// Перезагрузка канала для новой транзакции
void DMA_ChannelReload(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t DataSize)
{
	DMA_ChannelEnable(DMA_ChannelX, false);
	DMA_ChannelX->CNDTR = DataSize;
}
//-----------------------------------------------

//############## Устаревшие функции для работы только с DMA1 ###################

// Настройка DMA
void DMA1ChannelX_Config(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t M2M, uint32_t PriorityLvl,
	uint32_t MemorySize, uint32_t PeriphSize, uint32_t MemoryIncerment, uint32_t PeriphIncrement,
	uint32_t CircularMode, uint32_t DataDirect, uint32_t ChannelEnable)
{
	DMA_ChannelX->CCR |= (M2M | PriorityLvl | MemorySize | PeriphSize | MemoryIncerment
		| PeriphIncrement | CircularMode | DataDirect | ChannelEnable);
}
//-----------------------------------------------

// Источник и приемник данных
void DMA1ChannelX_DataConfig(
	DMA_Channel_TypeDef* DMA_ChannelX, uint32_t Source, uint32_t Destination, uint16_t Number)
{
	DMA_ChannelX->CNDTR = Number;
	DMA_ChannelX->CMAR = Source;
	DMA_ChannelX->CPAR = Destination;
}
//-----------------------------------------------

// Чтение количества данных
uint32_t DMA1ReadDataCount(DMA_Channel_TypeDef* DMA_ChannelX)
{
	return DMA_ChannelX->CNDTR;
}
//-----------------------------------------------

// Проверка окончания транзакции
bool DMA_TransferCompleteCheck(uint32_t Channel)
{
	if (DMA1->ISR & Channel)
		return true;
	else
		return false;
}
//-----------------------------------------------

// Сброс флага окончания транзакции
void DMA_TransferCompleteFlagReset(uint32_t Channel)
{
	DMA1->IFCR |= Channel;
}
//-----------------------------------------------
