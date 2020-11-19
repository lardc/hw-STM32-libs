#ifndef __ZW_USART_H
#define __ZW_USART_H

// Include
#include "stdinc.h"

void USARTx_Init(USART_TypeDef* USARTx, uint32_t SYS_clk, uint32_t BaudRate);
void USARTx_RecieveInterrupt(USART_TypeDef* USARTx, bool Enable);

void USARTx_SendArray16(USART_TypeDef* USARTx, uint16_t *Buffer, uint16_t BufferSize);
void USART1_SendArray16(uint16_t *Buffer, uint16_t BufferSize);
void USART2_SendArray16(uint16_t *Buffer, uint16_t BufferSize);
void USART3_SendArray16(uint16_t *Buffer, uint16_t BufferSize);

void USARTx_ReceiveArray16(USART_TypeDef* USARTx, uint16_t *Buffer, uint16_t BufferSize);
void USART1_ReceiveArray16(uint16_t *Buffer, uint16_t BufferSize);
void USART2_ReceiveArray16(uint16_t *Buffer, uint16_t BufferSize);
void USART3_ReceiveArray16(uint16_t *Buffer, uint16_t BufferSize);

uint16_t USARTx_GetBytesToReceive(USART_TypeDef* USARTx);
uint16_t USART1_GetBytesToReceive();
uint16_t USART2_GetBytesToReceive();
uint16_t USART3_GetBytesToReceive();

void USARTx_SendWord16(USART_TypeDef* USARTx, uint16_t Value);
void USARTx_ReceiveWord16(USART_TypeDef* USARTx, uint16_t *Value);

uint16_t USARTx_ReceiveChar(USART_TypeDef* USARTx);
uint16_t USART1_ReceiveChar();
uint16_t USART2_ReceiveChar();
uint16_t USART3_ReceiveChar();

void USARTx_SendChar(USART_TypeDef* USARTx, uint16_t Value);
void USART1_SendChar(uint16_t Value);
void USART2_SendChar(uint16_t Value);
void USART3_SendChar(uint16_t Value);

bool USARTx_RecieveCheck(USART_TypeDef* USARTx);
void USARTx_RecieveFlagClear(USART_TypeDef* USARTx);
void USARTx_OverrunFlagClear(USART_TypeDef* USARTx);
void USARTx_RegisterToFIFO(USART_TypeDef* USARTx);


#endif // __ZW_USART_H
