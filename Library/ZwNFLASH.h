// -----------------------------------------
// Global definitions
// ----------------------------------------

#ifndef __ZW_NFLASH_H
#define __ZW_NFLASH_H


// Includes
//
#include "stdinc.h"


void NFLASH_WriteArray16(uint32_t StartAddress, uint16_t* Buffer, uint32_t BufferSize);
void NFLASH_ReadArray16(uint32_t StartAddress, uint16_t* Buffer, uint32_t BufferSize);
void NFLASH_Unlock();
void NFLASH_ErasePages(uint32_t StartAddress, uint32_t StopAddress);
//
void NFLASH_WriteDT(uint32_t EPROMAddress, uint16_t* Buffer, uint16_t BufferSize);
void NFLASH_ReadDT(uint32_t EPROMAddress, uint16_t* Buffer, uint16_t BufferSize);


#endif // __ZW_NFLASH_H
