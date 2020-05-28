// -----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __ZW_SPI_H
#define __ZW_SPI_H

// Includes
//
#include "stdinc.h"
#include "SysConfig.h"
#include "ZwGPIO.h"

void SPI_Init(SPI_TypeDef* SPIx, uint32_t BR_bits, bool LSBFirst);
void SPI_WriteByte(SPI_TypeDef* SPIx, uint16_t Data);
uint16_t SPI_ReadByte(SPI_TypeDef* SPIx);

//######################### ���������� ������� #################################
void SPIx_Config(SPI_TypeDef* SPIx);

#endif // __ZW_SPI_H
