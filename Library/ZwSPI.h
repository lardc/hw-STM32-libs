// ----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __ZW_SPI_H
#define __ZW_SPI_H

// Includes
//
#include "stdinc.h"
#include "SysConfig.h"
#include "ZwGPIO.h"

// Function
//
void SPI_Init(SPI_TypeDef* SPIx, uint32_t BR_bits, bool LSBFirst);
void SPI_Init8b(SPI_TypeDef* SPIx, uint32_t BR_bits, bool LSBFirst);
void SPI_WriteByte(SPI_TypeDef* SPIx, uint16_t Data);
void SPI_WriteByte8b(SPI_TypeDef* SPIx, uint8_t Data);
uint16_t SPI_ReadByte(SPI_TypeDef* SPIx);
void SPI_InvertClockPolarity(SPI_TypeDef* SPIx, bool Invert);
void SPI_Enable(SPI_TypeDef* SPIx, bool Enable);

//######################### Устаревшие функции #################################
typedef enum __SPI_SuncPolarity
{
	FALL_Edge = 0,
	RISE_Edge = 1
} SPI_SyncPolarityEnum;

void SPIx_Config(SPI_TypeDef* SPIx);
void SPI_SetSyncPolarity(SPI_TypeDef* SPIx, SPI_SyncPolarityEnum Polarity);

#endif // __ZW_SPI_H
