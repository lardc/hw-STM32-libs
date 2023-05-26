// ----------------------------------------
// Device profile
// ----------------------------------------

// Header
#include "DeviceProfile.h"
//
// Includes
#include "BoardConfig.h"
#include "SysConfig.h"
#include "Global.h"
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "Controller.h"
#include "Constraints.h"

// Types
//
typedef struct __EPState
{
	Int16U Size;
	Int16U ReadCounter;
	Int16U LastReadCounter;
	pInt16U pDataCounter;
	pInt16U Data;
} EPState, *pEPState;
//
typedef struct __EPStates
{
	EPState ReadEPs[EP_READ_COUNT];
	EPState WriteEPs[EP_WRITE_COUNT];
} EPStates, *pEPStates;


// Variables
//
SCCI_Interface DEVICE_RS232_Interface;
BCCI_Interface DEVICE_CAN_Interface;
//
static SCCI_IOConfig RS232_IOConfig;
static BCCI_IOConfig CAN_IOConfig;
static xCCI_ServiceConfig X_ServiceConfig;
static xCCI_FUNC_CallbackAction ControllerDispatchFunction;
static EPStates RS232_EPState, CAN_EPState;
static Boolean UnlockedForNVWrite = FALSE;
//
static volatile Boolean *MaskChangesFlag;


// Forward functions
//
static void DEVPROFILE_FillWRPartDefault();
static Boolean DEVPROFILE_Validate16(Int16U Address, Int16U Data);
static Boolean DEVPROFILE_DispatchAction(Int16U ActionID, pInt16U UserError);
static Int16U DEVPROFILE_CallbackReadX(Int16U Endpoint, pInt16U *Buffer, Boolean Streamed,
									   Boolean RepeatLastTransmission, void *EPStateAddress, Int16U MaxNonStreamSize);
static Boolean DEVPROFILE_CallbackWriteX(Int16U Endpoint, pInt16U Buffer, Boolean Streamed, Int16U Length, void *EPStateAddress);


// Functions
//
void DEVPROFILE_Init(xCCI_FUNC_CallbackAction SpecializedDispatch, volatile Boolean *MaskChanges)
{
	// Save values
	ControllerDispatchFunction = SpecializedDispatch;
	MaskChangesFlag = MaskChanges;

	// Init interface
	RS232_IOConfig.IO_SendArray16 = &ZwSCI_SendArray16;
	RS232_IOConfig.IO_ReceiveArray16 = &ZwSCI_ReceiveArray16;
	RS232_IOConfig.IO_GetBytesToReceive = &ZwSCI_GetBytesToReceive;
	RS232_IOConfig.IO_ReceiveByte = &ZwSCI_ReceiveChar;
	CAN_IOConfig.IO_SendMessage = &NCAN_SendMessage;
	CAN_IOConfig.IO_SendMessageEx = &NCAN_SendMessageEx;
	CAN_IOConfig.IO_GetMessage = &NCAN_GetMessage;
	CAN_IOConfig.IO_IsMessageReceived = &NCAN_IsMessageReceived;
	CAN_IOConfig.IO_ConfigMailbox = &NCAN_ConfigMailbox;

	// Init service
	X_ServiceConfig.UserActionCallback = &DEVPROFILE_DispatchAction;
	X_ServiceConfig.ValidateCallback16 = &DEVPROFILE_Validate16;

	// Init interface driver
	SCCI_Init(&DEVICE_RS232_Interface, &RS232_IOConfig, &X_ServiceConfig, (pInt16U)DataTable,
			  DATA_TABLE_SIZE, SCCI_TIMEOUT_TICKS, &RS232_EPState);
	BCCI_Init(&DEVICE_CAN_Interface, &CAN_IOConfig, &X_ServiceConfig, (pInt16U)DataTable,
			  DATA_TABLE_SIZE, &CAN_EPState);

	// Set write protection
	SCCI_AddProtectedArea(&DEVICE_RS232_Interface, DATA_TABLE_WP_START, DATA_TABLE_SIZE - 1);
	BCCI_AddProtectedArea(&DEVICE_CAN_Interface, DATA_TABLE_WP_START, DATA_TABLE_SIZE - 1);
}
// ----------------------------------------

void DEVPROFILE_InitEPReadService(pInt16U Indexes, pInt16U Sizes, pInt16U *Counters, pInt16U *Datas)
{
	Int16U i;

	for(i = 0; i < EP_READ_COUNT; ++i)
	{
		RS232_EPState.ReadEPs[i].Size = Sizes[i];
		RS232_EPState.ReadEPs[i].pDataCounter = Counters[i];
		RS232_EPState.ReadEPs[i].Data = Datas[i];

		CAN_EPState.ReadEPs[i].Size = Sizes[i];
		CAN_EPState.ReadEPs[i].pDataCounter = Counters[i];
		CAN_EPState.ReadEPs[i].Data = Datas[i];

		RS232_EPState.ReadEPs[i].ReadCounter = RS232_EPState.ReadEPs[i].LastReadCounter = 0;
		CAN_EPState.ReadEPs[i].ReadCounter = CAN_EPState.ReadEPs[i].LastReadCounter = 0;

		SCCI_RegisterReadEndpoint16(&DEVICE_RS232_Interface, Indexes[i], &DEVPROFILE_CallbackReadX);
		BCCI_RegisterReadEndpoint16(&DEVICE_CAN_Interface, Indexes[i], &DEVPROFILE_CallbackReadX);
	}
}
// ----------------------------------------

void DEVPROFILE_InitEPWriteService(pInt16U Indexes, pInt16U Sizes, pInt16U *Counters, pInt16U *Datas)
{
	Int16U i;

	for(i = 0; i < EP_WRITE_COUNT; ++i)
	{
		RS232_EPState.WriteEPs[i].Size = Sizes[i];
		RS232_EPState.WriteEPs[i].pDataCounter = Counters[i];
		RS232_EPState.WriteEPs[i].Data = Datas[i];

		CAN_EPState.WriteEPs[i].Size = Sizes[i];
		CAN_EPState.WriteEPs[i].pDataCounter = Counters[i];
		CAN_EPState.WriteEPs[i].Data = Datas[i];

		SCCI_RegisterWriteEndpoint16(&DEVICE_RS232_Interface, Indexes[i], &DEVPROFILE_CallbackWriteX);
		BCCI_RegisterWriteEndpoint16(&DEVICE_CAN_Interface, Indexes[i], &DEVPROFILE_CallbackWriteX);
	}
}
// ----------------------------------------

void DEVPROFILE_ProcessRequests()
{
	// Handle interface requests
	SCCI_Process(&DEVICE_RS232_Interface, CONTROL_TimeCounter, *MaskChangesFlag);
	// Handle interface requests
	BCCI_Process(&DEVICE_CAN_Interface, *MaskChangesFlag);
}
// ----------------------------------------

void DEVPROFILE_ResetEPReadState()
{
	Int16U i;

	for(i = 0; i < EP_READ_COUNT; ++i)
	{
		RS232_EPState.ReadEPs[i].ReadCounter = 0;
		CAN_EPState.ReadEPs[i].ReadCounter = 0;
		RS232_EPState.ReadEPs[i].LastReadCounter = 0;
		CAN_EPState.ReadEPs[i].LastReadCounter = 0;
	}
}
// ----------------------------------------

void DEVPROFILE_ResetControlSection()
{
	DT_ResetWRPart(&DEVPROFILE_FillWRPartDefault);
}
// ----------------------------------------

void DEVPROFILE_ResetReadScopes(Int16U ResetPosition, Int16U ScopeMask)
{
	Int16U i;

	for(i = 0; i < EP_READ_COUNT; ++i)
	{
		if((1 << i) & ScopeMask)
		{
			*(RS232_EPState.ReadEPs[i].pDataCounter) = ResetPosition;
			*(CAN_EPState.ReadEPs[i].pDataCounter) = ResetPosition;

			MemZero16(RS232_EPState.ReadEPs[i].Data, RS232_EPState.ReadEPs[i].Size);
			MemZero16(CAN_EPState.ReadEPs[i].Data, CAN_EPState.ReadEPs[i].Size);
		}
	}
}
// ----------------------------------------

void DEVPROFILE_ResetWriteScopes(Int16U ResetPosition, Int16U ScopeMask)
{
	Int16U i;

	for(i = 0; i < EP_WRITE_COUNT; ++i)
	{
		if((1 << i) & ScopeMask)
		{
			*(RS232_EPState.WriteEPs[i].pDataCounter) = ResetPosition;
			*(CAN_EPState.WriteEPs[i].pDataCounter) = ResetPosition;

			MemZero16(RS232_EPState.WriteEPs[i].Data, RS232_EPState.WriteEPs[i].Size);
			MemZero16(CAN_EPState.WriteEPs[i].Data, CAN_EPState.WriteEPs[i].Size);
		}
	}
}
// ----------------------------------------

static void DEVPROFILE_FillWRPartDefault()
{
	Int16U i;

	// Write default values to data table
	for(i = 0; i < (DATA_TABLE_WP_START - DATA_TABLE_WR_START); ++i)
		DataTable[DATA_TABLE_WR_START + i] = VConstraint[i].Default;
}
// ----------------------------------------

static Boolean DEVPROFILE_Validate16(Int16U Address, Int16U Data)
{
	if(ENABLE_LOCKING && !UnlockedForNVWrite && (Address < DATA_TABLE_WR_START))
		return FALSE;

	if(Address < DATA_TABLE_WR_START)
	{
		if(NVConstraint[Address - DATA_TABLE_NV_START].Min > NVConstraint[Address - DATA_TABLE_NV_START].Max)
		{
			if(((Int16S)Data) < ((Int16S)(NVConstraint[Address - DATA_TABLE_NV_START].Min)) || ((Int16S)Data) > ((Int16S)(NVConstraint[Address - DATA_TABLE_NV_START].Max)))
					return FALSE;
		}
		else if(Data < NVConstraint[Address - DATA_TABLE_NV_START].Min || Data > NVConstraint[Address - DATA_TABLE_NV_START].Max)
			return FALSE;
	}
	else if(Address < DATA_TABLE_WP_START)
	{
		if(VConstraint[Address - DATA_TABLE_WR_START].Min > VConstraint[Address - DATA_TABLE_WR_START].Max)
		{
			if(((Int16S)Data) < ((Int16S)(VConstraint[Address - DATA_TABLE_WR_START].Min)) || ((Int16S)Data) > ((Int16S)(VConstraint[Address - DATA_TABLE_WR_START].Max)))
					return FALSE;
		}
		else if(Data < VConstraint[Address - DATA_TABLE_WR_START].Min || Data > VConstraint[Address - DATA_TABLE_WR_START].Max)
			return FALSE;
	}

	return TRUE;
}
// ----------------------------------------

static Boolean DEVPROFILE_DispatchAction(Int16U ActionID, pInt16U UserError)
{
	return (ControllerDispatchFunction) ? ControllerDispatchFunction(ActionID, UserError) : FALSE;
}
// ----------------------------------------

static Int16U DEVPROFILE_CallbackReadX(Int16U Endpoint, pInt16U *Buffer, Boolean Streamed,
									   Boolean RepeatLastTransmission, void *EPStateAddress, Int16U MaxNonStreamSize)
{
	Int16U pLen;
	pEPState epState;
	pEPStates epStates = (pEPStates)EPStateAddress;

	// Validate pointer
	if(!epStates)
		return 0;

	// Get endpoint
	epState = &epStates->ReadEPs[Endpoint - 1];

	// Handle transmission repeat
	if(RepeatLastTransmission)
		epState->ReadCounter = epState->LastReadCounter;

	// Write possible content reference
	*Buffer = epState->Data + epState->ReadCounter;

	// Calculate content length
	if(*(epState->pDataCounter) < epState->ReadCounter)
		pLen = 0;
	else
		pLen = *(epState->pDataCounter) - epState->ReadCounter;

	if(!Streamed)
		pLen = (pLen > MaxNonStreamSize) ? MaxNonStreamSize : pLen;

	// Update content state
	epState->LastReadCounter = epState->ReadCounter;
	epState->ReadCounter += pLen;

	return pLen;
}
// ----------------------------------------

static Boolean DEVPROFILE_CallbackWriteX(Int16U Endpoint, pInt16U Buffer, Boolean Streamed, Int16U Length, void *EPStateAddress)
{
	pEPState epState;
	pEPStates epStates = (pEPStates)EPStateAddress;

	// Validate pointer
	if(!epStates)
		return FALSE;

	// Get endpoint
	epState = &epStates->WriteEPs[Endpoint - 1];

	// Check for free space
	if (epState->Size < Length + *(epState->pDataCounter))
		return FALSE;
	else
	{
		MemCopy16(Buffer, epState->Data + *(epState->pDataCounter), Length);
		*(epState->pDataCounter) += Length;
		return TRUE;
	}
}
// ----------------------------------------


