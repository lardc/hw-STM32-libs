// -----------------------------------------
// BCCI communication interface types
// ----------------------------------------

#ifndef __BCCI_TYPES_H
#define __BCCI_TYPES_H

// Include
#include "stdinc.h"
//
#include "xCCICommon.h"

// Definitions
//
#define CAN_ID_W_16					10
#define CAN_ID_W_32					14
#define CAN_ID_WB_16				30
#define CAN_ID_RB_16				40
#define CAN_ID_R_16					20
#define CAN_ID_R_32					24
#define CAN_ID_CALL					50
#define CAN_ID_ERR					61

// Types
//
// CAN message (64bit)
typedef struct __CANMessage
{
	union
	{
		Int32U DWORD_0;
		struct
		{
			Int16U WORD_1;
			Int16U WORD_0;
		} WORD;
	} HIGH;

	union
	{
		Int32U DWORD_1;
		struct
		{
			Int16U WORD_3;
			Int16U WORD_2;
		} WORD;
	} LOW;

	union
	{
		Int32U all;
		struct
		{
			Int16U MsgID_L;
			Int16U MsgID_H;
		} bit;
	} MsgID;
	Int16U DLC;
} CANMessage, *pCANMessage;
//
// Pointers for IO functions
typedef void (*BCCI_FUNC_SendMessage)(Int16U mBox, pCANMessage Data);
typedef void (*BCCI_FUNC_SendMessageEx)(Int16U mBox, pCANMessage Data, Boolean AlterMessageID, Boolean AlterMessageLength);
typedef void (*BCCI_FUNC_GetMessage)(Int16U mBox, pCANMessage Data);
typedef Boolean (*BCCI_FUNC_IsMessageReceived)(Int16U mBox, pBoolean pMessageLost);
typedef void (*BCCI_FUNC_ConfigMailbox)(Int16U mBox, Int32U MsgID, Int16U DataLen);
//
// IO configuration
typedef struct __BCCI_IOConfig
{
	BCCI_FUNC_SendMessage IO_SendMessage;
	BCCI_FUNC_SendMessageEx IO_SendMessageEx;
	BCCI_FUNC_GetMessage IO_GetMessage;
	BCCI_FUNC_IsMessageReceived IO_IsMessageReceived;
	BCCI_FUNC_ConfigMailbox IO_ConfigMailbox;
} BCCI_IOConfig, *pBCCI_IOConfig;
//
// BCCI instance state
typedef struct __BCCI_Interface
{
	pBCCI_IOConfig IOConfig;
	pxCCI_ServiceConfig ServiceConfig;
	pInt16U DataTableAddress;
	Int16U DataTableSize;
	void *ArgForEPCallback;
	xCCI_ProtectionAndEndpoints ProtectionAndEndpoints;
} BCCI_Interface, *pBCCI_Interface;


#endif // __BCCI_TYPES_H
