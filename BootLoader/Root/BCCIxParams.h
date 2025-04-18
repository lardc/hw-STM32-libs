// -----------------------------------------
// Parameters of BCCI interface
// ----------------------------------------

#ifndef __BCCI_PARAMS_H
#define __BCCI_PARAMS_H

#include "xCCIParams.h"


// Маска поля функции
//
#define CAN_FUNC_MASK				0x000003FF

// Конфигурация Slave
//
#define CAN_SLAVE_NID				99				// Node ID устройства
#define CAN_SLAVE_NID_MASK			0x0003FC00		// Маска Slave node ID
#define CAN_SLAVE_NID_MPY			10				// Смещение адреса Slave

// Конфигурация Master
//
#define CAN_MASTER_NID				0				// Node ID устройства
#define CAN_MASTER_NID_MASK			0x03FC0000		// Маска Master node ID
#define CAN_MASTER_NID_MPY			18				// Смещение адреса Master

// CAN фильтр
//
#define CAN_SLAVE_FILTER_ID			(CAN_SLAVE_NID << CAN_SLAVE_NID_MPY)
#define CAN_MASTER_FILTER_ID		(CAN_MASTER_NID << CAN_MASTER_NID_MPY)


#endif // __BCCI_PARAMS_H
