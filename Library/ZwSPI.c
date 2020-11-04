// Includes
//
#include "ZwSPI.h"

// Functions
//
void SPI_Init(SPI_TypeDef* SPIx, uint32_t BR_bits, bool LSBFirst)
{
	BR_bits &= 0x7;

	if (SPIx == SPI1) RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	if (SPIx == SPI2) RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	if (SPIx == SPI3) RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

	SPIx->CR1 |= (BR_bits << 3);
	SPIx->CR1 |= SPI_CR1_MSTR;
	SPIx->CR1 |= SPI_CR1_SSI;
	SPIx->CR1 |= SPI_CR1_SSM;
	if (LSBFirst) SPIx->CR1 |= SPI_CR1_LSBFIRST;
	SPIx->CR2 = 15 << 8;
	SPIx->CR1 |= SPI_CR1_SPE;
}
//-----------------------------------------------

void SPI_Init8b(SPI_TypeDef* SPIx, uint32_t BR_bits, bool LSBFirst)
{
	BR_bits &= 0x7;

	if (SPIx == SPI1) RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	if (SPIx == SPI2) RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	if (SPIx == SPI3) RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

	SPIx->CR1 |= (BR_bits << 3);
	SPIx->CR1 |= SPI_CR1_MSTR;
	SPIx->CR1 |= SPI_CR1_SSI;
	SPIx->CR1 |= SPI_CR1_SSM;
	if (LSBFirst) SPIx->CR1 |= SPI_CR1_LSBFIRST;
	SPIx->CR2 = 7 << 8;
	SPIx->CR2 |= SPI_CR2_FRXTH;
	SPIx->CR1 |= SPI_CR1_SPE;
}
//-----------------------------------------------

void SPI_WriteByte(SPI_TypeDef* SPIx, uint16_t Data)
{
	while (SPIx->SR & SPI_SR_BSY);
	SPIx->DR = Data;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	while (SPIx->SR & SPI_SR_BSY);
	while (!(SPIx->SR & SPI_SR_RXNE));
}
//-----------------------------------------------

void SPI_WriteByte8b(SPI_TypeDef* SPIx, uint8_t Data)
{
	while (SPIx->SR & SPI_SR_BSY);
	*(uint8_t *)(&SPIx->DR) = Data;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	while (SPIx->SR & SPI_SR_BSY);
	while (!(SPIx->SR & SPI_SR_RXNE));
}
//-----------------------------------------------

uint16_t SPI_ReadByte(SPI_TypeDef* SPIx)
{
	while (SPIx->SR & SPI_SR_BSY);
	SPIx->DR = 0;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	while (SPIx->SR & SPI_SR_BSY);
	while (!(SPIx->SR & SPI_SR_RXNE));

	return SPIx->DR;
}
//-----------------------------------------------

void SPI_InvertClockPolarity(SPI_TypeDef* SPIx, bool Invert)
{
	bool ClockInverted = SPIx->CR1 & SPI_CR1_CPOL;

	if((Invert && !ClockInverted) || (!Invert && ClockInverted))
	{
		SPI_Enable(SPIx, false);

		if(Invert)
			SPIx->CR1 |= SPI_CR1_CPOL;
		else
			SPIx->CR1 &= ~SPI_CR1_CPOL;

		SPI_Enable(SPIx, true);
	}
}
//-----------------------------------------------

void SPI_Enable(SPI_TypeDef* SPIx, bool Enable)
{
	if(Enable)
	{
		SPIx->CR1 |= SPI_CR1_SPE;
	}
	else
	{
		while (SPIx->SR & SPI_SR_FTLVL);
		while (SPIx->SR & SPI_SR_BSY);

		SPIx->CR1 &= ~SPI_CR1_SPE;

		while (SPIx->SR & SPI_SR_FRLVL)
		{
			if(SPIx->DR)
				asm("nop");
		}
	}
}
//-----------------------------------------------

//######################### Устаревшие функции #################################

void SPIx_Config(SPI_TypeDef* SPIx)
{
	if (SPIx == SPI1) RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	if (SPIx == SPI2) RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	if (SPIx == SPI3) RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	SPIx->CR1 = 0x0000;
	SPIx->CR2 = 15 << 8;
	SPIx->CR1 |= SPI_CR1_BR;
	SPIx->CR1 |= SPI_CR1_MSTR;
	SPIx->CR1 |= SPI_CR1_SSI;
	SPIx->CR1 |= SPI_CR1_SSM;
	// SPIx->CR1 |= SPI_CR1_LSBFIRST;
	SPIx->CR1 |= SPI_CR1_SPE;
}
//-----------------------------------------------

void SPI_SetSyncPolarity(SPI_TypeDef* SPIx, SPI_SyncPolarityEnum Polarity)
{
	while (SPIx->SR & SPI_SR_BSY);
	switch (Polarity)
	{
		case FALL_Edge:
			SPIx->CR1 &=~ SPI_CR1_CPOL;
			break;

		case RISE_Edge:
			SPIx->CR1 |= SPI_CR1_CPOL;
			break;
	}
}
//-----------------------------------------------

void SPI1_WriteByte(uint8_t Data)
{
#ifdef SPI1_CS_0
	SPI1_CS_1;
	SPI1_CS_0;
	SPI_WriteByte(SPI1, Data);
	SPI1_CS_1;
#endif
}
//-----------------------------------------------

void SPI2_WriteByte(uint8_t Data)
{
#ifdef SPI2_CS_0
	SPI2_CS_1;
	SPI2_CS_0;
	SPI_WriteByte(SPI2, Data);
	SPI2_CS_1;
#endif
}
//-----------------------------------------------

void SPI3_WriteByte(uint8_t Data)
{
#ifdef SPI3_CS_0
	SPI3_CS_1;
	SPI3_CS_0;
	SPI_WriteByte(SPI3, Data);
	SPI3_CS_1;
#endif
}
//-----------------------------------------------
