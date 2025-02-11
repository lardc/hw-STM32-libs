// -----------------------------------------
// CAN high-level interface functions
// ----------------------------------------

#ifndef __ZW_NCAN_H
#define __ZW_NCAN_H


// Include
//
#include "stdinc.h"
#include "BCCITypes.h"


// Совместимость TrueSTUDIO и IAR
#ifndef CAN1
	#define CAN1 CAN
#endif


// Functions
//
void NCAN_Init(uint32_t SystemFrequency, uint32_t Baudrate, bool AutoRetransmit);
void NCAN_FilterInit(uint16_t FilterNumber, uint32_t FilterExtID, uint32_t FilterExtIDMask);
void NCAN_FIFOInterrupt(bool Enable);
void NCAN_RecieveData();
bool NCAN_RecieveCheck();
void NCAN_RecieveFlagReset();
void NCAN_ConfigMailbox(Int16U mBox, Int32U MsgID, Int16U DataLength, bool IsReceiveMailbox, Int32U Mask);
void NCAN_SendMessage(Int16U mBox, pCANMessage Data);
void NCAN_SendMessageEx(Int16U mBox, pCANMessage Data, Boolean AlterMessageID, Boolean AlterMessageLength);
Boolean NCAN_IsMessageReceived(Int16U mBox, pBoolean pMessageLost);
void NCAN_GetMessage(Int16U mBox, pCANMessage Data);
void NCAN_InterruptSetPriority(uint16_t Priority);


#endif // __ZW_NCAN_H
