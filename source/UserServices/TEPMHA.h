/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TEPMHA_H
#define TEPMHA_H

#include "build.h"
#include "IOAPI.h"
#include "TEPMAPI.h"
#include "SYSAPI.h"
#include "CEM.h"

#ifdef BUILD_SAM3X8E
#include "tc.h"
#include "pwm.h"
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define TEPMHA_nEventChannels       16u
typedef FTM_Type tstTimerModule;
#endif //BUILD_MK60

#ifdef BUILD_MK64
#include "mk64f12.h"
#define TEPMHA_nEventChannels	18u
typedef FTM_Type tstTimerModule;
#define TEPMHA_nCounterMask 0xffff
#define TEPMHA_nLongestDelay 0xfe00
#define TEPMHA_nCounterGlobalDiv 4

#ifdef BUILD_SPARKDOG_PF
#define TEPMHA_nPRIOCHANNEL 5
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
#define TEPMHA_nPRIOCHANNEL 4
#endif //BUILD_SPARKDOG_TEENSY_ADAPT

#endif //BUILD_MK64

#ifdef BUILD_SAM3X8E
#define TEPMHA_nEventChannels       16u
typedef Tc tstTimerModule;
typedef Pwm tstPWMModule;
#define TEPMHA_nCounterMask 0xffffffff
#define TEPMHA_nCounterGlobalDiv 8
#endif //BUILD_SAM3X8E


#define TEPMHA_nUserEventsMax       4u
#define TEPMHA_nKernelEventsMax     100u
#define TEPMHA_nMastersMax		    1u


#ifdef _TEPMHA_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN uint32 TEPMHA_u32PortClockRequested;


#ifdef BUILD_MK60
typedef enum
{
	TEPMHA_enFTM0,
	TEPMHA_enFTM1,
	TEPMHA_enFTM2,
	TEPMHA_enFTM3,
	TEPMHA_enTimerModuleCount
} TEPMHA_tenTimerModule;	
#endif //BUILD_MK60

#ifdef BUILD_MK64
typedef enum
{
	TEPMHA_enFTM0,
	TEPMHA_enFTM1,
	TEPMHA_enFTM2,
	TEPMHA_enFTM3,
	TEPMHA_enTimerModuleCount
} TEPMHA_tenTimerModule;
#endif //BUILD_MK64

#ifdef BUILD_SAM3X8E
typedef enum
{
	TEPMHA_enTC0,
	TEPMHA_enTC1,
	TEPMHA_enTC2,
	TEPMHA_enPWM0,
	TEPMHA_enModuleCount,
	TEPMHA_enModuleInvalid
} TEPMHA_tenModule;
#endif //BUILD_SAM3X8E

typedef enum
{
    TEPMHA_enCapCom,
	TEPMHA_enPWM,
	TEPMHA_enSoft
} TEPMHA_tenModuleType;


#ifdef BUILD_MK60
#define TEPMHA_nChannelInfo            \
{                                      \
	{EH_IO_TMR1, TEPMHA_enFTM0, 7u, 4u},  \
	{EH_IO_TMR2, TEPMHA_enFTM0, 6u, 4u},  \
	{EH_IO_TMR3, TEPMHA_enFTM0, 5u, 4u},  \
	{EH_IO_TMR4, TEPMHA_enFTM0, 4u, 4u},  \
	{EH_IO_TMR5, TEPMHA_enFTM3, 3u, 4u},  \
	{EH_IO_TMR6, TEPMHA_enFTM3, 2u, 4u},  \
	{EH_IO_TMR7, TEPMHA_enFTM3, 1u, 4u},  \
	{EH_IO_TMR8, TEPMHA_enFTM3, 0u, 4u},  \
	{EH_IO_TMR9, TEPMHA_enFTM3, 4u, 6u},  \
	{EH_IO_TMR10, TEPMHA_enFTM3, 5u, 6u}, \
	{EH_IO_TMR11, TEPMHA_enFTM3, 6u, 6u}, \
	{EH_IO_TMR12, TEPMHA_enFTM3, 7u, 6u}, \
	{EH_IO_TMR13, TEPMHA_enFTM0, 3u, 4u}, \
	{EH_IO_TMR14, TEPMHA_enFTM0, 2u, 4u}, \
	{EH_IO_TMR15, TEPMHA_enFTM0, 1u, 4u}, \
	{EH_IO_TMR16, TEPMHA_enFTM0, 0u, 4u}, \
	{EH_IO_ADSE4, TEPMHA_enFTM1, 0u, 6u}, \
	{EH_IO_GPSE9, TEPMHA_enFTM1, 0u, 3u}, \
	{EH_IO_GPSE8, TEPMHA_enFTM1, 1u, 3u}, \
	{EH_IO_ADSE5, TEPMHA_enFTM2, 0u, 6u}, \
	{EH_IO_GPSE7, TEPMHA_enFTM2, 0u, 3u}, \
	{EH_IO_CAN1T, TEPMHA_enFTM2, 0u, 3u}, \
	{EH_IO_CAN1R, TEPMHA_enFTM2, 1u, 3u}  \
}	
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPMHA_nChannelInfo               \
{                                       \
	{EH_IO_TMR1, FTM_enFTM3,           TEPMHA_enCapCom, 0u,0u, 4u, FTM3_IRQn},	\
	{EH_IO_TMR2, FTM_enFTM0,           TEPMHA_enCapCom, 7u,0u, 4u, FTM0_IRQn},	\
	{EH_IO_TMR3, FTM_enFTM0,           TEPMHA_enCapCom, 4u,0u, 4u, FTM0_IRQn},	\
	{EH_IO_TMR4, FTM_enFTM3,           TEPMHA_enCapCom, 2u,0u, 4u, FTM3_IRQn},	\
	{EH_IO_TMR5, FTM_enFTM3,           TEPMHA_enCapCom, 3u,0u, 4u, FTM3_IRQn},	\
	{EH_IO_TMR6, FTM_enFTM0,           TEPMHA_enCapCom, 2u,0u, 4u, FTM0_IRQn},	\
	{EH_IO_TMR7, FTM_enFTM0,           TEPMHA_enCapCom, 3u,0u, 4u, FTM0_IRQn},	\
	{EH_IO_TMR8, FTM_enFTM2,           TEPMHA_enCapCom, 0u,0u, 3u, FTM2_IRQn},	\
	{EH_IO_TMR9, FTM_enFTM2,           TEPMHA_enCapCom, 1u,0u, 3u, FTM2_IRQn},	\
	{EH_IO_TMR10, FTM_enFTM3,           TEPMHA_enCapCom, 4u,0u, 3u, FTM3_IRQn},	\
	{EH_IO_TMR11, FTM_enFTM3,           TEPMHA_enCapCom, 5u,0u, 3u, FTM3_IRQn},	\
	{EH_IO_TMR12, FTM_enFTM3,           TEPMHA_enCapCom, 6u,0u, 3u, FTM3_IRQn},	\
	{EH_IO_TMR13, FTM_enFTM3,           TEPMHA_enCapCom, 7u,0u, 3u, FTM3_IRQn},	\
	{EH_IO_TMR14, FTM_enFTM3,           TEPMHA_enCapCom, 1u,0u, 3u, FTM3_IRQn},	\
	{EH_IO_TMR15, FTM_enFTM0,           TEPMHA_enCapCom, 5u,0u, 4u, FTM0_IRQn},	\
	{EH_IO_TMR16, FTM_enFTM0,           TEPMHA_enCapCom, 6u,0u, 4u, FTM0_IRQn},	\
	{EH_IO_TMR17, FTM_enFTM0,           TEPMHA_enCapCom, 0u,0u, 4u, FTM0_IRQn},  \
	{EH_IO_TMR18, FTM_enFTM0,           TEPMHA_enCapCom, 1u,0u, 4u, FTM0_IRQn},  \
}

#define TEPMHA_nChannelReverseInfo \
{\
	{FTM_enFTM0, 0u, EH_IO_TMR17},\
	{FTM_enFTM0, 1u, EH_IO_TMR18},\
	{FTM_enFTM0, 2u, EH_IO_TMR6},\
	{FTM_enFTM0, 3u, EH_IO_TMR7},\
	{FTM_enFTM0, 4u, EH_IO_TMR3},\
	{FTM_enFTM0, 5u, EH_IO_TMR15},\
	{FTM_enFTM0, 6u, EH_IO_TMR16},\
	{FTM_enFTM0, 7u, EH_IO_TMR2},\
	{FTM_enFTM1, 0u, EH_IO_Invalid},\
	{FTM_enFTM1, 1u, EH_IO_Invalid},\
	{FTM_enFTM1, 2u, EH_IO_Invalid},\
	{FTM_enFTM1, 3u, EH_IO_Invalid},\
	{FTM_enFTM1, 4u, EH_IO_Invalid},\
	{FTM_enFTM1, 5u, EH_IO_Invalid},\
	{FTM_enFTM1, 6u, EH_IO_Invalid},\
	{FTM_enFTM1, 7u, EH_IO_Invalid},\
	{FTM_enFTM2, 0u, EH_IO_TMR8},\
	{FTM_enFTM2, 1u, EH_IO_TMR9},\
	{FTM_enFTM2, 2u, EH_IO_Invalid},\
	{FTM_enFTM2, 3u, EH_IO_Invalid},\
	{FTM_enFTM2, 4u, EH_IO_Invalid},\
	{FTM_enFTM2, 5u, EH_IO_Invalid},\
	{FTM_enFTM2, 6u, EH_IO_Invalid},\
	{FTM_enFTM2, 7u, EH_IO_Invalid},\
	{FTM_enFTM3, 0u, EH_IO_TMR1},\
	{FTM_enFTM3, 1u, EH_IO_TMR14},\
	{FTM_enFTM3, 2u, EH_IO_TMR4},\
	{FTM_enFTM3, 3u, EH_IO_TMR5},\
	{FTM_enFTM3, 4u, EH_IO_TMR10},\
	{FTM_enFTM3, 5u, EH_IO_TMR11},\
	{FTM_enFTM3, 6u, EH_IO_TMR12},\
	{FTM_enFTM3, 7u, EH_IO_TMR13}\
}
#endif //BUILD_MK64

#ifdef BUILD_SAM3X8E
#define TEPMHA_nChannelInfo                                                                      \
{                                                                                                \
	{EH_IO_TMR1,  TEPMHA_enTC0,           TEPMHA_enCapCom,  0u, 0u, 1u, TC0_IRQn},               \
	{EH_IO_TMR2,  TEPMHA_enTC2,           TEPMHA_enCapCom,  2u, 0u, 1u, TC7_IRQn},               \
	{EH_IO_TMR3,  TEPMHA_enTC2,           TEPMHA_enCapCom,  0u, 1u, 1u, TC6_IRQn},               \
	{EH_IO_TMR4,  TEPMHA_enTC2,           TEPMHA_enCapCom,  0u, 0u, 1u, TC6_IRQn},               \
	{EH_IO_TMR5,  TEPMHA_enPWM0,          TEPMHA_enPWM,     7u, 0u, 1u, PWM_IRQn},               \
	{EH_IO_TMR6,  TEPMHA_enPWM0,          TEPMHA_enPWM,     6u, 0u, 1u, PWM_IRQn},               \
	{EH_IO_TMR7,  TEPMHA_enPWM0,          TEPMHA_enPWM,     5u, 0u, 1u, PWM_IRQn},               \
	{EH_IO_TMR8,  TEPMHA_enPWM0,          TEPMHA_enPWM,     4u, 0u, 1u, PWM_IRQn},               \
	{EH_IO_TMR9,  TEPMHA_enTC2,           TEPMHA_enCapCom,  2u, 1u, 1u, TC7_IRQn},               \
	{EH_IO_TMR10, TEPMHA_enTC2,           TEPMHA_enCapCom,  4u, 0u, 1u, TC8_IRQn},               \
	{EH_IO_TMR11, TEPMHA_enTC2,           TEPMHA_enCapCom,  4u, 1u, 1u, TC8_IRQn},               \
	{EH_IO_TMR12, TEPMHA_enTC0,           TEPMHA_enCapCom,  0u, 1u, 1u, TC0_IRQn},               \
	{EH_IO_ADD5, TEPMHA_enTC0,           TEPMHA_enCapCom,  4u, 1u, 0u, TC2_IRQn},               \
	{EH_IO_ADD7, TEPMHA_enTC0,           TEPMHA_enCapCom,  2u, 1u, 0u, TC1_IRQn},               \
	{EH_IO_ADD8, TEPMHA_enTC0,           TEPMHA_enCapCom,  2u, 0u, 0u, TC1_IRQn},               \
	{EH_IO_EXTINT, TEPMHA_enTC0,           TEPMHA_enCapCom,  4u, 0u, 0u, TC2_IRQn},               \
}
#endif //BUILD_SAM3X8E

#ifdef BUILD_SAM3X8E
#define TEPMHA_nChannelResourceList   \
{                                                                                                \
	EH_IO_TMR1,               \
	EH_IO_TMR2,               \
	EH_IO_TMR3,               \
	EH_IO_TMR4,             \
	EH_IO_TMR5,               \
	EH_IO_TMR6,               \
	EH_IO_TMR7,               \
	EH_IO_TMR8,               \
	EH_IO_TMR9,               \
	EH_IO_TMR10,               \
	EH_IO_TMR11,               \
	EH_IO_TMR12,               \
	EH_IO_ADD5,               \
	EH_IO_ADD7,               \
	EH_IO_ADD8,               \
	EH_IO_EXTINT               \
}
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK64
#define TEPMHA_nChannelResourceList   \
{                 \
	EH_IO_TMR1,   \
	EH_IO_TMR2,   \
	EH_IO_TMR3,   \
	EH_IO_TMR4,   \
	EH_IO_TMR5,   \
	EH_IO_TMR6,   \
	EH_IO_TMR7,   \
	EH_IO_TMR8,   \
	EH_IO_TMR9,   \
	EH_IO_TMR10,  \
	EH_IO_TMR11,  \
	EH_IO_TMR12,  \
	EH_IO_TMR13,  \
	EH_IO_TMR14,  \
	EH_IO_TMR15,  \
	EH_IO_TMR16,  \
	EH_IO_TMR17,  \
	EH_IO_TMR18   \
}
#endif //BUILD_MK64


#ifdef BUILD_MK60
#define TEPMHA_nChannelFastInfo \
{                               \
	EH_IO_TMR16,              \
	EH_IO_TMR15,              \
	EH_IO_TMR14,              \
	EH_IO_TMR13,              \
	EH_IO_TMR4,               \
	EH_IO_TMR3,               \
	EH_IO_TMR2,               \
	EH_IO_TMR1                \
}
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPMHA_nChannelFastInfo  \
{                   \
	EH_IO_TMR2,    \
	EH_IO_TMR3,    \
	EH_IO_TMR8,    \
	EH_IO_TMR9,    \
	EH_IO_TMR5,    \
	EH_IO_TMR10,   \
	EH_IO_TMR11,   \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_TMR13,   \
	EH_IO_TMR14,   \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_Invalid, \
	EH_IO_TMR1,   \
	EH_IO_TMR12,   \
	EH_IO_TMR6,   \
	EH_IO_TMR7,   \
	EH_IO_TMR15,   \
	EH_IO_TMR16,   \
	EH_IO_TMR17,   \
	EH_IO_TMR18,   \
}
#endif //BUILD_MK64

#ifdef BUILD_SAM3X8E
#define TEPMHA_nChannelFastInfo \
{                               \
	EH_IO_TMR1,               \
	EH_IO_TMR12,              \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_Invalid,            \
	EH_IO_TMR4,               \
	EH_IO_Invalid,            \
	EH_IO_TMR2,               \
	EH_IO_TMR9,               \
	EH_IO_TMR10,              \
	EH_IO_TMR11               \
}
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define TEPMHA_nChannelSlowInfo \
{                               \
	EH_IO_TMR8,               \
	EH_IO_TMR7,               \
	EH_IO_TMR6,               \
	EH_IO_TMR5,               \
	EH_IO_TMR9,               \
	EH_IO_TMR10,              \
	EH_IO_TMR11,              \
	EH_IO_TMR12               \
}
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPMHA_nChannelSlowInfo \
{                               \
	EH_IO_TMR8,               \
	EH_IO_TMR7,               \
	EH_IO_TMR6,               \
	EH_IO_TMR5,               \
	EH_IO_TMR9,               \
	EH_IO_TMR10,              \
	EH_IO_TMR11,              \
	EH_IO_TMR12               \
}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define TEPMHA_nChannelSlowInfo \
{                               \
	{EH_IO_TMR8},               \
	{EH_IO_TMR7},               \
	{EH_IO_TMR6}                \
} 
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define TEPMHA_nMasterInfo    \
{                             \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM3},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM0},            \
	{EH_VIO_FTM1},            \
	{EH_VIO_FTM1},            \
	{EH_VIO_FTM1},            \
	{EH_VIO_FTM2},            \
	{EH_VIO_FTM2},            \
	{EH_VIO_FTM2},            \
	{EH_VIO_FTM2}             \
}	
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPMHA_nMasterInfo  \
{                   \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM2},  \
	{EH_VIO_FTM2},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3}  \
}
#endif

#ifdef BUILD_SAM3X8E
#define TEPMHA_nMasterInfo   \
{                            \
	{EH_VIO_TC0},            \
	{EH_VIO_TC2},            \
	{EH_VIO_TC0},            \
	{EH_VIO_TC2},            \
	{EH_VIO_TC0},            \
	{EH_VIO_TC0},            \
	{EH_VIO_TC0},            \
	{EH_VIO_TC0},            \
	{EH_VIO_TC2},            \
	{EH_VIO_TC2},            \
	{EH_VIO_TC2},            \
	{EH_VIO_TC0}             \
}
#endif //BUILD_SAM3X8E

#if defined(BUILD_MK60) || defined (BUILD_MK64)
#define TEPMHA_xRequestPortClock(x)                                                            \
if ((FTM0 == x) && (0 == (TEPMHA_u32PortClockRequested & 0x1))){SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_FTM0_MASK);TEPMHA_u32PortClockRequested |= 1;} \
if ((FTM1 == x) && (0 == (TEPMHA_u32PortClockRequested & 0x2))){SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_FTM1_MASK);TEPMHA_u32PortClockRequested |= 2;} \
if ((FTM2 == x) && (0 == (TEPMHA_u32PortClockRequested & 0x4))){SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_FTM2_MASK);TEPMHA_u32PortClockRequested |= 4;} \
if ((FTM3 == x) && (0 == (TEPMHA_u32PortClockRequested & 0x8))){SIM_vSetReg32(SIM_SCGC3, SIM_SCGC3_FTM3_MASK);TEPMHA_u32PortClockRequested |= 8;}
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
#define TEPMHA_xRequestTimerClock(x)                       \
if (TC0 == x)                                              \
{                                                          \
    (void)SIM_boEnablePeripheralClock(TC0_IRQn);           \
    (void)SIM_boEnablePeripheralClock(TC1_IRQn);           \
    (void)SIM_boEnablePeripheralClock(TC2_IRQn);           \
}                                                          \
if (TC1 == x)                                              \
{                                                          \
	(void)SIM_boEnablePeripheralClock(TC3_IRQn);           \
	(void)SIM_boEnablePeripheralClock(TC4_IRQn);           \
	(void)SIM_boEnablePeripheralClock(TC5_IRQn);           \
}                                                          \
if (TC2 == x)                                              \
{                                                          \
	(void)SIM_boEnablePeripheralClock(TC6_IRQn);           \
	(void)SIM_boEnablePeripheralClock(TC7_IRQn);           \
	(void)SIM_boEnablePeripheralClock(TC8_IRQn);           \
}

#define TEPMHA_xRequestPWMClock(x)                         \
if (PWM == x)                                              \
{                                                          \
	(void)SIM_boEnablePeripheralClock(PWM_IRQn);           \
}
#endif //BUILD_SAM3X8E

#if defined(BUILD_MK60) || defined (BUILD_MK64)
#define TEPMHA_xInitInterrupts(x)                \
if (FTM0 == x){TEPM_vInitInterrupts(FTM0_IRQn);} \
if (FTM1 == x){TEPM_vInitInterrupts(FTM1_IRQn);} \
if (FTM2 == x){TEPM_vInitInterrupts(FTM2_IRQn);} \
if (FTM3 == x){TEPM_vInitInterrupts(FTM3_IRQn);} 
#endif //BUILD_MK6X


void TEPMHA_vStart(puint32 const pu32Arg);
void TEPMHA_vRun(puint32 const pu32Arg);
void TEPMHA_vTerminate(puint32 const pu32Arg);
SYSAPI_tenSVCResult TEPMHA_vInitTEPMResource(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMResourceCB*);
void TEPMHA_vInitTEPMChannel(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMChannelCB*);
void TEPHAM_boGetMasterEHIOResourceList(IOAPI_tenEHIOResource, IOAPI_tenEHIOResource*, puint32);
void TEPMHA_vAppendTEPMQueue(IOAPI_tenEHIOResource, TEPMAPI_tstTimedUserEvent*, TEPMAPI_ttEventCount);
void TEPMHA_vConfigureKernelTEPMInput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedKernelEvent*);
void TEPMHA_vConfigureKernelTEPMOutput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedKernelEvent*, TEPMAPI_ttEventCount);
void TEPMHA_vConfigureUserTEPMInput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedUserEvent*);
void TEPMHA_vInitiateUserCallBack(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
uint32 TEPMHA_u32GetFTMTableIndex(IOAPI_tenEHIOResource);
void TEPMHA_vStartEventProgramKernelQueues(void);
void TEPMHA_vSynchroniseEventProgramKernelQueues(void);
void TEPMHA_vForceQueueTerminate(void*, uint32, uint32);
bool TEPMHA_boFlagIsSet(void*, uint32, puint32, uint32, uint32);
bool TEMPHA_boInterruptEnabled(void*, uint32);
TEPMAPI_ttEventTime TEPMHA_tGetScheduledVal(void*, uint32, bool, uint32);
void TEMPHA_vResetTimerFlag(void*, uint32);
void TEPMHA_vGetFreeVal(void*, puint32);
uint32 TEPMHA_u32GetFreeVal(void*, uint32);
void TEPMHA_vDisconnectEnable(void*, uint32);
void* TEPMHA_pvGetModuleFromEnum(TEPMHA_tenTimerModule);
void TEPMHA_vCapComAction(TEPMAPI_tenAction, void*, uint32, uint32, TEPMAPI_ttEventTime);
IOAPI_tenTriState TEPMHA_enGetTimerDigitalState(IOAPI_tenEHIOResource);
void* TEPMHA_pvGetTimerModuleFromVIO(IOAPI_tenEHIOResource);
uint32 TEPMHA_u32GetTimerChannelsPerInterruptGroup(void* pvModule);
uint32 TEPMHA_u32GetTimerStartChannelInterruptGroup(IOAPI_tenEHIOResource);
uint32 TEPMHA_u32GetTimerHardwareSubChannel(uint32);
uint32 TEPMHA_u32GetTimerHardwareChannel(IOAPI_tenEHIOResource);
TEPMHA_tenTimerModule TEPMHA_enTimerEnumFromModule(tstTimerModule*);
IOAPI_tenEHIOResource TEPMHA_enGetTimerResourceFromVIOAndIndex(IOAPI_tenEHIOResource, uint32);
TEPMAPI_ttEventTime TEPMHA_u32GetTimerVal(IOAPI_tenEHIOResource);
bool TEPMHA_boCheckFalseAlarm(void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX);
uint32 TEPMHA_u32GetModulePhaseCorrect(TEPMHA_tenTimerModule enTimerModule, uint32 u32ChannelIDX);

#endif //TEPMHA_H
