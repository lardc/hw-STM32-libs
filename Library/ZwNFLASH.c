// Include
//
#include "ZwNFLASH.h"


// Definitions
//
#define FLASH_PAGE_SIZE 0x800


// Forward functions
//
uint16_t NFLASH_ReadWord16(uint32_t Address);


// Functions
//
void NFLASH_WriteArray16(uint32_t StartAddress, uint16_t* Buffer, uint32_t BufferSize)
{
	// Enable flash access
	FLASH->CR |= FLASH_CR_PG;

	for (uint32_t i = 0; i < BufferSize; ++i, StartAddress += 2)
	{
		*(__IO uint16_t*)StartAddress = (Buffer[i] >> 8) | (Buffer[i] << 8);
		while (FLASH->SR & FLASH_SR_BSY);
	}

	// Disable flash access
	FLASH->CR &= ~FLASH_CR_PG;
}
// -----------------------------------------------

// Read half-word array
void NFLASH_ReadArray16(uint32_t StartAddress, uint16_t* Buffer, uint32_t BufferSize)
{
	for (uint32_t i = 0; i < BufferSize; ++i, StartAddress += 2)
		Buffer[i] = NFLASH_ReadWord16(StartAddress);
}
// -----------------------------------------------

// Read single half-word
uint16_t NFLASH_ReadWord16(uint32_t Address)
{
	uint16_t data = *(__IO uint16_t*)Address;
	return (data >> 8) | (data << 8);
}
// -----------------------------------------------

// Unblock flash write operations
void NFLASH_Unlock()
{
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}
// -----------------------------------------------

// Erase page
void NFLASH_ErasePages(uint32_t StartAddress, uint32_t StopAddress)
{
	// Enable flash access
	FLASH->CR |= FLASH_CR_PER;

	for (uint32_t i = StartAddress; i < StopAddress; i += FLASH_PAGE_SIZE)
	{
		FLASH->AR = i;
		FLASH->CR |= FLASH_CR_STRT;
		while (FLASH->SR & FLASH_SR_BSY);
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	// Disable flash access
	FLASH->CR &= ~FLASH_CR_PER;
}
// -----------------------------------------------

void NFLASH_WriteDT(uint32_t EPROMAddress, uint16_t* Buffer, uint16_t BufferSize)
{
	// Prepare flash
	NFLASH_Unlock();
	NFLASH_ErasePages(EPROMAddress, EPROMAddress + FLASH_PAGE_SIZE);

	// Write data
	NFLASH_WriteArray16(EPROMAddress, Buffer, BufferSize);
}
// -----------------------------------------------

void NFLASH_ReadDT(uint32_t EPROMAddress, uint16_t* Buffer, uint16_t BufferSize)
{
	NFLASH_ReadArray16(EPROMAddress, Buffer, BufferSize);
}
// -----------------------------------------------
