// ----------------------------------------
// Controller logic
// ----------------------------------------

// Header
#include "Controller.h"
//
// Includes
#include "SysConfig.h"
#include "BoardConfig.h"
#include "DataTable.h"
#include "SCCISlave.h"
#include "DeviceProfile.h"
#include "BCCIxParams.h"

// Definitions
//
#define VALUES_WR_SIZE		512

// Types
//
typedef void (*pFunction)(void);

// Variables
//
Int32U jumpAddress;
pFunction jump2Application;
//
static Int32U WriteAddress = BOOT_LOADER_MAIN_PR_ADDR;
static volatile Boolean CycleActive = FALSE;
static Int16U MEMBUF_Values_Write[VALUES_WR_SIZE], MEMBUF_ValuesWrite_Counter = 0;
volatile Int64U CONTROL_TimeCounter = 0, CurrentTimeout = BOOT_LOADER_DELAY;
volatile Boolean WaitForFWUpload = FALSE;
volatile Boolean ResetExecute = FALSE;
//
volatile Int32U *ProgramAddressStart = ((volatile Int32U *)BOOT_LOADER_MAIN_PR_ADDR);

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U UserError);
void CONTROL_MainProgramStart();
void CONTROL_EraseFlash();
void CONTROL_ResetToDefault();
void CONTROL_ConfigCAN(Int16U NodeID);


// Functions
//
void CONTROL_Init()
{
	Int16U EPWriteIndexes[EP_WRITE_COUNT] = { EP16_WR };
	Int16U EPWriteSized[EP_WRITE_COUNT] = { VALUES_WR_SIZE };
	pInt16U EPWriteCounters[EP_WRITE_COUNT] = { (pInt16U)&MEMBUF_ValuesWrite_Counter };
	pInt16U EPWriteDatas[EP_WRITE_COUNT] = { MEMBUF_Values_Write };

	// Конфигурация сервиса работы DataTable и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация DataTable
	DT_Init(EPROMService, false);

	// Инициализация функций связанных с CAN NodeID
	// Получение значений из define или использование значения 99 по умолчанию
	Int16U NodeID = 99;
#ifdef CAN_SLAVE_NID
	NodeID = CAN_SLAVE_NID;
#elif defined(CAN_SALVE_NID)
	NodeID = CAN_SALVE_NID;
#endif
	// Попытка чтения значения из DT
	if(DataTable[REG_NODE_ID] != 0 && DataTable[REG_NODE_ID] != INT16U_MAX)
		NodeID = DataTable[REG_NODE_ID];
	CONTROL_ConfigCAN(NodeID);

	// Device profile initialization
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive, NodeID);
	DEVPROFILE_InitEPWriteService(EPWriteIndexes, EPWriteSized, EPWriteCounters, EPWriteDatas);
	// Reset control values
	DEVPROFILE_ResetControlSection();

	CONTROL_ResetToDefault();
}
// ----------------------------------------

void CONTROL_Idle()
{
	IWDG_Refresh();
	DEVPROFILE_ProcessRequests();

	if (CONTROL_TimeCounter > CurrentTimeout)
	{
		if (!WaitForFWUpload) CONTROL_MainProgramStart();
		if (ResetExecute) NVIC_SystemReset();
	}
}
// ----------------------------------------

void CONTROL_EraseFlash()
{
	NFLASH_Unlock();
	NFLASH_ErasePages(BOOT_LOADER_ERASE_FIRST_PAGE, BOOT_LOADER_ERASE_LAST_PAGE);
}
// ----------------------------------------

void CONTROL_ResetToDefault()
{
	DataTable[REG_XOR_STM] = 0;
	WriteAddress = BOOT_LOADER_MAIN_PR_ADDR;
	DEVPROFILE_ResetWriteScopes(0, EP16_WR_MASK);
}
// ----------------------------------------

void CONTROL_MainProgramStart()
{
	// Disable TIMER2
	TIM_Interupt(TIM2, 0, false);
	TIM_Stop(TIM2);

	// Reset flash flag
	BOOT_LOADER_VARIABLE = 0;

	// Slow down watchdog timer
	IWDG_ConfigureSlowUpdate();

	// Disable interrupts
	__disable_irq();

	jumpAddress = *(__IO uint32_t*)(BOOT_LOADER_MAIN_PR_ADDR + 4);
	jump2Application = (pFunction)jumpAddress;
	__set_MSP(*(__IO uint32_t*)BOOT_LOADER_MAIN_PR_ADDR);
	jump2Application();
}
//--------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U UserError)
{
	switch(ActionID)
	{
		case ACT_FL_ERASE:
			{
				IWDG_ConfigureSlowUpdate();
				CONTROL_EraseFlash();
				IWDG_ConfigureFastUpdate();
			}
			break;
		case ACT_FL_RESET_COUNTERS:
			CONTROL_ResetToDefault();
			break;
		case ACT_FL_WRITE:
			{
				if (MEMBUF_ValuesWrite_Counter > 0)
				{
					NFLASH_WriteArray16(WriteAddress, MEMBUF_Values_Write, MEMBUF_ValuesWrite_Counter);
					WriteAddress += MEMBUF_ValuesWrite_Counter * 2;

					for (Int16U i = 0; i < MEMBUF_ValuesWrite_Counter; i++)
						DataTable[REG_XOR_STM] ^= MEMBUF_Values_Write[i];

					DEVPROFILE_ResetWriteScopes(0, EP16_WR_MASK);
				}
			}
			break;
		case ACT_FL_VALIDATE:
			{
				if (DataTable[REG_XOR_STM] == DataTable[REG_XOR_PC] && DataTable[REG_XOR_STM] != 0)
				{
					CurrentTimeout = CONTROL_TimeCounter + BOOT_LOADER_RESP_DELAY;
					WaitForFWUpload = FALSE;
				}
				else
					*UserError = ERR_CRC;
			}
			break;
		case ACT_FL_SET_REQUEST:
			WaitForFWUpload = TRUE;
			break;
		case ACT_FL_CLR_REQUEST:
			BOOT_LOADER_VARIABLE = 0;
			break;
		case ACT_FL_RESET:
			{
				CurrentTimeout = CONTROL_TimeCounter + BOOT_LOADER_RESP_DELAY;
				ResetExecute = TRUE;
			}
			break;
		default:
			return FALSE;
	}

	return TRUE;
}
// ----------------------------------------

void CONTROL_ConfigCAN(Int16U NodeID)
{
	Int32U Mask = ((Int32U)NodeID) << CAN_SLAVE_NID_MPY;
	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, false);
	NCAN_FIFOInterrupt(true);
	NCAN_FilterInit(0, Mask, Mask);
	NCAN_InterruptSetPriority(0);
}
//------------------------------------------------------------------------------
