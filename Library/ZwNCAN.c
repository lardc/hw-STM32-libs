// Includes
//
#include "ZwNCAN.h"
#include "ZwRCC.h"
#include "BCCIxParams.h"


// Types
//
// Store information about mailbox
typedef struct __CANMailboxItem
{
	uint32_t	MsgID;
	bool		DataReady;
	uint16_t	DataLength;
	CANMessage	Message;
} CANMailboxItem;


// Definitions
//
#define MAILBOXmax		42		// Количество MailBox


// Variables
//
static volatile CANMailboxItem MailBox[MAILBOXmax] = {0};


// Forward fumctions
//
uint32_t NCAN_WordSwap(uint32_t Data);
void NCAN_SendMessageX(Int16U mBox, pCANMessage Data, Boolean AlterMessageID, Boolean AlterMessageLength);


// Functions
//
void NCAN_Init(uint32_t SystemFrequency, uint32_t Baudrate, bool AutoRetransmit)
{
	float BitTime, SystemClockTime, tq, tbs1, tbs2;
	uint32_t TS1, TS2, BRP;

	// Сброс модуля CAN
	RCC->APB1RSTR |= RCC_APB1RSTR_CAN1RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_CAN1RST;

	// Расчёт параметров работы
	SystemClockTime = 1.0f / SystemFrequency;
	BitTime = 1.0f / Baudrate;
	tq = BitTime / 10;
	tbs1 = tq * 7;
	tbs2 = tq * 2;
	//
	BRP = (uint32_t)(tq / SystemClockTime - 1);
	TS1 = (uint32_t)(tbs1 / tq - 1);
	TS2 = (uint32_t)(tbs2 / tq - 1);

    // Без автоматической ретрансляции, запрос инициализации
	CAN1->MCR = CAN_MCR_INRQ;
	if (AutoRetransmit)
		CAN1->MCR |= CAN_MCR_NART;

	// Настройка скорости CAN
	CAN1->BTR  = TS2 << 20; // 4 TS2
	CAN1->BTR |= TS1 << 16; // 9 TS1
	CAN1->BTR |= BRP;       // 3 BRP

	// Запуск модуля
	CAN1->BTR &= ~(CAN_BTR_SILM | CAN_BTR_LBKM);
	CAN1->MCR &= ~CAN_MCR_INRQ;         // Нормальный режим работы
	while (CAN1->MSR & CAN_MSR_INAK);   // Ожидание переключения

	// Ожидание готовности
	while (!(CAN1->TSR & CAN_TSR_TME0));
}
//-----------------------------------------------

void NCAN_FIFOInterrupt(bool Enable)
{
	if (Enable)
	{
		CAN1->IER |= CAN_IER_FMPIE0;
		NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	}
	else
	{
		CAN1->IER &= ~CAN_IER_FMPIE0;
		NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	}
}
//-----------------------------------------------

void NCAN_InterruptSetPriority(uint16_t Priority)
{
	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, (Priority & 0xF));
}
//-----------------------------------------------

void NCAN_FilterInit(uint16_t FilterNumber, uint32_t FilterExtID, uint32_t FilterExtIDMask)
{
	uint32_t FilterNumberBit = 0;

	if (FilterNumber > 13)
		return;
	else
		FilterNumberBit = 1 << FilterNumber;

	// Сдвиг старших бит ID, маски и добавление бита IDE
	FilterExtID		= ((FilterExtID		& 0x0003FFFF) << 3) | CAN_TI0R_IDE;
	FilterExtIDMask	= ((FilterExtIDMask	& 0x0003FFFF) << 3) | CAN_TI0R_IDE;

	CAN1->FMR |= CAN_FMR_FINIT;			// Разблокировать инициализацию фильтров
	CAN1->FA1R &= ~FilterNumberBit;		// Деактивация выбранного фильтра

	// Параметры фильтрации
	CAN1->FS1R |= FilterNumberBit;		// 32-битный режим регистра фильтров
	CAN1->FM1R &= ~FilterNumberBit;		// Режим ID/Mask
	CAN1->FFA1R &= ~FilterNumberBit;	// Сообщение сохраняется в FIFO0

	CAN1->sFilterRegister[FilterNumber].FR1 = FilterExtID;
	CAN1->sFilterRegister[FilterNumber].FR2 = FilterExtIDMask;

	CAN1->FA1R |= FilterNumberBit;		// Активировать выбранный фильтр
	CAN1->FMR &= ~CAN_FMR_FINIT;    	// Заблокировать инициализацию фильтров
}
//-----------------------------------------------

void NCAN_RecieveData()
{
	uint32_t i, MsgID;
	CANMessage InputMessage;

	MsgID = CAN1->sFIFOMailBox[0].RIR >> 3;

	// Поиск мэйлбокса
	for (i = 0; i < MAILBOXmax; ++i)
	{
		if ((MsgID & (CAN_SLAVE_NID_MASK | CAN_FUNC_MASK)) == MailBox[i].MsgID ||
			(MsgID & (CAN_MASTER_NID_MASK | CAN_FUNC_MASK)) == MailBox[i].MsgID)
		{
			InputMessage.HIGH.DWORD_0 = NCAN_WordSwap(CAN1->sFIFOMailBox[0].RDLR);
			InputMessage.LOW.DWORD_1  = NCAN_WordSwap(CAN1->sFIFOMailBox[0].RDHR);
			InputMessage.MsgID.all	  = MsgID;
			InputMessage.DLC		  = CAN1->sFIFOMailBox[0].RDTR & 0xF;
			//
			MailBox[i].DataReady = TRUE;
			MailBox[i].Message = InputMessage;
			break;
		}
	}
}
//-----------------------------------------------

void NCAN_ConfigMailbox(Int16U mBox, Int32U MsgID, Int16U DataLength)
{
	MailBox[mBox].MsgID = MsgID;
	MailBox[mBox].DataLength = DataLength;
}
//-----------------------------------------------

void NCAN_SendMessageX(Int16U mBox, pCANMessage Data, Boolean AlterMessageID, Boolean AlterMessageLength)
{
	uint32_t NewMsgID = Data->MsgID.all;

	if (!AlterMessageID)
		NewMsgID &= CAN_MASTER_NID_MASK;

	NewMsgID |= MailBox[mBox].MsgID;

	CAN1->sTxMailBox[0].TIR = (NewMsgID << 3) | CAN_TI0R_IDE;
	CAN1->sTxMailBox[0].TDTR = AlterMessageLength ? Data->DLC : MailBox[mBox].DataLength;

	CAN1->sTxMailBox[0].TDLR = NCAN_WordSwap(Data->HIGH.DWORD_0);
	CAN1->sTxMailBox[0].TDHR = NCAN_WordSwap(Data->LOW.DWORD_1);

	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	while (!(CAN1->TSR & CAN_TSR_TXOK0))
	{
		if(CAN1->TSR & CAN_TSR_TERR0)
			break;
	}
}
//-----------------------------------------------

void NCAN_SendMessage(Int16U mBox, pCANMessage Data)
{
	NCAN_SendMessageX(mBox, Data, FALSE, FALSE);
}
//-----------------------------------------------

void NCAN_SendMessageEx(Int16U mBox, pCANMessage Data, Boolean AlterMessageID, Boolean AlterMessageLength)
{
	NCAN_SendMessageX(mBox, Data, AlterMessageID, AlterMessageLength);
}
//-----------------------------------------------

Boolean NCAN_IsMessageReceived(Int16U mBox, pBoolean pMessageLost)
{
	return MailBox[mBox].DataReady;
}
//-----------------------------------------------

void NCAN_GetMessage(Int16U mBox, pCANMessage Data)
{
	if (Data) *Data = MailBox[mBox].Message;
	MailBox[mBox].DataReady = FALSE;
}
//-----------------------------------------------

void NCAN_RecieveFlagReset()
{
	CAN1->RF0R |= CAN_RF0R_RFOM0;
}
//-----------------------------------------------

bool NCAN_RecieveCheck()
{
	return (CAN1->RF0R & CAN_RF0R_FMP0);
}
//-----------------------------------------------

uint32_t NCAN_WordSwap(uint32_t Data)
{
	uint32_t result;

	result  = (Data & 0x000000ff) << 24;
	result |= (Data & 0x0000ff00) << 8;
	result |= (Data & 0x00ff0000) >> 8;
	result |= (Data & 0xff000000) >> 24;

	return result;
}
//-----------------------------------------------
