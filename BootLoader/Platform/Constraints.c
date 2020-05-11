// -----------------------------------------
// Global definitions
// ----------------------------------------

// Header
#include "Constraints.h"
#include "DeviceObjectDictionary.h"

#define NO		0	// equal to FALSE
#define YES		1	// equal to TRUE

// Constants
//
const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE] =
									  {
										   {0, 0, 0},							// 0
										   {0, 0, 0}							// 1
									  };

const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START] =
									 {
										   {0, INT16U_MAX, 0},					// 2
										   {0, INT16U_MAX, 0},					// 3
										   {0, 0, 0},							// 4
										   {0, 0, 0},							// 5
										   {0, 0, 0},							// 6
										   {0, 0, 0},							// 7
										   {0, 0, 0},							// 8
										   {0, 0, 0}							// 9
									 };
// ----------------------------------------

// No more
