// ----------------------------------------
// Common declarations for xCCI interfaces
// ----------------------------------------

#ifndef __xCCI_COMMON_H
#define __xCCI_COMMON_H

// Include
#include "stdinc.h"
#include "xCCIParams.h"

// Pointers to callback functions
typedef Boolean (*xCCI_FUNC_CallbackAction)(Int16U ActionID, pInt16U UserError);
typedef Boolean (*xCCI_FUNC_CallbackValidate16)(Int16U Address, Int16U Data);
typedef Boolean (*xCCI_FUNC_CallbackValidateFloat)(Int16U Address, float Data);
typedef Int16U (*xCCI_FUNC_CallbackReadEndpoint16)(Int16U Endpoint, pInt16U *Buffer, Boolean Streamed, Boolean RepeatLastTransmission, void *UserArgument, Int16U MaxNonStreamSize);
typedef Boolean (*xCCI_FUNC_CallbackWriteEndpoint16)(Int16U Endpoint, pInt16U Buffer, Boolean Streamed, Int16U Length, void *UserArgument);
typedef Int16U (*xCCI_FUNC_CallbackReadEndpointFloat)(Int16U Endpoint, float** Buffer, void *UserArgument);

// Service configuration
typedef struct __xCCI_ServiceConfig
{
	xCCI_FUNC_CallbackAction UserActionCallback;
	xCCI_FUNC_CallbackValidate16 ValidateCallback16;
	xCCI_FUNC_CallbackValidateFloat ValidateCallbackFloat;
} xCCI_ServiceConfig, *pxCCI_ServiceConfig;
//
// Protected area data
typedef struct __xCCI_ProtectedArea
{
	Int16U StartAddress;
	Int16U EndAddress;
} xCCI_ProtectedArea;
//
typedef struct __xCCI_ProtectionAndEndpoints
{
	Int16U ProtectedAreasUsed;
	xCCI_ProtectedArea ProtectedAreas[xCCI_MAX_PROTECTED_AREAS];
	xCCI_FUNC_CallbackReadEndpoint16 ReadEndpoints16[xCCI_MAX_READ_ENDPOINTS + 1];
	xCCI_FUNC_CallbackWriteEndpoint16 WriteEndpoints16[xCCI_MAX_WRITE_ENDPOINTS + 1];
	xCCI_FUNC_CallbackReadEndpointFloat ReadEndpointsFloat[xCCI_MAX_READ_ENDPOINTS + 1];
} xCCI_ProtectionAndEndpoints, *pxCCI_ProtectionAndEndpoints;

// Constants
//
enum xCCI_ErrorCodes
{
	ERR_NO_ERROR			= 0,
	ERR_TIMEOUT				= 1,
	ERR_CRC					= 2,
	ERR_INVALID_FUNCTION	= 3,
	ERR_INVALID_ADDESS		= 4,
	ERR_INVALID_SFUNCTION	= 5,
	ERR_INVALID_ACTION		= 6,
	ERR_INVALID_ENDPOINT	= 7,
	ERR_ILLEGAL_SIZE		= 8,
	ERR_TOO_LONG			= 9,
	ERR_NOT_SUPPORTED		= 10,
	ERR_PROTECTED			= 11,
	ERR_VALIDATION			= 12,
	ERR_BLOCKED				= 13,
	ERR_PROTOCOL_MISMATCH	= 254,
	ERR_USER				= 255
};
//
enum SCCI_FunctionCodes
{
	FUNCTION_WRITE			= 1,
	FUNCTION_READ			= 2,
	FUNCTION_WRITE_BLOCK	= 3,
	FUNCTION_READ_BLOCK		= 4,
	FUNCTION_CALL			= 5,
	FUNCTION_ERROR			= 6,
	FUNCTION_FAST_READ_BLK  = 7
};
//
enum SCCI_SubFunctionCodes
{
	SFUNC_NONE				= 0,
	SFUNC_16				= 1,
	SFUNC_32				= 2,
	SFUNC_16_2				= 3,
	SFUNC_REP_16			= 3,
	SFUNC_REP_32			= 4,
	SFUNC_FLOAT				= 5
};

// Functions
//
// Create protected area
Int16U xCCI_AddProtectedArea(pxCCI_ProtectionAndEndpoints PAE, Int16U StartAddress, Int16U EndAddress);
// Remove protected area
Boolean xCCI_RemoveProtectedArea(pxCCI_ProtectionAndEndpoints PAE, Int16U AreaIndex);
// Register read endpoint 16-bit source callback
Boolean xCCI_RegisterReadEndpoint16(pxCCI_ProtectionAndEndpoints PAE, Int16U Endpoint, xCCI_FUNC_CallbackReadEndpoint16 ReadCallback);
// Register read endpoint float source callback
Boolean xCCI_RegisterReadEndpointFloat(pxCCI_ProtectionAndEndpoints PAE, Int16U Endpoint, xCCI_FUNC_CallbackReadEndpointFloat ReadCallback);
// Register write endpoint 16-bit destination callback
Boolean xCCI_RegisterWriteEndpoint16(pxCCI_ProtectionAndEndpoints PAE, Int16U Endpoint, xCCI_FUNC_CallbackWriteEndpoint16 WriteCallback);
// Check register address
Boolean xCCI_InProtectedZone(pxCCI_ProtectionAndEndpoints PAE, Int16U Address);

#endif // __xCCI_COMMON_H
