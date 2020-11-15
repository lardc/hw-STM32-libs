#include "ZwSCI.h"
#include "ZwRCC.h"
#include "SysConfig.h"

// Variables
//
static volatile uint8_t FIFO_RXcount = 0;		// Счетчик байтов, принятых по USART
static volatile uint8_t FIFO[USART_FIFOlen];	// USART FIFO

// Functions
//
void USART_Init(USART_TypeDef* USARTx, uint32_t SYS_clk, uint32_t BaudRate)
{
	RCC_USART_Clk_EN(USART_1_ClkEN);		// Включение тактирования UART1.

	USARTx->BRR = SYS_clk / BaudRate;		// Скорость
	USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE); // Разрешаем прием и передачу
	USARTx->CR1 |= USART_CR1_UE;			// Включение USART1
}
//-----------------------------------------

void USART_Recieve_Interupt(USART_TypeDef* USARTx, uint32_t Priority, bool Enable)
{
	IRQn_Type USARTXinterupt;

	if (USARTx == USART1) USARTXinterupt = USART1_IRQn;
	if (USARTx == USART2) USARTXinterupt = USART2_IRQn;
	if (USARTx == USART3) USARTXinterupt = USART3_IRQn;

	if (Enable)
	{
		USARTx->CR1 |= USART_CR1_RXNEIE;

		NVIC_SetPriority(USARTXinterupt, Priority);
		NVIC_EnableIRQ(USARTXinterupt);
	}
	else
	{
		USARTx->CR1 &= ~USART_CR1_RXNEIE;
		NVIC_DisableIRQ(USARTXinterupt);
	}
}
//-----------------------------------------

void ZwSCI_SendArray16(pInt16U Buffer, Int16U BufferSize)
{
	for (uint16_t i = 0; i < BufferSize; ++i)
		ZwSCI_SendInt16(Buffer[i]);
}
//-----------------------------------------

void ZwSCI_ReceiveArray16(pInt16U Buffer, Int16U BufferSize)
{
	for (uint16_t i = 0; i < BufferSize; ++i)
		ZwSCI_ReceiveInt16(&Buffer[i]);
}
//-----------------------------------------

Int16U ZwSCI_GetBytesToReceive()
{
	return FIFO_RXcount;
}
//-----------------------------------------

void ZwSCI_SendInt16(Int16U Value)
{
	ZwSCI_SendChar((uint8_t)(Value >> 8));
	ZwSCI_SendChar((uint8_t)(Value & 0x00FF));
}
//-----------------------------------------

void ZwSCI_ReceiveInt16(pInt16U Value)
{
	Int16U charH, charL;

	charH = ZwSCI_ReceiveChar();
	charL = ZwSCI_ReceiveChar();

	*Value = charL | (charH << 8);
}
//-----------------------------------------

Int16U ZwSCI_ReceiveChar()
{
	Int16U Char;
	while (!ZwSCIx_ReceiveChar(&Char));
	return Char;
}
//-----------------------------------------

Boolean ZwSCIx_ReceiveChar(pInt16U Char)
{
	if (FIFO_RXcount)
	{
		NVIC_DisableIRQ(USART1_IRQn);

		*Char = FIFO[0];
		for (uint8_t i = 1; i < FIFO_RXcount; ++i)
			FIFO[i - 1] = FIFO[i];
		--FIFO_RXcount;

		NVIC_EnableIRQ(USART1_IRQn);

		return TRUE;
	}
	else
		return FALSE;
}
//-----------------------------------------

void ZwSCI_SendChar(Int16U Value)
{
	USART1->TDR = (uint8_t)Value;
	while (!(USART1->ISR & USART_ISR_TC));
	USART1->ISR &= ~USART_ISR_TC;
}
//-----------------------------------------

bool ZwSCI_RecieveCheck(USART_TypeDef* USARTx)
{
	return (USARTx->ISR & USART_ISR_RXNE);
}
//-----------------------------------------

void ZwSCI_RecieveFlagClear(USART_TypeDef* USARTx)
{
	USARTx->ISR &= ~USART_ISR_RXNE;
	USARTx->ICR |= USART_ICR_ORECF;
}
//-----------------------------------------

void ZwSCI_RegisterToFIFO(USART_TypeDef* USARTx)
{
	if (FIFO_RXcount < USART_FIFOlen)
		FIFO[FIFO_RXcount++] = USARTx->RDR;
}
//-----------------------------------------
