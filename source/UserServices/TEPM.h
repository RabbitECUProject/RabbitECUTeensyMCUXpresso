/******************************************************************************/
/* TEPM header file                                                           */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef TEPM_H
#define TEPM_H

#include "IOAPI.h"
#include "TEPMAPI.h"
#include "SYSAPI.h"
#include "TEPMHA.h"

#define TEPM_nSoonCounts  80
#define TEPM_nSoonCountsLoose  250
#define TEPM_nFarCounts   0xe0000000

#define TEPM_SPEED
#define TEPM_SPEED_MK6X
#define TEPM_PRIO_INPUT_MK6X
#define TEPM_REENTRANCY_OFF
#define TEPM_REENTRANCY_HW_OFF

#ifdef BUILD_SPARKDOG_PF
#define TEPM_PRIO_RESOURCE EH_IO_TMR11
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
#define TEPM_PRIO_RESOURCE EH_IO_TMR10
#endif //BUILD_SPARKDOG_TEENSY_ADAPT

#ifdef BUILD_MK60
#define TEPM_nEventChannels	24u
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPM_nEventChannels	18u
#endif //BUILD_MK64

#define TEPM_nEventsMax			30u
#define TEPM_nMastersMax		1u


typedef enum
{
	FTM_enFTM0,
	FTM_enFTM1,
	FTM_enFTM2,
	FTM_enFTM3,
	FTM_enFTMModuleCount
} FTM_tenFTMModule;	

#ifdef BUILD_MK60
#define TEPM_nChannelInfo               \
{                                       \
	{EH_IO_TMR1, FTM_enFTM0, 7u, 4u},	\
	{EH_IO_TMR2, FTM_enFTM0, 6u, 4u},	\
	{EH_IO_TMR3, FTM_enFTM0, 5u, 4u},	\
	{EH_IO_TMR4, FTM_enFTM0, 4u, 4u},	\
	{EH_IO_TMR5, FTM_enFTM3, 3u, 4u},	\
	{EH_IO_TMR6, FTM_enFTM3, 2u, 4u},	\
	{EH_IO_TMR7, FTM_enFTM3, 1u, 4u},	\
	{EH_IO_TMR8, FTM_enFTM3, 0u, 4u},	\
	{EH_IO_TMR9, FTM_enFTM3, 4u, 6u},	\
	{EH_IO_TMR10, FTM_enFTM3, 5u, 6u},	\
	{EH_IO_TMR11, FTM_enFTM3, 6u, 6u},	\
	{EH_IO_TMR12, FTM_enFTM3, 7u, 6u},	\
	{EH_IO_TMR13, FTM_enFTM0, 3u, 4u},	\
	{EH_IO_TMR14, FTM_enFTM0, 2u, 4u},	\
	{EH_IO_TMR15, FTM_enFTM0, 1u, 4u},	\
	{EH_IO_TMR16, FTM_enFTM0, 0u, 4u},	\
	{EH_IO_ADSE4, FTM_enFTM1, 0u, 6u},  \
	{EH_IO_GPSE9, FTM_enFTM1, 0u, 3u},  \
	{EH_IO_GPSE8, FTM_enFTM1, 1u, 3u},  \
	{EH_IO_ADSE5, FTM_enFTM2, 0u, 6u},  \
	{EH_IO_GPSE7, FTM_enFTM2, 0u, 3u},  \
	{EH_IO_CAN1T, FTM_enFTM2, 0u, 3u},  \
	{EH_IO_CAN1R, FTM_enFTM2, 1u, 3u}   \
}
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPM_nChannelInfo               \
{                                       \
	{EH_IO_TMR1, FTM_enFTM3, 0u, 0u, 3u},	\
	{EH_IO_TMR2, FTM_enFTM0, 0u, 0u, 3u},	\
	{EH_IO_TMR3, FTM_enFTM0, 1u, 0u, 3u},	\
	{EH_IO_TMR4, FTM_enFTM0, 1u, 0u, 3u},	\
	{EH_IO_TMR5, FTM_enFTM3, 4u, 0u, 3u},	\
	{EH_IO_TMR6, FTM_enFTM3, 2u, 0u, 3u},	\
	{EH_IO_TMR7, FTM_enFTM3, 3u, 0u, 3u},	\
	{EH_IO_TMR8, FTM_enFTM0, 2u, 0u, 3u},	\
	{EH_IO_TMR9, FTM_enFTM0, 3u, 0u, 3u},	\
	{EH_IO_TMR10, FTM_enFTM0, 5u, 0u, 3u},	\
	{EH_IO_TMR11, FTM_enFTM0, 6u, 0u, 3u},	\
	{EH_IO_TMR12, FTM_enFTM3, 1u, 0u, 3u},	\
	{EH_IO_TMR13, FTM_enFTM2, 0u, 0u, 3u},	\
	{EH_IO_TMR14, FTM_enFTM2, 1u, 0u, 3u},	\
	{EH_IO_TMR15, FTM_enFTM3, 4u, 0u, 2u},	\
	{EH_IO_TMR16, FTM_enFTM3, 5u, 0u, 2u},	\
	{EH_IO_TMR17, FTM_enFTM3, 6u, 0u, 2u},  \
	{EH_IO_TMR18, FTM_enFTM3, 7u, 0u, 2u},  \
}	
#endif //BUILD_MK60

#ifdef BUILD_MK60
#define TEPM_nChannelFastInfo  \
{                   \
	{EH_IO_TMR16},  \
	{EH_IO_TMR15},  \
	{EH_IO_TMR14},  \
	{EH_IO_TMR13},  \
	{EH_IO_TMR4},   \
	{EH_IO_TMR3},   \
	{EH_IO_TMR2},   \
	{EH_IO_TMR1}    \
}
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define TEPM_nChannelFastInfo  \
{                   \
	{EH_IO_TMR2},    \
	{EH_IO_TMR3},    \
	{EH_IO_TMR8},    \
	{EH_IO_TMR9},    \
	{EH_IO_TMR5},    \
	{EH_IO_TMR10},   \
	{EH_IO_TMR11},   \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_TMR13},   \
	{EH_IO_TMR14},   \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_Invalid}, \
	{EH_IO_TMR1},   \
	{EH_IO_TMR12},   \
	{EH_IO_TMR6},   \
	{EH_IO_TMR7},   \
	{EH_IO_TMR15},   \
	{EH_IO_TMR16},   \
	{EH_IO_TMR17},   \
	{EH_IO_TMR18},   \
}
#endif //BUILD_MK64

#ifdef BUILD_MK60
#define TEPM_nMasterInfo  \
{                   \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM3},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM0},  \
	{EH_VIO_FTM1},  \
	{EH_VIO_FTM1},  \
	{EH_VIO_FTM1},  \
	{EH_VIO_FTM2},  \
	{EH_VIO_FTM2},  \
	{EH_VIO_FTM2},  \
	{EH_VIO_FTM2}   \
}	
#endif

#ifdef BUILD_MK64
#define TEPM_nMasterInfo  \
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


#define TEPM_xInitInterrupts(x)									\
if (FTM0 == x){TEPM_vInitInterrupts(FTM0_IRQn);}	\
if (FTM1 == x){TEPM_vInitInterrupts(FTM1_IRQn);}	\
if (FTM2 == x){TEPM_vInitInterrupts(FTM2_IRQn);}	\
if (FTM3 == x){TEPM_vInitInterrupts(FTM3_IRQn);}	\

typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	TEPMAPI_ttEventTime tEventTime;
} TEPM_tstTEPMResult;

typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	TEPMHA_tenTimerModule enModule;
	TEPMHA_tenModuleType enModuleType;
	uint32 u32Channel;
	uint32 u32SubChannel;
	uint32 u32MuxSel;
	IRQn_Type enIRQType;
} TEPM_tstTEPMChannel;

typedef struct
{
	TEPMHA_tenTimerModule enModule;
	uint32 u32Channel;
	IOAPI_tenEHIOResource enEHIOResource;
} TEPM_tstTEPMReverseChannel;

typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource[TEPMHA_nMastersMax];
} TEPM_tstMasters;


void TEPM_vStart(puint32 const pu32Arg);
void TEPM_vRun(puint32 const pu32Arg);
void TEPM_vAsyncRequest(void);
IOAPI_tenEHIOResource TEPM_enGetPrimaryLinkedResource(void);
void TEMP_vTerminate(puint32 const pu32Arg);
SYSAPI_tenSVCResult TEPM_vInitTEPMResource(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMResourceCB*);
uint32 TEPM_u32InitTEPMChannel(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMChannelCB*, bool boPWMMode);
void TEPM_boGetMasterEHIOResourceList(IOAPI_tenEHIOResource, IOAPI_tenEHIOResource*, puint32);
void TEPM_vAppendTEPMQueue(IOAPI_tenEHIOResource, TEPMAPI_tstTimedUserEvent*, TEPMAPI_ttEventCount);
void TEPM_vConfigureKernelTEPMInput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedKernelEvent*);
void TEPM_vConfigureKernelTEPMOutput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedKernelEvent*, TEPMAPI_ttEventCount);
void TEPM_vConfigureUserTEPMInput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedUserEvent*);
void TEPM_vInitiateUserCallBack(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void TEPM_vGetTimerVal(IOAPI_tenEHIOResource, puint32);
void TEPM_vInterruptHandler(IOAPI_tenEHIOResource, void*);
void TEPM_vMissingToothInterruptHandler(IOAPI_tenEHIOResource, void*);
uint32 TEPM_u32GetFTMTableIndex(IOAPI_tenEHIOResource);
void TEPM_vStartEventProgramKernelQueues(bool, uint32);
void TEPM_vRunEventToothProgramKernelQueues(bool, uint32, uint32);
void TEPM_vSynchroniseEventProgramKernelQueues(void);
IOAPI_tenTriState TEPM_enGetTimerDigitalState(IOAPI_tenEHIOResource);
void TEPM_vEnableSequences(bool boEnable);
uint32 TEPM_u32GetTimerVal(IOAPI_tenEHIOResource, void*);
void TEPM_vSetFuelCutsMask(uint32, uint32, uint32);
void TEPM_vSetSparkCutsMask(uint32, uint32, uint32);
void TEPM_vConfigureMissingToothInterrupt(void);
void TEPM_vSetNextMissingToothInterrupt(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime, uint32);
#endif // TEPM_H

