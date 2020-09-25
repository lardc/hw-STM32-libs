// -----------------------------------------
// General definitions
// -----------------------------------------

#ifndef __ZW_BASE_H
#define __ZW_BASE_H

#include "stdbool.h"
#include "stdint.h"

// Common definitions
//
#define MHZ			*1000000L
#define KHZ			*1000L
#define HZ			*1L

// Type definitions
//
typedef long double			Float64;	// Double precision floating point
typedef long double			* pFloat64;
typedef float				Float32;	// Single precision floating point
typedef float				* pFloat32;
typedef signed long long	Int64S;	// Signed	64 bit quantity
typedef signed long long	* pInt64S;
typedef unsigned long long	Int64U;	// Unsigned 64 bit quantity
typedef unsigned long long	* pInt64U;
typedef signed int			Int32S;	// Signed	32 bit quantity
typedef signed int			* pInt32S;
typedef unsigned int		Int32U;	// Unsigned	32 bit quantity
typedef unsigned int		* pInt32U;
typedef signed short		Int16S;	// Signed	16 bit quantity
typedef signed short		* pInt16S;
typedef unsigned short		Int16U;	// Unsigned	16 bit quantity
typedef unsigned short		* pInt16U;
typedef signed char			Int8S;	// Signed	8 bit quantity
typedef signed char			* pInt8S;
typedef unsigned char		Int8U;	// Unsigned	8 bit quantity
typedef unsigned char		* pInt8U;

// Boolean definition
//
#ifdef bool
#undef bool
#undef true
#undef false
#endif

typedef _Bool bool;
typedef enum __bool_val
{
	false = 0,
	true = 1
} bool_val;

#ifndef Boolean
typedef bool				Boolean;
typedef bool				* pBoolean;
typedef enum __bool_val_c
{
	FALSE = 0,
	TRUE = 1
} bool_val_c;
#endif

#ifndef NULL
#define NULL				((void *)0)
#endif

// Limits
//
#define INT8U_MAX	255
#define INT8S_MAX	127
#define INT8S_MIN	(-INT8S_MAX-1)
#define INT16U_MAX	65535
#define INT16S_MAX	32767
#define INT16S_MIN	(-INT16S_MAX-1)
#define INT32U_MAX	4294967295ul
#define INT32S_MAX	2147483647
#define INT32S_MIN	(-INT32S_MAX-1)
#define INT64U_MAX	18446744073709551615
#define INT64S_MAX	9223372036854775807
#define INT64S_MIN	(-INT64S_MAX-1)

// Bits
//
#define BIT0	0x1u
#define BIT1	0x2u
#define BIT2	0x4u
#define BIT3	0x8u
#define BIT4	0x10u
#define BIT5	0x20u
#define BIT6	0x40u
#define BIT7	0x80u
#define BIT8	0x100u
#define BIT9	0x200u
#define BIT10	0x400u
#define BIT11	0x800u
#define BIT12	0x1000u
#define BIT13	0x2000u
#define BIT14	0x4000u
#define BIT15	0x8000u
#define BIT16	0x10000u
#define BIT17	0x20000u
#define BIT18	0x40000u
#define BIT19	0x80000u
#define BIT20	0x100000u
#define BIT21	0x200000u
#define BIT22	0x400000u
#define BIT23	0x800000u
#define BIT24	0x1000000u
#define BIT25	0x2000000u
#define BIT26	0x4000000u
#define BIT27	0x8000000u
#define BIT28	0x10000000u
#define BIT29	0x20000000u
#define BIT30	0x40000000u
#define BIT31	0x80000000u

#endif // __ZW_BASE_H
