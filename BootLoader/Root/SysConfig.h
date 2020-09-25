#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H


// Flash loader options
//--------------------------------------------------------
#define BOOT_LOADER_VARIABLE			(*((volatile uint32_t *)0x20000000))
#define BOOT_LOADER_REQUEST				0x12345678
#define BOOT_LOADER_DELAY				5000			// in ms
#define BOOT_LOADER_RESP_DELAY			20				// in ms
//
#define BOOT_LOADER_FLAG_ADDR			0x20000000
#define BOOT_LOADER_MAIN_PR_ADDR		0x08008000
//
#define BOOT_LOADER_ERASE_FIRST_PAGE	BOOT_LOADER_MAIN_PR_ADDR
#define BOOT_LOADER_ERASE_LAST_PAGE		0x0801F800
//--------------------------------------------------------

// System Clock
//--------------------------------------------------------
#define SYSCLK							70000000		// in Hz
#define QUARTZ_FREQUENCY				20000000		// in Hz
//--------------------------------------------------------

// USART
//--------------------------------------------------------
#define USART_BAUDRATE					115200
#define USART_FIFOlen					32
#define	USART_TIMEOUT_TICKS				1000
//--------------------------------------------------------

// Timers
//--------------------------------------------------------
#define TIMER2_uS						1000			// in us
#define TIMER2_BLINK_PERIOD				50				// in ticks
//--------------------------------------------------------

// CAN
//--------------------------------------------------------
#define CAN_BAUDRATE					1000000
//--------------------------------------------------------


#endif // __SYSCONFIG_H
