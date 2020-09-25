#ifndef __ZW_SCI_H
#define __ZW_SCI_H

// Include
#include "stdinc.h"

void USART_Init(USART_TypeDef * USARTx, uint32_t SYS_clk, uint32_t BaudRate);
void USART_Recieve_Interupt(USART_TypeDef * USARTx, uint32_t Priority, bool Enable);
void ZwSCI_SendArray16(pInt16U Buffer, Int16U BufferSize);
void ZwSCI_ReceiveArray16(pInt16U Buffer, Int16U BufferSize);
Int16U ZwSCI_GetBytesToReceive();
Int16U ZwSCI_ReceiveChar();
void ZwSCI_SendInt16(Int16U Value);
void ZwSCI_ReceiveInt16(pInt16U Value);
Boolean ZwSCIx_ReceiveChar(pInt16U Char);
void ZwSCI_SendChar(Int16U Value);
bool ZwSCI_RecieveCheck(USART_TypeDef* USARTx);
void ZwSCI_RecieveFlagClear(USART_TypeDef* USARTx);
void ZwSCI_RegisterToFIFO(USART_TypeDef* USARTx);

#endif // __ZW_SCI_H
