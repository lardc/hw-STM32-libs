// ----------------------------------------
// Device object dictionary
// ----------------------------------------

#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H


// ACTIONS
//
#define ACT_SAVE_TO_ROM				200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM		201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT		202	// Сброс DataTable в состояние по умолчанию
//
#define ACT_FL_ERASE				300	// Erase flash
#define ACT_FL_RESET_COUNTERS		301	// Reset checksum counters
#define ACT_FL_WRITE				302	// Begin flash write
#define ACT_FL_VALIDATE				303	// Validate flash write
#define ACT_FL_CLR_REQUEST			312	// Clear request flag for firmware upload
#define ACT_FL_RESET				313	// Force reset
//
#define ACT_FL_SET_REQUEST			320	// Set request flag for firmware upload


// REGISTERS
//
// No NVRAM section
//
#define REG_NODE_ID					0	// Выставление CAN NodeID
#define REG_DUMMY					2	// Compatibility register
#define REG_XOR_PC					3	// PC calculated XOR value
//
#define REG_XOR_STM					10	// STM calculated XOR value


// ENDPOINTS
//
#define EP16_WR						1	// Endpoint for firmware value sequence


// ENDPOINTS MASKS
//
#define EP16_WR_MASK				BIT0


// USER ERROR CODES
//
#define ERR_NONE					0	// No error
#define ERR_CONFIGURATION_LOCKED	1	// Device is locked for writing
#define ERR_OPERATION_BLOCKED		2	// Operation can't be done due to current device state
#define ERR_DEVICE_NOT_READY		3	// Device isn't ready to switch state
#define ERR_WRONG_PWD				4	// Неправильный ключ


#endif // __DEV_OBJ_DIC_H
