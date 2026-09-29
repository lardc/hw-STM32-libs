#include "TRM101.h"

// Include
#include "OWENProtocol.h"
#include "SysConfig.h"
#include "Controller.h"
#include "LowLevel.h"
#include "ZwUSART.h"

static Boolean TRM_ReadChar(pInt16U Char, Int64U StartTime)
{
	while(CONTROL_TimeCounter - StartTime <= TRM_TIMEOUT_TICKS)
	{
		if(USART2_GetBytesToReceive())
		{
			*Char = USART2_ReceiveChar();
			return TRUE;
		}
	}

	return FALSE;
}
// ----------------------------------------

static void TRM_SendBuffer(pInt16U Buffer, Int16U BufferSize)
{
	Int16U i;

	LL_RS485_SetTxMode(true);
	for(i = 0; i < BufferSize; i++)
		USART2_SendChar(Buffer[i] & 0xFF);
	LL_RS485_SetTxMode(false);
}
// ----------------------------------------

void TRM_DataExchange(Int16U Address, Int16U Hash, Boolean Request, pInt16U DataIn, Int16U DataInSize, pInt16U DataOut, pInt16U DataOutSize, pTRMError error)
{
	Int16U DataCounter, i;
	Int16U RawBuffer[OWNP_MAX_FRAME_SIZE], ASCIIBuffer[OWNP_MAX_ASCII_FRAME_SIZE];
	OWENProtocol_Frame frameOut, frameIn;
	Int64U startTime = CONTROL_TimeCounter;
	Int16U Char = 0;

	// Compose frame
	frameOut.Address = Address;
	frameOut.AddressLength = 8;
	frameOut.Hash = Hash;
	frameOut.Request = Request ? 1 : 0;
	frameOut.DataSize = Request ? 0 : DataInSize;
	if(frameOut.DataSize)
		MemCopy16(DataIn, frameOut.Data, frameOut.DataSize);

	// Pack frame
	DataCounter = OWENProtocol_FramePack(&frameOut, RawBuffer);

	// Convert to ASCII and send
	DataCounter = OWENProtocol_FrameToASCII(RawBuffer, DataCounter, ASCIIBuffer);
	TRM_SendBuffer(ASCIIBuffer, DataCounter);

	// Recieve data
	do
	{
		if(!TRM_ReadChar(&Char, startTime))
		{
			*error = TRME_ResponseTimeout;
			return;
		}
	}
	while(Char != 0x23);

	DataCounter = 0;
	ASCIIBuffer[DataCounter++] = Char;
	do
	{
		if(!TRM_ReadChar(&Char, startTime))
		{
			*error = TRME_ResponseTimeout;
			return;
		}

		ASCIIBuffer[DataCounter++] = Char;
	}
	while(Char != 0x0D && DataCounter < OWNP_MAX_ASCII_FRAME_SIZE);

	if(Char != 0x0D)
	{
		*error = TRME_InputBufferOverrun;
		return;
	}

	// Convert ASCII data to frame
	DataCounter = OWENProtocol_ASCIIToFrame(ASCIIBuffer, DataCounter, RawBuffer);
	OWENProtocol_FrameUnPack(RawBuffer, DataCounter, &frameIn);

	// Validate input data
	if(frameIn.CRC_OK == 0)
	{
		*error = TRME_CheckSumError;
		return;
	}
	else if(frameOut.Request == 0 && frameOut.Checksum != frameIn.Checksum)
	{
		*error = TRME_CheckSumError;
		return;
	}
	else if(frameOut.Request)
	{
		if(frameOut.Address != frameIn.Address ||
			frameOut.AddressLength != frameIn.AddressLength ||
			frameOut.Hash != frameIn.Hash ||
			frameIn.Request ||
			frameIn.DataSize == 0 || frameIn.DataSize > OWPNP_MAX_DATA_BYTES)
		{
			*error = TRME_WrongResponse;
			return;
		}
	}

	// Write output data
	if (frameOut.Request)
	{
		for (i = 0; i < frameIn.DataSize; i++)
			DataOut[i] = frameIn.Data[i];

		*DataOutSize = frameIn.DataSize;
	}
	*error = TRME_None;
}
// ----------------------------------------

float TRM_UnpackToFloat(pInt16U buf)
{
	union
	{
		Int32U u;
		float f;
	} value;

	value.u = ((Int32U)(buf[0] & 0xFF) << 24)
			| ((Int32U)(buf[1] & 0xFF) << 16)
			| ((Int32U)(buf[2] & 0xFF) << 8);

	return value.f;
}
// ----------------------------------------

void TRM_PackFromFloat(float value, pInt16U out3)
{
	union
	{
		Int32U u;
		float f;
	} packed;

	packed.f = value;

	out3[0] = (Int16U)(packed.u >> 24);
	out3[1] = (Int16U)((packed.u >> 16) & 0xFF);
	out3[2] = (Int16U)((packed.u >> 8) & 0xFF);
}
// ----------------------------------------

Int16U TRM_ReadF24(Int16U Address, Int16U Hash, pTRMError error)
{
	Int16U Data[OWPNP_MAX_DATA_BYTES], DataCounter;

	TRM_DataExchange(Address, Hash, TRUE, NULL, 0, Data, &DataCounter, error);
	return (DataCounter == 3) ? TRM_UnpackToFloat(Data) : 0;
}
// ----------------------------------------

void TRM_WriteF24(Int16U Address, Int16U Hash, float Value, pTRMError error)
{
	Int16U Data[OWPNP_MAX_DATA_BYTES], DataCounter, DataOut[3];

	TRM_PackFromFloat(Value, DataOut);
	TRM_DataExchange(Address, Hash, FALSE, DataOut, 3, Data, &DataCounter, error);
}
// ----------------------------------------

void TRM_Command(Int16U Address, Int16U Hash, Boolean Start, pTRMError error)
{
	Int16U Data[OWPNP_MAX_DATA_BYTES], DataCounter;
	Int16U CMD = Start ? 1 : 0;

	TRM_DataExchange(Address, Hash, FALSE, &CMD, 1, Data, &DataCounter, error);
}
// ----------------------------------------

float TRM_ReadTemp(Int16U Address, pTRMError error)
{
	return TRM_ReadF24(Address, 0xB8DF, error);
}
// ----------------------------------------

float TRM_ReadPower(Int16U Address, pTRMError error)
{
	return TRM_ReadF24(Address, 0x35E8, error);
}
// ----------------------------------------

void TRM_SetTemp(Int16U Address, float Temperature, pTRMError error)
{
	TRM_WriteF24(Address, 0x9107, Temperature, error);
}
// ----------------------------------------

void TRM_Start(Int16U Address, pTRMError error)
{
	TRM_Command(Address, 0xAF90, TRUE, error);
}
// ----------------------------------------

void TRM_Stop(Int16U Address, pTRMError error)
{
	TRM_Command(Address, 0xAF90, FALSE, error);
}
// ----------------------------------------
