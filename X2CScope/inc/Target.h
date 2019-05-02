/**
 * @file
 *
 * @brief Common includes, macros and type definitions.
 */
/*
 * $LastChangedRevision: 12 $
 * $LastChangedDate:: 2017-03-01 18:11:38 +0100#$
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef __TARGET_H__
#define __TARGET_H__

#if defined(__GENERIC_TI_C28X__)
#define __GENERIC_DEVICE_TYPE__
#define __DATA_WIDTH_16BIT__
#define __ADDRESS_WIDTH_32BIT__

#elif defined(__GENERIC_ARM_ARMV7__) || defined(__GENERIC_ARM_ARMV6__) || defined(__GENERIC_MICROCHIP_PIC32__)
#define __GENERIC_DEVICE_TYPE__
#define __DATA_WIDTH_8BIT__
#define __ADDRESS_WIDTH_32BIT__

#elif defined(__GENERIC_MICROCHIP_DSPIC__) || defined(__GENERIC_MICROCHIP_PIC24__)
#define __GENERIC_DEVICE_TYPE__
#define __DATA_WIDTH_8BIT__
#define __ADDRESS_WIDTH_16BIT__

#elif defined(__GENERIC_X86__)
#define __GENERIC_DEVICE_TYPE__
#define __DATA_WIDTH_8BIT__
#define __ADDRESS_WIDTH_32BIT__

#endif

#if defined(__GENERIC_DEVICE_TYPE__)
#if defined(__CUSTOM_DATATYPE_DEFINITIONS__)
#include "X2cDataTypes.h"
#else
#include <stdint.h>
#if defined(__GENERIC_TI_C28X__)
typedef unsigned char uint8;
typedef signed char int8;
#else
typedef int8_t int8;
typedef uint8_t uint8;
#endif
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef float float32;
/* most compilers define long double as 64-bit float implementation */
typedef long double float64;
#endif
#endif


#if !defined(__GENERIC_DEVICE_TYPE__)

/* processor types */
/* TI processors */
#if defined(__TMS320F28069__)
#define __TMS320F2806X__
#elif defined(__TMS320F28035__) || defined(__TMS320F28034__)
#define __TMS320F2803X__
#elif defined(__TMS320F28027__)
#define __TMS320F2802X__
#elif defined(__TMS320F28335__)
#define __TMS320F2833X__
#elif defined(__TMS320F2808__)
#define __TMS320F280X__
#elif defined(__TMS320F2811__)
#define __TMS320F281X__
#elif defined(__TM4C123GH6__) || defined(__TM4C123BE6__)
#define __SERIES_TM4C__

/* ST processors */
#elif defined(__STM32F100C6__)
#define __STM32F100XX__
#elif defined(__STM32F103T6__) || defined(__STM32F103VB__) || defined(__STM32F103V8__)
#define __STM32F103XX__
#elif defined(__STM32F103ZC__)
#define __SERIES_STM32F1__
#elif defined(__STM32F407ZG__) || defined(__STM32F407ZE__) || defined(__STM32F407VG__)
#define __SERIES_STM32F4__
#elif defined(__STM32F051R8__) || defined(__STM32F051C8__)
#define __SERIES_STM32F0__
#elif defined(__STM32F303RE__) || defined(__STM32F303RB__)
#define __SERIES_STM32F3__

/* Renesas processors */
#elif defined(__RX62T__) || defined(__R5F562TA__)
#include <iorx62t.h>
#define __RX6XX__
#elif defined(__RX62G__) || defined(__R5F562GA__)
#include <iorx62g.h>
#define __RX6XX__

/* Freescale processors */
#elif defined(__MPC5643L__)
#include <MPC5643L.h>
#define __SERIES_MPC5__

#elif defined(__KECONTROL__)
#define __SERIES_KECONTROL__

#elif defined(__DSPIC33EP256MC506__) || defined(__DSPIC33EP128GP502__) || defined(__DSPIC33EP32GP502__) || \
defined(__DSPIC33EP256GP502__) || defined(__DSPIC33EP256MC502__) || defined(__DSPIC33EP128MC202__) || \
defined(__DSPIC33EP128GM604__)
#define __SERIES_DSPIC33E__
#elif defined(__DSPIC33FJ12GP202__) || defined(__DSPIC33FJ128MC802__)
#define __SERIES_DSPIC33F__
#elif defined(__PIC32MZ2048EC__)
#define __SERIES_PIC32MZ__
#elif defined(__PIC32MX170F256__)
#define __SERIES_PIC32MX__

#elif defined(__XMC4800F144K2048__)
#define __SERIES_XMC4800__

#elif defined(MATLAB_MEX_FILE)
#define __MATLAB__

#elif defined(SCILAB_SIM_FILE)
#define __SCILAB__

#else
#error PROCESSOR TYPE NOT DEFINED
#endif

/* TI processor family */
#if defined(__TMS320F2806X__) || defined(__TMS320F2803X__) || \
	defined(__TMS320F2833X__) || defined(__TMS320F280X__) || \
	defined(__TMS320F281X__) || defined(__TMS320F2802X__)
#define __C2000__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_16BIT__

#elif defined(__SERIES_TM4C__)
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__STM32F100XX__) || defined(__STM32F103XX__)
#define __STM32F10XXX__
#define __VENDOR_ST__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__SERIES_STM32F4__) || defined(__SERIES_STM32F0__) || defined(__SERIES_STM32F1__) || \
	defined(__SERIES_STM32F3__)
#define __VENDOR_ST__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__RX6XX__)
#define __RENESAS_RX__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__SERIES_DSPIC33E__) || defined(__SERIES_DSPIC33F__)
#define __VENDOR_MICROCHIP__
#define __ADDRESS_WIDTH_16BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__SERIES_PIC32MZ__) || defined(__SERIES_PIC32MX__)
#define __VENDOR_MICROCHIP__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__SERIES_MPC5__)
#define __VENDOR_FREESCALE__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__SERIES_KECONTROL__)
#define __VENDOR_KEBA__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__SERIES_XMC4800__)
#define __VENDOR_INFINEON__
#define __ADDRESS_WIDTH_32BIT__
#define __DATA_WIDTH_8BIT__

#elif defined(__MATLAB__) || defined(__SCILAB__)
/* do nothing, just prevent compiler errors in Matlab/Scilab */
#else
#error PROCESSOR TYPE NOT DEFINED
#endif

/* compiler information */
#if defined(__C2000__) || defined(__SERIES_TM4C__)
#define __COMPILER_TI__

#elif defined(__VENDOR_ST__)
#define __COMPILER_ST__

#elif defined(__RENESAS_RX__)
#define __COMPILER_IAR_EW__

#elif defined(__MATLAB__)
#define __COMPILER_MATLAB__

#elif defined(__SCILAB__)
#define __COMPILER_SCILAB__

#elif defined(__VENDOR_MICROCHIP__)
#define __COMPILER_MICROCHIP__

#elif defined(__VENDOR_FREESCALE__) || defined(__VENDOR_KEBA__) || defined(__VENDOR_INFINEON__)
/* just avoid compiler error */

#else
#error PROCESSOR TYPE NOT DEFINED
#endif


/* includes, system information etc. */
#if defined(__TMS320F2806X__)
#include "F2806x_Device.h"
#define KICK_DOG \
	do { \
		EALLOW; \
		SysCtrlRegs.WDKEY = (uint16)0x0055; \
		SysCtrlRegs.WDKEY = (uint16)0x00AA; \
		EDIS; \
	} while (0)
#elif defined(__TMS320F2803X__)
#include "DSP2803x_Device.h"
#define KICK_DOG \
	do { \
		EALLOW; \
		SysCtrlRegs.WDKEY = (uint16)0x0055; \
		SysCtrlRegs.WDKEY = (uint16)0x00AA; \
		EDIS; \
	} while (0)
#elif defined(__TMS320F2802X__)
#include "F2802x_Device.h"
#define KICK_DOG \
	do { \
		EALLOW; \
		SysCtrlRegs.WDKEY = (uint16)0x0055; \
		SysCtrlRegs.WDKEY = (uint16)0x00AA; \
		EDIS; \
	} while (0)
#elif defined(__TMS320F2833X__)
#include "DSP2833x_Device.h"
#define KICK_DOG \
	do { \
		EALLOW; \
		SysCtrlRegs.WDKEY = (uint16)0x0055; \
		SysCtrlRegs.WDKEY = (uint16)0x00AA; \
		EDIS; \
	} while (0)
#elif defined(__TMS320F280X__)
#include "DSP280x_Device.h"
#define KICK_DOG \
	do { \
		EALLOW; \
		SysCtrlRegs.WDKEY = (uint16)0x0055; \
		SysCtrlRegs.WDKEY = (uint16)0x00AA; \
		EDIS; \
	} while (0)
#elif defined(__TMS320F281X__)
#include "DSP281x_Device.h"
#define KICK_DOG \
	do { \
		EALLOW; \
		SysCtrlRegs.WDKEY = (uint16)0x0055; \
		SysCtrlRegs.WDKEY = (uint16)0x00AA; \
		EDIS; \
	} while (0)
#elif defined(__STM32F10XXX__)
	#include "stm32f10x_lib.h"
	#define KICK_DOG \
		do { \
			IWDG_ReloadCounter(); \
		} while (0)
#elif defined(__SERIES_STM32F1__)
	#include "stm32f10x.h"
	#define KICK_DOG \
		do { \
			IWDG_ReloadCounter(); \
		} while (0)
#elif defined(__SERIES_STM32F4__)
	#include <stm32f4xx.h>
	#define KICK_DOG \
		do { \
			IWDG_ReloadCounter(); \
		} while (0)
#elif defined(__SERIES_STM32F0__)
	#include <stm32f0xx.h>
	#define KICK_DOG \
		do { \
			IWDG_ReloadCounter(); \
		} while (0)
#elif defined(__SERIES_STM32F3__)
	#include <stm32f30x.h>
	#define KICK_DOG \
		do { \
			IWDG_ReloadCounter(); \
		} while (0)

#elif defined(__RENESAS_RX__)
#define KICK_DOG \
          do { \
          } while (0)
#elif defined(__VENDOR_MICROCHIP__)
#include <xc.h>
#if defined(__XC32__)
#define KICK_DOG \
    do { \
        WDTCONSET = 0x01; \
    } while (0)
#else
#define KICK_DOG \
    do { \
        ClrWdt(); \
    } while (0)
#endif

#elif defined(__SERIES_MPC5__)
#define KICK_DOG \
	do { \
		SWT.SR.B.WSC = 0xA602; \
		SWT.SR.B.WSC = 0xB480; \
	} while (0)
#elif defined(__SERIES_TM4C__)
#include <stdint.h>
#include <stdbool.h>
#include <driverlib/pin_map.h>
#include <driverlib/uart.h>
#include <driverlib/watchdog.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>
#include <inc/hw_memmap.h>
#ifndef WATCHDOG_RESET_VALUE
#define WATCHDOG_RESET_VALUE ((uint32)400000)
#endif
#define KICK_DOG \
	do { \
		WatchdogUnlock(WATCHDOG0_BASE); \
		WatchdogReloadSet(WATCHDOG0_BASE, WATCHDOG_RESET_VALUE); \
		WatchdogLock(WATCHDOG0_BASE); \
	} while(0)
/* disables watchdog reset */
#define DISABLE_WATCHDOG_RESET \
	do { \
		WatchdogUnlock(WATCHDOG0_BASE); \
		WatchdogResetDisable(WATCHDOG0_BASE); \
		WatchdogLock(WATCHDOG0_BASE); \
	} while(0)

/* enables watchdog reset */
#define ENABLE_WATCHDOG_RESET \
	do { \
		WatchdogUnlock(WATCHDOG0_BASE); \
		WatchdogResetEnable(WATCHDOG0_BASE); \
		WatchdogLock(WATCHDOG0_BASE); \
	} while(0)


#elif defined(__VENDOR_INFINEON__)
#include <DAVE.h>
#define KICK_DOG \
	do { \
		WATCHDOG_ClearAlarm(); \
		WATCHDOG_Service(); \
} while(0)

#elif defined(__MATLAB__) || defined(__SCILAB__) || defined(__VENDOR_KEBA__)
/* do nothing, just avoid compiler errors */
#else
#error PROCESSOR TYPE NOT DEFINED
#endif

/* datatype definitions */
#if defined(__MATLAB__) || defined(__SCILAB__)
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed long int32;
typedef unsigned long uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef float float32;
typedef double float64;
#else
#error UNSUPPORTED ARCHITECTURE
#endif
#elif defined(__TMS320F2806X__) || defined(__TMS320F2833X__) || \
	defined(__TMS320F280X__) || defined(__TMS320F281X__)
/* int16, int32, int64 already defined in TI header files */
typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
#elif defined(__TMS320F2803X__) || defined(__TMS320F2802X__)
/* int16, int32 already defined in TI header file */
typedef signed char int8;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
#elif defined(__STM32F10XXX__) || defined(__RENESAS_RX__) || \
  defined(__SERIES_DSPIC33E__) || defined(__SERIES_DSPIC33F__) || defined(__SERIES_STM32F4__) || \
  defined(__SERIES_PIC32MZ__) || defined(__SERIES_PIC32MX__) ||\
  defined(__SERIES_STM32F0__) || defined(__SERIES_STM32F1__) || defined(__SERIES_TM4C__) || \
  defined(__SERIES_KECONTROL__) || defined(__SERIES_XMC4800__) || defined(__SERIES_STM32F3__)
#include <stdint.h>
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef float float32;
typedef double float64;
#elif defined(__SERIES_MPC5__)
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef float float32;
typedef double float64;
#else
#error PROCESSOR TYPE NOT DEFINED
#endif

#endif

#include "Typecast.h"

/* limits for INT8 values */
#ifndef INT8_MAX
#define INT8_MAX ((int8)127)
#endif
#ifndef INT8_MIN
#define INT8_MIN ((int8)-128)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX ((uint8)255)
#endif

/* limits for INT16 values */
#ifndef INT16_MAX
#define INT16_MAX ((int16)32767)
#endif
#ifndef INT16_MIN
#define INT16_MIN ((int16)-32768)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX ((uint16)65535)
#endif

/* limits for INT32 values */
#ifndef INT32_MAX
#define INT32_MAX ((int32)2147483647)
#endif
#ifndef INT32_MIN
#define INT32_MIN ((int32)-2147483648)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX ((uint32)4294967295)
#endif

/* limits for INT64 values */
#ifndef INT64_MAX
#define INT64_MAX ((int64)9223372036854775807)
#endif
#ifndef INT64_MIN
#define INT64_MIN ((int64)-9223372036854775808)
#endif
#ifndef UINT64_MAX
#define UINT64_MAX ((uint64)18446744073709551615)
#endif

/* limits for FLOAT32 values */
#ifndef FLOAT32_MAX
#define FLOAT32_MAX	 ((float32)3.4028234663852885981170418348452e+38)
#endif
#ifndef FLOAT32_MIN
#define FLOAT32_MIN	 ((float32)-3.4028234663852885981170418348452e+38)
#endif

/* limits for FLOAT64 values */
#if defined(__COMPILER_TI__)
#ifndef FLOAT64_MAX
#define FLOAT64_MAX	  ((float64)3.4028234663852885981170418348452e+38)
#endif
#ifndef FLOAT64_MIN
#define FLOAT64_MIN	  ((float64)-3.4028234663852885981170418348452e+38)
#endif
#else
#ifndef FLOAT64_MAX
#define FLOAT64_MAX	  ((float64)1.797693134862315708145274237317e+308)
#endif
#ifndef FLOAT64_MIN
#define FLOAT64_MIN	  ((float64)-1.797693134862315708145274237317e+308)
#endif
#endif

/* limits for INT15 values */
#define INT15_MAX ((int16)16383)
#define INT15_MIN ((int16)-16384)

/* limits for INT30 values */
#define INT30_MAX ((int32)536870911)
#define INT30_MIN ((int32)-536870912)
#define UINT30_MAX ((uint32)1073741823)

/* limits for INT31 values */
#define INT31_MAX ((int32)1073741823)
#define INT31_MIN ((int32)-1073741824)
#define UINT31_MAX ((uint32)2147483647)

/* limits for INT63 values */
#define INT63_MAX ((int64)4611686018427387903)
#define INT63_MIN ((int64)-4611686018427387904)

/* floating point constants */
#define PI_R32		((float32)3.1415926535897932384626433832795028841971693993751058209749445923078164062)
#define PI_R64		((float64)3.1415926535897932384626433832795028841971693993751058209749445923078164062)

typedef enum typeclass_t {
	TYPE_INT8,
	TYPE_UINT8,
	TYPE_INT16,
	TYPE_UINT16,
	TYPE_INT32,
	TYPE_UINT32,
	TYPE_INT64,
	TYPE_UINT64,
	TYPE_FLOAT32,
	TYPE_FLOAT64
} TypeClass;

#endif
