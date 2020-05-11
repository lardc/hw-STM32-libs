#ifndef __ZW_DMA_H
#define __ZW_DMA_H

// Includes
//
#include "stdinc.h"
#include "ZwRCC.h"

#define DMA_TRANSFER_ERROR			DMA_CCR_TEIE
#define DMA_HALF_TRANSFER			DMA_CCR_HTIE
#define DMA_TRANSFER_COMPLETE		DMA_CCR_TCIE
#define DMA_MEM2MEM_EN				DMA_CCR_MEM2MEM
#define DMA_MEM2MEM_DIS				0
#define DMA_LvlPriority_LOW			0
#define DMA_LvlPriority_MEDIUM		DMA_CCR_PL_0
#define DMA_LvlPriority_HIGHT		DMA_CCR_PL_1
#define DMA_LvlPriority_VHIGHT		DMA_CCR_PL
#define DMA_MSIZE_8BIT				0
#define DMA_MSIZE_16BIT				DMA_CCR_MSIZE_0
#define DMA_MSIZE_32BIT				DMA_CCR_MSIZE_1
#define DMA_PSIZE_8BIT				0
#define DMA_PSIZE_16BIT				DMA_CCR_PSIZE_0
#define DMA_PSIZE_32BIT				DMA_CCR_PSIZE_1
#define DMA_MINC_EN					DMA_CCR_MINC
#define DMA_MINC_DIS				0
#define DMA_PINC_EN					DMA_CCR_PINC
#define DMA_PINC_DIS				0
#define DMA_CIRCMODE_EN				DMA_CCR_CIRC
#define DMA_CIRCMODE_DIS			0
#define DMA_READ_FROM_MEM			DMA_CCR_DIR
#define DMA_READ_FROM_PERIPH		0
#define DMA_CHANNEL_EN				DMA_CCR_EN

void DMA_Clk_Enable(uint32_t DMA_ClockEnable);
void DMA_Reset(DMA_Channel_TypeDef * DMA_ChannelX);
void DMA_Interrupt(DMA_Channel_TypeDef * DMA_ChannelX,uint32_t Interrupt,uint8_t Priority, bool Enable);
void DMAChannelX_Config(DMA_Channel_TypeDef * DMA_ChannelX,uint32_t M2M,uint32_t PriorityLvl,uint32_t MemorySize,
						uint32_t PeriphSize,uint32_t MemoryIncerment,uint32_t PeriphIncrement,uint32_t CircularMode,uint32_t DataDirect,uint32_t ChannelEnable);
void DMAChannelX_DataConfig(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t MemAddr, uint32_t PeripheralAddr, uint16_t Number);
bool DMA_IsTransferComplete(DMA_TypeDef* DMA, uint32_t Channel);
void DMA_TransferCompleteReset(DMA_TypeDef* DMA, uint32_t Channel);
uint16_t DMA_ReadDataCount(DMA_Channel_TypeDef * DMA_ChannelX);
void DMA_ChannelEnable(DMA_Channel_TypeDef* DMA_ChannelX, bool Enable);
void DMA_ChannelReload(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t DataSize);

//############## Устаревшие функции для работы только с DMA1 ###################

void DMA1ChannelX_Config(DMA_Channel_TypeDef * DMA1_ChannelX,uint32_t M2M,uint32_t PriorityLvl,uint32_t MemorySize,
						uint32_t PeriphSize,uint32_t MemoryIncerment,uint32_t PeriphIncrement,uint32_t CircularMode,uint32_t DataDirect,uint32_t ChannelEnable);
void DMA1ChannelX_DataConfig(DMA_Channel_TypeDef * DMA1_ChannelX, uint32_t Source, uint32_t Destination, uint16_t Number);
uint32_t DMA1ReadDataCount(DMA_Channel_TypeDef * DMA1_ChannelX);
void DMA_TransferCompleteFlagReset(uint32_t Channel);
bool DMA_TransferCompleteCheck(uint32_t Channel);

#endif // __ZW_DMA_H
