// Header
#include "ZwUSART.h"

// Includes
#include "ZwRCC.h"
#include "SysConfig.h"

// Variables
//
static volatile uint8_t USART1_ReceiveCount = 0;
static volatile uint8_t USART2_ReceiveCount = 0;
static volatile uint8_t USART3_ReceiveCount = 0;

static volatile uint8_t USART1_ReceiveFIFO[USART_FIFOlen];
static volatile uint8_t USART2_ReceiveFIFO[USART_FIFOlen];
static volatile uint8_t USART3_ReceiveFIFO[USART_FIFOlen];

// Forward functions
//
void USARTx_AttachArrays(USART_TypeDef* USARTx, volatile uint8_t **FIFO, volatile uint8_t **Counter);

// Functions
//
void USARTx_Init(USART_TypeDef* USARTx, uint32_t SYS_clk, uint32_t BaudRate)
{
	if(USARTx == USART1)
		RCC_USART_Clk_EN(USART_1_ClkEN);
	else if(USARTx == USART2)
		RCC_USART_Clk_EN(USART_2_ClkEN);
	else if(USARTx == USART3)
		RCC_USART_Clk_EN(USART_3_ClkEN);
	
	// Параметры тактирования
	USARTx->BRR = SYS_clk / BaudRate;
	// Разрешение приёма и передачи
	USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;
	// Включение
	USARTx->CR1 |= USART_CR1_UE;
}
//-----------------------------------------

void USARTx_RecieveInterrupt(USART_TypeDef* USARTx, bool Enable)
{
	IRQn_Type USARTXinterupt = USART1_IRQn;

	if(USARTx == USART2)
		USARTXinterupt = USART2_IRQn;
	else if(USARTx == USART3)
		USARTXinterupt = USART3_IRQn;
	
	if(Enable)
	{
		USARTx->CR1 |= USART_CR1_RXNEIE;
		NVIC_EnableIRQ(USARTXinterupt);
	}
	else
	{
		USARTx->CR1 &= ~USART_CR1_RXNEIE;
		NVIC_DisableIRQ(USARTXinterupt);
	}
}
//-----------------------------------------

void USARTx_SendArray16(USART_TypeDef* USARTx, uint16_t *Buffer, uint16_t BufferSize)
{
	for(uint16_t i = 0; i < BufferSize; ++i)
		USARTx_SendWord16(USARTx, Buffer[i]);
}
//-----------------------------------------

void USART1_SendArray16(uint16_t *Buffer, uint16_t BufferSize)
{
	USARTx_SendArray16(USART1, Buffer, BufferSize);
}
//-----------------------------------------

void USART2_SendArray16(uint16_t *Buffer, uint16_t BufferSize)
{
	USARTx_SendArray16(USART2, Buffer, BufferSize);
}
//-----------------------------------------

void USART3_SendArray16(uint16_t *Buffer, uint16_t BufferSize)
{
	USARTx_SendArray16(USART3, Buffer, BufferSize);
}
//-----------------------------------------

void USARTx_ReceiveArray16(USART_TypeDef* USARTx, uint16_t *Buffer, uint16_t BufferSize)
{
	for(uint16_t i = 0; i < BufferSize; ++i)
		USARTx_ReceiveWord16(USARTx, &Buffer[i]);
}
//-----------------------------------------

void USART1_ReceiveArray16(uint16_t *Buffer, uint16_t BufferSize)
{
	USARTx_ReceiveArray16(USART1, Buffer, BufferSize);
}
//-----------------------------------------

void USART2_ReceiveArray16(uint16_t *Buffer, uint16_t BufferSize)
{
	USARTx_ReceiveArray16(USART2, Buffer, BufferSize);
}
//-----------------------------------------

void USART3_ReceiveArray16(uint16_t *Buffer, uint16_t BufferSize)
{
	USARTx_ReceiveArray16(USART3, Buffer, BufferSize);
}
//-----------------------------------------

uint16_t USARTx_GetBytesToReceive(USART_TypeDef* USARTx)
{
	volatile uint8_t *FIFO, *Counter;
	USARTx_AttachArrays(USARTx, &FIFO, &Counter);
	return (*Counter);
}
//-----------------------------------------

uint16_t USART1_GetBytesToReceive()
{
	return USARTx_GetBytesToReceive(USART1);
}
//-----------------------------------------

uint16_t USART2_GetBytesToReceive()
{
	return USARTx_GetBytesToReceive(USART2);
}
//-----------------------------------------

uint16_t USART3_GetBytesToReceive()
{
	return USARTx_GetBytesToReceive(USART3);
}
//-----------------------------------------

void USARTx_SendWord16(USART_TypeDef* USARTx, uint16_t Value)
{
	USARTx_SendChar(USARTx, Value >> 8);
	USARTx_SendChar(USARTx, Value & 0x00FF);
}
//-----------------------------------------

void USARTx_ReceiveWord16(USART_TypeDef* USARTx, uint16_t *Value)
{
	uint16_t charH, charL;
	
	charH = USARTx_ReceiveChar(USARTx);
	charL = USARTx_ReceiveChar(USARTx);
	
	*Value = charL | (charH << 8);
}
//-----------------------------------------

uint16_t USARTx_ReceiveChar(USART_TypeDef* USARTx)
{
	volatile uint8_t *FIFO, *Counter;
	uint16_t Result = 0;
	USARTx_AttachArrays(USARTx, &FIFO, &Counter);

	while((*Counter) == 0);

	USARTx_RecieveInterrupt(USARTx, false);
	Result = FIFO[0];
	for(uint8_t i = 1; i < (*Counter); ++i)
		FIFO[i - 1] = FIFO[i];
	--(*Counter);
	USARTx_RecieveInterrupt(USARTx, true);

	return Result;
}
//-----------------------------------------

uint16_t USART1_ReceiveChar()
{
	return USARTx_ReceiveChar(USART1);
}
//-----------------------------------------

uint16_t USART2_ReceiveChar()
{
	return USARTx_ReceiveChar(USART2);
}
//-----------------------------------------

uint16_t USART3_ReceiveChar()
{
	return USARTx_ReceiveChar(USART3);
}
//-----------------------------------------

void USARTx_SendChar(USART_TypeDef* USARTx, uint16_t Value)
{
	USARTx->TDR = (uint8_t)Value;
	while(!(USARTx->ISR & USART_ISR_TC));
	USARTx->ISR &= ~USART_ISR_TC;
}
//-----------------------------------------

void USART1_SendChar(uint16_t Value)
{
	USARTx_SendChar(USART1, Value);
}
//-----------------------------------------

void USART2_SendChar(uint16_t Value)
{
	USARTx_SendChar(USART2, Value);
}
//-----------------------------------------

void USART3_SendChar(uint16_t Value)
{
	USARTx_SendChar(USART3, Value);
}
//-----------------------------------------

bool USARTx_RecieveCheck(USART_TypeDef* USARTx)
{
	return USARTx->ISR & USART_ISR_RXNE;
}
//-----------------------------------------

void USARTx_RecieveFlagClear(USART_TypeDef* USARTx)
{
	// Регистр ISR только для чтения
	// очистка флага полученных данных производится при считываниии из регистра данных

	// USARTx->ISR &= ~USART_ISR_RXNE;
}
//-----------------------------------------

void USARTx_OverrunFlagClear(USART_TypeDef* USARTx)
{
	if(USARTx->ISR & USART_ISR_ORE)
		USARTx->ICR |= USART_ICR_ORECF;
}
//-----------------------------------------

void USARTx_RegisterToFIFO(USART_TypeDef* USARTx)
{
	volatile uint8_t *FIFO, *Counter;
	USARTx_AttachArrays(USARTx, &FIFO, &Counter);

	if((*Counter) < USART_FIFOlen)
		FIFO[(*Counter)++] = USARTx->RDR;
}
//-----------------------------------------

void USARTx_AttachArrays(USART_TypeDef* USARTx, volatile uint8_t **FIFO, volatile uint8_t **Counter)
{
	if(USARTx == USART1)
	{
		*FIFO = USART1_ReceiveFIFO;
		*Counter = &USART1_ReceiveCount;
	}
	else if(USARTx == USART2)
	{
		*FIFO = USART2_ReceiveFIFO;
		*Counter = &USART2_ReceiveCount;
	}
	else if(USARTx == USART3)
	{
		*FIFO = USART3_ReceiveFIFO;
		*Counter = &USART3_ReceiveCount;
	}
}
//-----------------------------------------
