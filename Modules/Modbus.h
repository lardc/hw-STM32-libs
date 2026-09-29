#ifndef CONTROLLER_MODBUS_H_
#define CONTROLLER_MODBUS_H_

// Include
#include "stdinc.h"

// Types
//
typedef enum __ModbusError
{
	MODBUS_OK					= 0,
	MODBUS_ERR_TIMEOUT			= 1,
	MODBUS_ERR_FRAME_BREAK		= 2,
	MODBUS_ERR_BAD_CRC			= 3,
	MODBUS_ERR_SLAVE_EXCEPTION	= 4,
	MODBUS_ERR_WRONG_SLAVE		= 5,
	MODBUS_ERR_WRONG_FUNCTION	= 6,
	MODBUS_ERR_BUFFER_OVERFLOW	= 7,
	MODBUS_ERR_BAD_INIT			= 8
} ModbusError, *pModbusError;

typedef void (*ModbusFunc_SendByte)(Int16U Value);
typedef Int16U (*ModbusFunc_GetBytesToReceive)(void);
typedef Int16U (*ModbusFunc_ReceiveByte)(void);
typedef void (*ModbusFunc_SetTxMode)(Boolean State);

// Functions
//
void Modbus_Init(ModbusFunc_SendByte SendByte, ModbusFunc_GetBytesToReceive GetBytesToReceive,
		ModbusFunc_ReceiveByte ReceiveByte, ModbusFunc_SetTxMode SetTxMode,
		Int32U BaudRate, volatile Int64U *pTimeCounter, Int16U ResponseTimeoutTicks);

Int16U Modbus_BuildReadHoldingRegs(Int8U Slave, Int16U Address, Int16U Count, pInt8U Buffer);
Int16U Modbus_BuildWriteMultipleRegs(Int8U Slave, Int16U Address, Int16U Count, pInt16U Values, pInt8U Buffer);

ModbusError Modbus_SendReceive(pInt8U TxBuffer, Int16U TxLength, pInt8U RxBuffer, Int16U RxBufferSize, pInt16U RxLength);

#endif /* CONTROLLER_MODBUS_H_ */
