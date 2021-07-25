/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TEPM                                                   */
/* DESCRIPTION:        Timed Event Programs module provides services for the  */
/*                     output of timed pulse-trains                           */
/* FILE NAME:          TEPM.c                                                 */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _TEPM_C

#include <string.h>
#include "build.h"
#include "CEM.h"
#include "client.h"
#include "CPUAbstract.h"
#include "CQUEUE.h"
#include "declarations.h"
#include "stddef.h"
#include "types.h"
#include "IRQ.h"
#include "TEPM.h"
#include "TEPMHA.h"
#include "TEPMAPI.h"
#include "MSG.h"
#include "SIM.h"
#include "REGSET.h"
#include "RESM.h"
#include "IOAPI.h"
#include "SYSAPI.h"


//matthew to remove
#include "PIMAPI.h"
#include "PIM.h"


//#define DEBUG_TEPM

/* Private data declarations
   -------------------------*/
TEPMAPI_tstTimedUserEvent TEPM_aastTimedUserEvents[TEPMHA_nEventChannels][TEPMHA_nUserEventsMax];
TEPMAPI_tstTimedKernelEvent TEPM_aastTimedKernelEvents[TEPMHA_nEventChannels][TEPMHA_nKernelEventsMax];
const TEPM_tstTEPMChannel TEPM_rastTEPMChannel[] = TEPMHA_nChannelInfo;
const IOAPI_tenEHIOResource TEPM_rastTEPMFastChannel[] = TEPMHA_nChannelFastInfo;
const TEPM_tstMasters TEPM_rastMasters[] = TEPMHA_nMasterInfo;
CQUEUE_tstQueue TEPM_astProgramKernelQueue[TEPMHA_nEventChannels];
CQUEUE_tstQueue TEPM_astProgramUserQueue[TEPMHA_nEventChannels];
TEPMAPI_tpfEventCB TEPM_atpfEventKernelCB[TEPMHA_nEventChannels];
TEPMAPI_tpfEventCB TEPM_atpfEventUserCB[TEPMHA_nEventChannels];
TEPM_tstTEPMResult TEPM_astEventResult[TEPMHA_nEventChannels];
MSG_tstMBX* TEPM_apstMBX[TEPMHA_nEventChannels];
bool TEPM_aboTEPMChannelModeInput[TEPMHA_nEventChannels];
bool TEPM_aboTEPMChannelModeOutput[TEPMHA_nEventChannels];
bool TEPM_aboTEPMChannelModeToothScheduled[TEPMHA_nEventChannels];
bool TEPM_aboTEPMChannelModeRecursive[TEPMHA_nEventChannels];
bool TEPM_au32TEPMChannelRecursionCount[TEPMHA_nEventChannels];
bool TEPM_aboTEPMChannelModePWM[TEPMHA_nEventChannels];
bool TEPM_aboTEPMChannelAsyncRequestEnable[TEPMHA_nEventChannels];
uint32 TEPM_au32TEPMChannelSequence[TEPMHA_nEventChannels];
bool TEPM_aboSynchroniseEnable[TEPMHA_nEventChannels];
volatile bool TEPM_aboQueueOverflow[TEPMHA_nEventChannels];
EXTERN uint32 CAM_u32RPMRaw;	
EXTERN IOAPI_tenEdgePolarity CEM_enEdgePolarity;
EXTERN uint32 CEM_u32ToothEdgeCounter;
EXTERN uint32 CEM_u32CycleToothEdgeCounter;
EXTERN uint8 CEM_u8PhaseRepeats;
EXTERN CEM_tenTriggerType CEM_enTriggerType;
EXTERN bool CEM_boProceed;
bool TEPM_boDisableSequences;
IOAPI_tenEHIOResource TEPM_astTEPMVVTInputs[4];
IOAPI_tenEHIOResource TEPM_astTEPMLinkedResource[TEPMHA_nEventChannels];
uint32 TEPM_u32PrimaryPhaseIDX;
uint16 TEPM_au16PWMLowResidual[16];
uint32 TEPM_u32FuelCutsPercent;
uint32 TEPM_u32SparkCutsPercent;
uint32 TEPM_u32FuelCutsMask;
uint32 TEPM_u32SparkCutsMask;
uint32 TEPM_u32FuelChannelCount;
uint32 TEPM_u32SparkChannelCount;
uint32 TEPM_u32FuelCutsCounter;
uint32 TEPM_u32StartLinkPending;

#ifdef TEPM_PRIO_INPUT_MK6X
extern const TEPM_tstTEPMReverseChannel TEPMHA_rastTEPMReverseChannel[];
#endif //TEPM_PRIO_INPUT_MK6X

#define TEPM_nTableCount sizeof(TEPM_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/
#ifdef TEPM_USER_MODE
static void TEPM_vRunEventProgramUserQueue(void*, uint32, uint32);
#endif //TEPM_USER_MODE

static void TEPM_vFirstStartLinkToothFractions(uint32, uint32, uint32);
static void TEPM_vRunEventProgramKernelQueue(void*, uint32, uint32, uint32, bool);	 
static void* TEPM_pvGetModule(IOAPI_tenEHIOResource);	 
static void* TEPM_pstGetModuleFromEnum(TEPMHA_tenTimerModule);
static uint32 TEPM_u32GetTimerHardwareChannel(IOAPI_tenEHIOResource);
static uint32 TEPM_u32GetToothFractionTime(TEPMAPI_ttEventTime, uint32);

/* Public function definitions
   ---------------------------*/	 
void TEPM_vStart(puint32 const pu32Arg)
{
	uint32 u32QueueIDX;
	
	for (u32QueueIDX = 0; u32QueueIDX < TEPMHA_nEventChannels; u32QueueIDX++)
	{
		CQUEUE_xInit(TEPM_astProgramUserQueue + u32QueueIDX, 
			TEPMHA_nUserEventsMax, (uint32)TEPM_aastTimedUserEvents + u32QueueIDX * TEPMHA_nUserEventsMax * sizeof(TEPMAPI_tstTimedUserEvent));	
		
		CQUEUE_xInit(TEPM_astProgramKernelQueue + u32QueueIDX, 
			TEPMHA_nKernelEventsMax, (uint32)TEPM_aastTimedKernelEvents + u32QueueIDX * TEPMHA_nUserEventsMax * sizeof(TEPMAPI_tstTimedKernelEvent));	
		
		TEPM_atpfEventKernelCB[u32QueueIDX] = NULL;
		TEPM_atpfEventUserCB[u32QueueIDX] = NULL;		
		TEPM_apstMBX[u32QueueIDX] = NULL;
		TEPM_aboTEPMChannelModeInput[u32QueueIDX] = FALSE;
		TEPM_aboTEPMChannelModeOutput[u32QueueIDX] = FALSE;
		TEPM_aboTEPMChannelAsyncRequestEnable[u32QueueIDX] = FALSE;
		TEPM_au32TEPMChannelSequence[u32QueueIDX] = ~0;
		TEPM_boDisableSequences = FALSE;
		TEPM_astTEPMLinkedResource[u32QueueIDX] = EH_IO_Invalid;
	}

	TEPM_u32FuelCutsPercent = 5;
}

IOAPI_tenEHIOResource TEPM_enGetPrimaryLinkedResource(void)
{
	return TEPM_astTEPMLinkedResource[TEPM_u32PrimaryPhaseIDX];
}

void TEPM_vAsyncRequest(void)
{
	CEM_u32GlobalCycleFraction = 0x0000;
	TEPM_vStartEventProgramKernelQueues(TRUE, 0);
	CEM_u32GlobalCycleFraction += (0x10000 / CEM_u32SyncPoints);
	TEPM_vStartEventProgramKernelQueues(TRUE, 1);

	CEM_u32GlobalCycleFraction = 0x10000;
	TEPM_vStartEventProgramKernelQueues(TRUE, 4);
	CEM_u32GlobalCycleFraction += (0x10000 / CEM_u32SyncPoints);
	TEPM_vStartEventProgramKernelQueues(TRUE, 5);
}

void TEPM_vEnableSequences(bool boEnable)
{
	TEPM_boDisableSequences = ~boEnable;
}

void TEPM_vRun(puint32 const pu32Arg)
{
	/*
	static uint32 u32Count;
	uint32 u32TimerChannelIDX;
	void* pvModule;
	const IOAPI_tenEHIOResource raenResourceList[] = TEPMHA_nChannelResourceList;
	uint32 u32ChannelIDX;
    IOAPI_tenEHIOResource enEHIOResource;
	TEPMAPI_ttEventTime tEventTimeScheduled;
	
	u32Count++;
	
	if (0 == (u32Count % 50))
	{
		for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
		{
			if (TRUE == TEPM_aboTEPMChannelModePWM[u32TimerChannelIDX])
			{
				enEHIOResource = raenResourceList[u32TimerChannelIDX];
				uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
				pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
				u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);

				CPU_xEnterCritical();
				tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 50;
				tEventTimeScheduled &= TEPMHA_nCounterMask;
				TEPMHA_vCapComAction(TEPMAPI_enSetHigh, pvModule, u32ChannelIDX, 0, tEventTimeScheduled);
				CPU_xExitCritical();
			}
		}
	}

	if (1 == (u32Count % 50))
	{
		for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
		{
			if (TRUE == TEPM_aboTEPMChannelModePWM[u32TimerChannelIDX])
			{
				enEHIOResource = raenResourceList[u32TimerChannelIDX];
				uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
				pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
				u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);

				CPU_xEnterCritical();
				tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
				tEventTimeScheduled += ((*TEPM_aastTimedKernelEvents[u32TableIDX][0].ptEventTime));
				tEventTimeScheduled &= TEPMHA_nCounterMask;
				TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, 0, tEventTimeScheduled);
				CPU_xExitCritical();
			}
		}
	}
	*/
}

void TEPM_vTerminate(puint32 const pu32Arg)
{
	
	
}

uint32 TEPM_u32InitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB, bool boPWMMode)
{
	uint32 u32TableIDX;
	MSG_tenMBXErr enMBXErr;

	TEPMHA_vInitTEPMChannel(enEHIOResource, pstTEPMChannelCB);
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	TEPM_astTEPMLinkedResource[u32TableIDX] = pstTEPMChannelCB->enLinkedResource;

	if ((pstTEPMChannelCB->enAction == TEPMAPI_enCapAny) |
	    (pstTEPMChannelCB->enAction == TEPMAPI_enCapRising) |
	    (pstTEPMChannelCB->enAction == TEPMAPI_enCapFalling))
	{
	    TEPM_aboTEPMChannelModeInput[u32TableIDX] = TRUE;
	    TEPM_aboTEPMChannelModeOutput[u32TableIDX] = FALSE;
	    TEPM_aboTEPMChannelModeRecursive[u32TableIDX] = pstTEPMChannelCB->boRecursive;
	}
	else
	{
	    TEPM_aboTEPMChannelModeOutput[u32TableIDX] = TRUE;	
	    TEPM_aboTEPMChannelModeInput[u32TableIDX] = FALSE;
	    TEPM_aboTEPMChannelModePWM[u32TableIDX] = boPWMMode;
	    TEPM_aboTEPMChannelModeRecursive[u32TableIDX] = FALSE;
    }

	if (0x10000000 <= pstTEPMChannelCB->u32Sequence)
	{
		/* Special case for VVT input MS 8bit = 0x10 */
		TEPM_au32TEPMChannelSequence[u32TableIDX] = pstTEPMChannelCB->u32Sequence;
	}
	else if (0x10000 <= pstTEPMChannelCB->u32Sequence)
	{
		/* Special case for fuel pressure control solenoid MS 16bit != 0*/
		TEPM_au32TEPMChannelSequence[u32TableIDX] = (pstTEPMChannelCB->u32Sequence & 0xf) +
				((pstTEPMChannelCB->u32Sequence & 0xf00) >> 4) +
				((pstTEPMChannelCB->u32Sequence & 0xf0000) >> 8) +
				((pstTEPMChannelCB->u32Sequence & 0xf000000) >> 12) +
				0x04000000;
	}
	else if ((0x00000080 <= pstTEPMChannelCB->u32Sequence) &&
			(0x000000FF >= pstTEPMChannelCB->u32Sequence))
	{
		/* Special case for fuel pressure control solenoid Golf MK6 16bit != 0*/
		TEPM_au32TEPMChannelSequence[u32TableIDX] = (pstTEPMChannelCB->u32Sequence & 0xff) +
				0x08000000;
	}
	else
	{
		TEPM_au32TEPMChannelSequence[u32TableIDX] = pstTEPMChannelCB->u32Sequence & 0xffff;

		if (((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0x0ff) == 0x0ff) ||
			((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff00) == 0xff00))
		{
			TEPM_au32TEPMChannelSequence[u32TableIDX] |= 0x02000000;
		}
		else
		{
			TEPM_au32TEPMChannelSequence[u32TableIDX] |= 0x01000000;
		}
	}


	TEPM_aboTEPMChannelAsyncRequestEnable[u32TableIDX] = pstTEPMChannelCB->boAsyncRequestEnable;

	/* Check if a mailbox is allocated for this table index */
	if (NULL == TEPM_apstMBX[u32TableIDX])
	{
		enMBXErr = MSG_enMBXNew((MSG_tstMBX**)&TEPM_apstMBX[u32TableIDX], 
					MSG_enTEPMEvent);
	}		
	
	/* TODO suppress warning */
	(void)enMBXErr;

	return TEPM_rastTEPMChannel[u32TableIDX].u32MuxSel;
}


SYSAPI_tenSVCResult TEPM_vInitTEPMResource(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMResourceCB* pstTEPMResourceCB)
{
	return TEPMHA_vInitTEPMResource(enEHIOResource, pstTEPMResourceCB);
}

void TEPM_boGetMasterEHIOResourceList(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOResource* penEHIOResource, puint32 pu32MasterCount)
{
	uint32 u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	uint32 u32MasterIDX = 0;
	
	while ((RESM_nMastersMax > *pu32MasterCount) && (TEPMHA_nMastersMax > u32MasterIDX))
	{
		if (NULL != TEPM_rastMasters[u32ChannelIDX].enEHIOResource[u32MasterIDX])
		{
			*penEHIOResource = TEPM_rastMasters[u32ChannelIDX].enEHIOResource[u32MasterIDX];
			penEHIOResource++; 
			(*pu32MasterCount)++; 
			u32MasterIDX++;
		}
	}
}

void TEPM_vAppendTEPMQueue(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTimedUserEvent* pstTimedEvents, TEPMAPI_ttEventCount tEventCount)
{
	uint32 u32TableIDX;

	CPU_xEnterCritical();
	
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	
	while ((0 < tEventCount--)
			&& (FALSE == CQUEUE_xIsFull(TEPM_astProgramUserQueue + u32TableIDX)))
	{
		memcpy((void*)&TEPM_aastTimedUserEvents[u32TableIDX][TEPM_astProgramUserQueue[u32TableIDX].u32Tail],
						 (void*)pstTimedEvents++, sizeof(TEPMAPI_tstTimedUserEvent));
		CQUEUE_xAddItem(TEPM_astProgramUserQueue + u32TableIDX);		
	}	
	
	CPU_xExitCritical();
}

void TEPM_vConfigureUserTEPMInput(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTimedUserEvent* pstTimedEvent)
{
	uint32 u32TableIDX;
	
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	
	switch (pstTimedEvent->enMethod)
	{
		case TEPMAPI_enLinkPrimaryProgram:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPrimaryEventCB;	
			TEPM_u32PrimaryPhaseIDX = u32TableIDX;
			break;			
		}
		case TEPMAPI_enLinkVVT1Input:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPhaseEventCB;
			TEPM_astTEPMVVTInputs[0] = enEHIOResource;
			break;
		}
		case TEPMAPI_enLinkVVT2Input:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPhaseEventCB;
			TEPM_astTEPMVVTInputs[1] = enEHIOResource;
			break;
		}
		case TEPMAPI_enLinkVVT3Input:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPhaseEventCB;
			TEPM_astTEPMVVTInputs[2] = enEHIOResource;
			break;
		}
		case TEPMAPI_enLinkVVT4Input:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPhaseEventCB;	
			TEPM_astTEPMVVTInputs[3] = enEHIOResource;
			break;			
		}	
		case TEPMAPI_enLinkFreqInput:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = & CEM_vFreqEventCB;	
			break;	
		}
		default:
		{
			break;
		}
	}		
		
	TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;	
	
	if (FALSE == CQUEUE_xIsFull(TEPM_astProgramUserQueue + u32TableIDX))
	{
		memcpy((void*)&TEPM_aastTimedUserEvents[u32TableIDX][TEPM_astProgramUserQueue[u32TableIDX].u32Tail],
						 (void*)pstTimedEvent, sizeof(TEPMAPI_tstTimedUserEvent));
		CQUEUE_xAddItem(TEPM_astProgramUserQueue + u32TableIDX);		
	}				
}

void TEPM_vConfigureKernelTEPMOutput(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTimedKernelEvent* pstTimedEvents, TEPMAPI_ttEventCount tEventCount)
{
	uint32 u32TableIDX;
	
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
		
	TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvents->pfEventCB;
	
	if ((CEM_enAutocorrelationMode != CEM_enTriggerType) &&
			(CEM_enTypeSuzukiM15A != CEM_enTriggerType))
	{
		TEPM_aboTEPMChannelModeToothScheduled[u32TableIDX] = pstTimedEvents->boToothScheduled;

		if (0 != pstTimedEvents->boToothScheduled)
		{
			TEPM_u32StartLinkPending |= (1 << u32TableIDX);
		}
	}

	/* Reset the queue count it is not possible to append a Kernel output queue */
	CQUEUE_xClearCount(TEPM_astProgramKernelQueue + u32TableIDX);
	
	while ((0 < tEventCount--)
			&& (FALSE == CQUEUE_xIsFull(TEPM_astProgramKernelQueue + u32TableIDX)))
	{
		if ((CEM_enAutocorrelationMode == CEM_enTriggerType) ||
				(CEM_enTypeSuzukiM15A == CEM_enTriggerType))
		{
			/* Schedule to the tooth not supported for these crank sensor configurations */
			pstTimedEvents->boToothScheduled = FALSE;
		}

		memcpy((void*)&TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Tail],
						 (void*)pstTimedEvents++, sizeof(TEPMAPI_tstTimedKernelEvent));
		CQUEUE_xAddItem(TEPM_astProgramKernelQueue + u32TableIDX);		
	}			
}

void TEPM_vInitiateUserCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{		
	MSG_tstMBX* pstMBX;
	tpfClientCB pfClientCB;	
	CLIENT_tenErr enErr;	
	uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);		
	
	/* Store the programmed event resource for the user callback */
	TEPM_astEventResult[u32TableIDX].enEHIOResource = enEHIOResource;	
	
	/* Store the timer result for the user callback */	
	TEPM_astEventResult[u32TableIDX].tEventTime = tEventTime;	
	
	pfClientCB = (tpfClientCB)TEPM_atpfEventUserCB[u32TableIDX];							
	pstMBX = TEPM_apstMBX[u32TableIDX];									
	MSG_vMBXPost(pstMBX, (void*)&TEPM_astEventResult[u32TableIDX]);
							
	if ((NULL != pfClientCB) && (MSG_boMBXValid(pstMBX)))
	{
		enErr = CLIENT_enEnqueueCB(pstMBX, pfClientCB);
	}	
	
	/* TODO suppress warning */
	(void)enErr;
}


void TEPM_u32GetFreeVal(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32Data)
{
	void* pvModule = TEPM_pvGetModule(enEHIOResource);

    TEPMHA_vGetFreeVal(pvModule, pu32Data);
}

void TEPM_vInterruptHandler(IOAPI_tenEHIOResource enEHVIOResource, void* pvData)
{
	uint32 u32ChannelIDX;
	uint32 u32TableIDX;
	uint32 u32Flags = 0;
	uint32 u32Temp;
	TEPMAPI_tpfEventCB pfTEPMEventCB;
	uint32 u32StartChannelIDX = 0;
	uint32 u32EndChannelIDX;
    IOAPI_tenEHIOResource enEHIOResource;
	static uint32 u32FalseAlarmCount;
	TEPMAPI_ttEventTime tEventTimeScheduled;

#ifdef TEPM_PRIO_INPUT_MK6X
	static uint32 u32PrioChannelIDX = TEPMHA_nPRIOCHANNEL;
#endif //TEPM_PRIO_INPUT_MK6X


	/* Note here the resource passed in is the timer module resource not the timer channel */
	void* pvModule = TEPMHA_pvGetTimerModuleFromVIO(enEHVIOResource);

#ifndef TEPM_SPEED_MK6X
	u32StartChannelIDX = 0;
#endif //TEPM_SPEED_MK6X
	u32EndChannelIDX = u32StartChannelIDX + TEPMHA_u32GetTimerChannelsPerInterruptGroup(pvModule);
	
	/* Loop through the channels contained within this interrupt group */
	for (u32ChannelIDX = u32StartChannelIDX; u32ChannelIDX < u32EndChannelIDX; u32ChannelIDX++)
	{
#ifdef TEPM_SPEED_MK6X
		if (FTM_CnSC_CHF_MASK == (FTM_CnSC_CHF_MASK & ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC))
#else
	    if (true == TEPMHA_boFlagIsSet(pvModule, u32ChannelIDX, &u32Flags, u32Sequence[u32Prio], u32Prio))
#endif  //TEPM_SPEED_MK6X
		{
#ifdef TEPM_SPEED_MK6X
		    if (FTM_CnSC_CHIE_MASK == (FTM_CnSC_CHIE_MASK & ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC))
#else
		    if (true == TEMPHA_boInterruptEnabled(pvModule, u32ChannelIDX))
#endif //TEPM_SPEED_MK6X
			{
#ifndef TEPM_SPEED_MK6X
			enEHIOResource = TEPMHA_enGetTimerResourceFromVIOAndIndex(enEHVIOResource, u32ChannelIDX);
			u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
#else
			u32Temp = 8 * ((uint32)(enEHVIOResource - EH_VIO_FTM0)) + u32ChannelIDX;
			enEHIOResource = TEPMHA_rastTEPMReverseChannel[u32Temp].enEHIOResource;
			u32TableIDX = enEHIOResource - EH_IO_TMR1;
#endif //TEPM_SPEED_MK6X
				
				if (TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX])
				{
					if (TRUE == TEPM_aboTEPMChannelModePWM[u32TableIDX])
					{
						pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
						u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);

						if (0 == TEPM_au16PWMLowResidual[u32TableIDX])
						{
							tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0);
							u32Temp = (*TEPM_aastTimedKernelEvents[u32TableIDX][0].ptEventTime);
							u32Temp *= TEPM_aastTimedKernelEvents[u32TableIDX][0].tAccumulate;
							u32Temp /= 0x10000;
							tEventTimeScheduled += u32Temp;
							tEventTimeScheduled &= TEPMHA_nCounterMask;
							TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, 0, tEventTimeScheduled);
							TEPM_au16PWMLowResidual[u32TableIDX] = TEPM_aastTimedKernelEvents[u32TableIDX][0].tAccumulate - u32Temp;
						}
						else
						{
							tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0);
							tEventTimeScheduled += TEPM_au16PWMLowResidual[u32TableIDX];
							tEventTimeScheduled &= TEPMHA_nCounterMask;
							TEPMHA_vCapComAction(TEPMAPI_enSetHigh, pvModule, u32ChannelIDX, 0, tEventTimeScheduled);
							TEPM_au16PWMLowResidual[u32TableIDX] = 0;
						}
					}
					else
					{
						if (TRUE == TEPM_aboQueueOverflow[u32TableIDX])
						{
							TEPM_aboQueueOverflow[u32TableIDX] = FALSE;
							CQUEUE_xSetHead(TEPM_astProgramKernelQueue + u32TableIDX, 0);
							TEPM_vStartEventProgramKernelQueues(FALSE, TEPM_au32TEPMChannelSequence[u32TableIDX]);
						}
						else if (FALSE == TEPMHA_boCheckFalseAlarm(pvModule, u32ChannelIDX, TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX)))
						{
							if (TRUE == CEM_boProceed)
							{
								TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX,
									TEPM_au32TEPMChannelSequence[u32TableIDX], false);
								TEPM_vInitiateUserCallBack(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
							}
						}
						else
						{
							if (TRUE == CEM_boProceed)
							{
								u32FalseAlarmCount++;
							}
						}
					}
				}			
				else if (TRUE == TEPM_aboTEPMChannelModeInput[u32TableIDX])
				{
					pfTEPMEventCB = TEPM_atpfEventKernelCB[u32TableIDX];
						
					if (NULL != pfTEPMEventCB)
					{
#if defined (TEPM_REENTRANCY)
						IOAPI_tenTriState enPrioTriState;
						IOAPI_tenTriState enPrioTriStateOld;
						bool boHWReentrancy = FALSE;
						bool boHWReentrancyFlag = FALSE;

						if (TRUE == TEPM_aboTEPMChannelModeRecursive[u32TableIDX])
						{

							if (0 == TEPM_au32TEPMChannelRecursionCount[u32TableIDX])
							{
								/* Let's go again */
								TEMPHA_vResetTimerFlag(pvModule, u32ChannelIDX);
								TEPM_au32TEPMChannelRecursionCount[u32TableIDX]++;
								pfTEPMEventCB(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
							}
							else
							{
								TEPM_au32TEPMChannelRecursionCount[u32TableIDX]++;
							}

							if (0 != TEPM_au32TEPMChannelRecursionCount[u32TableIDX])
							{
								TEPM_au32TEPMChannelRecursionCount[u32TableIDX]--;
							}
						}
						else
#endif //TEPM_REENTRANCY
						{
							pfTEPMEventCB(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
#ifdef TEPM_REENTRANCY_HW
							IOAPI_tenTriState enPrioTriState;
							IOAPI_tenTriState enPrioTriStateOld;
							bool boHWReentrancy = FALSE;
							bool boHWReentrancyFlag = FALSE;

							if (u32PrioChannelIDX == u32ChannelIDX)
							{
								if (IOAPI_enEdgeFalling == CEM_enEdgePolarity)
								{
									/* But HW inverted */
									enPrioTriStateOld = IOAPI_enHigh;
									enPrioTriState = TEPM_enGetTimerDigitalState(TEPM_PRIO_RESOURCE);
								}
								else
								{
									/* But HW inverted */
									enPrioTriStateOld = IOAPI_enLow;
									enPrioTriState = TEPM_enGetTimerDigitalState(TEPM_PRIO_RESOURCE);
								}

								boHWReentrancy = TRUE;
								boHWReentrancyFlag = FALSE;
							}
#endif //TEPM_REENTRANCY_HW
						}
					}
				}
            }
			
			TEMPHA_vResetTimerFlag(pvModule, u32ChannelIDX);
		}

#ifdef TEPM_PRIO_INPUT_MK6X

#ifdef TEPM_SPEED_MK6X
		if ((u32PrioChannelIDX < u32EndChannelIDX) &&
				(FTM_CnSC_CHIE_MASK == (FTM_CnSC_CHIE_MASK & ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC)))
#else
		if ((u32PrioChannelIDX < u32EndChannelIDX) && (true == TEMPHA_boInterruptEnabled(pvModule, u32PrioChannelIDX)))
#endif //TEPM_SPEED_MK6X
		{
#ifndef TEPM_SPEED_MK6X
			enEHIOResource = TEPMHA_enGetTimerResourceFromVIOAndIndex(enEHVIOResource, u32PrioChannelIDX);
			u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
#else
			u32Temp = 8 * ((uint32)(enEHVIOResource - EH_VIO_FTM0)) + u32PrioChannelIDX;
			enEHIOResource = TEPMHA_rastTEPMReverseChannel[u32Temp].enEHIOResource;
			u32TableIDX = enEHIOResource - EH_IO_TMR1;
#endif //TEPM_SPEED_MK6X

			if (TRUE == TEPM_aboTEPMChannelModeInput[u32TableIDX])
			{
#if defined (TEPM_REENTRANCY_HW)
				if (TRUE == boHWReentrancy)
				{
					/* Sanity check edge polarity */
					if (IOAPI_enEdgeFalling == CEM_enEdgePolarity)
					{
						enPrioTriState = TEPM_enGetTimerDigitalState(enEHIOResource);

						/* Invert the polarity because the circuit inverts */
						if (enPrioTriState != enPrioTriStateOld)
						{
							if (IOAPI_enHigh == enPrioTriState)
							{
								boHWReentrancyFlag = TRUE;
							}
						}

						enPrioTriStateOld = enPrioTriState;//move up
					}
				}
#endif //TEPM_RENTRANCY_HW

				if (FTM_CnSC_CHF_MASK == (FTM_CnSC_CHF_MASK & ((tstTimerModule*)pvModule)->CONTROLS[u32PrioChannelIDX].CnSC))
				{
					pfTEPMEventCB = TEPM_atpfEventKernelCB[u32TableIDX];

#if defined (TEPM_REENTRANCY)
					if (IOAPI_enEdgeFalling == CEM_enEdgePolarity)
					{
						/* But HW inverted */
						enPrioTriStateOld = IOAPI_enHigh;
						enPrioTriState = TEPM_enGetTimerDigitalState(TEPM_PRIO_RESOURCE);
					}
					else
					{
						/* But HW inverted */
						enPrioTriStateOld = IOAPI_enLow;
						enPrioTriState = TEPM_enGetTimerDigitalState(TEPM_PRIO_RESOURCE);
					}

					boHWReentrancy = TRUE;
					boHWReentrancyFlag = FALSE;
#endif //TEPM_REENTRANCY

					if (NULL != pfTEPMEventCB)
					{
#if defined (TEPM_REENTRANCY)
						if (TRUE == TEPM_aboTEPMChannelModeRecursive[u32TableIDX])
						{

							if (0 == TEPM_au32TEPMChannelRecursionCount[u32TableIDX])
							{
								/* Let's go again */
								TEMPHA_vResetTimerFlag(pvModule, u32PrioChannelIDX);
								TEPM_au32TEPMChannelRecursionCount[u32TableIDX]++;
								pfTEPMEventCB(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32PrioChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
							}
							else
							{
								TEPM_au32TEPMChannelRecursionCount[u32TableIDX]++;
							}

							if (0 != TEPM_au32TEPMChannelRecursionCount[u32TableIDX])
							{
								TEPM_au32TEPMChannelRecursionCount[u32TableIDX]--;
							}
						}
						else
#endif //TEPM_REENTRANCY
						{
#if defined (TEPM_REENTRANCY_HW)
							//boHWReentrancy = FALSE;
#endif //TEPM_REENTRANCY_HW
							pfTEPMEventCB(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32PrioChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
						}
					}

					TEMPHA_vResetTimerFlag(pvModule, u32PrioChannelIDX);
				}
#if defined (TEPM_REENTRANCY_HW)
				else if (TRUE == boHWReentrancyFlag)
				{
					pfTEPMEventCB = TEPM_atpfEventKernelCB[u32TableIDX];

					if (NULL != pfTEPMEventCB)
					{
						boHWReentrancyFlag = FALSE;
						pfTEPMEventCB(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32PrioChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
					}
				}
#endif //TEPM_REENTRANCY_HW
			}
		}
#endif //TEPM_PRIO_INPUT_MK6X
	}
}

void TEPM_vMissingToothInterruptHandler(IOAPI_tenEHIOResource enEHVIOResource, void* pvData)
{
	static bool flag;
	uint32 u32LastGap;

	u32LastGap = TEPMHA_u32SetNextMissingToothInterrupt(0, 0, 0);

	CEM_tToothEventTimeLast += u32LastGap;
	CEM_tToothEventTimeLast &= TEPMHA_nCounterMask;

	TEPM_vRunEventToothProgramKernelQueues(FALSE, CEM_u32CycleToothEdgeCounter, u32LastGap);
	CEM_u32ToothEdgeCounter++;
	CEM_u32CycleToothEdgeCounter++;

	if (flag)
	{
		PIM_vAssertPortBit(PIMAPI_enPHYS_PORT_E, 0x400, IOAPI_enLow);
	}
	else
	{
		PIM_vAssertPortBit(PIMAPI_enPHYS_PORT_E, 0x400, IOAPI_enHigh);
	}

	flag = !flag;

}

IOAPI_tenTriState TEPM_enGetTimerDigitalState(IOAPI_tenEHIOResource enEHIOResource)
{
    return TEPMHA_enGetTimerDigitalState(enEHIOResource);
}

/* Private function definitions
   ---------------------------*/

#ifdef TEPM_USER_MODE
static void TEPM_vRunEventProgramUserQueue(void* pvModule, uint32 u32ChannelIDX, uint32 u32TableIDX)
{
	TEPMAPI_tstTimedUserEvent* pstTimedEvent;
	TEPMAPI_ttEventTime tEventTimeScheduled;
	TEPMAPI_ttEventTime tEventTimeRemains;	
	
	if (FALSE == (CQUEUE_xIsEmpty(TEPM_astProgramUserQueue + u32TableIDX)))
	{
		pstTimedEvent = &TEPM_aastTimedUserEvents[u32TableIDX][TEPM_astProgramUserQueue[u32TableIDX].u32Head];		
		
		switch (pstTimedEvent->enMethod)
		{
			case TEPMAPI_enHardAlarm:
			{
				if (0x7ff0 > pstTimedEvent->tEventTime)
				{
					tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + pstTimedEvent->tEventTime;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					if ((0x10 > tEventTimeRemains) || (0x8000 < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
					}
				}
				else
				{
					tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + pstTimedEvent->tEventTime;			
				}
				break;
			}				
			case TEPMAPI_enHardLinkedTimeStep:
			{		
				if (0x7ff0 > pstTimedEvent->tEventTime)
				{
					tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + pstTimedEvent->tEventTime;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					if ((0x10 > tEventTimeRemains) || (0x8000 < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
					}
				}
				else
				{
					tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + pstTimedEvent->tEventTime;			
				}
				break;
			}
			default:
			{
				break;
			}
		}		

		//TEPMHA_vCapComAction(pstTimedEvent->enAction, pvModule, u32ChannelIDX, 0, tEventTimeScheduled);				
		TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;
		CQUEUE_xRemoveItem(TEPM_astProgramUserQueue + u32TableIDX);
	}
	else
	{
#ifdef TEPM_REMOVE_RESIDUAL_INTERRUPTS
		/* The queue is empty therefore this was a next window interrupt 
		   so go ahead and disable interrupts */
		TEPMHA_vDisconnectEnable(pvModule, u32ChannelIDX);
#endif		
	}
}
#endif //TEPM_USER_MODE


void TEPM_vStartEventProgramKernelQueues(bool boAsyncRequest, uint32 u32SequenceIDX)
{
	IOAPI_tenEHIOResource enEHIOResource;		
	void* pvModule;
	uint32 u32ChannelIDX;
	uint32 u32SubChannelIDX;
	const IOAPI_tenEHIOResource raenResourceList[] = TEPMHA_nChannelResourceList;
	uint32 u32TimerChannelIDX;
	uint32 u32TableIDX;
	static uint32 u32SyncRPMLimit = 6000;
	bool boSyncProceed = false;

	if (TRUE == boAsyncRequest)
	{
		CEM_u32GlobalCycleTime = 0x8000;
	}
	
	for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
	{
		enEHIOResource = raenResourceList[u32TimerChannelIDX];

#ifndef TEPM_SPEED_MK6X
		u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
#else
		u32TableIDX = enEHIOResource - EH_IO_TMR1;
#endif //TEPM_SPEED_MK6X
		
		if ((TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX]) &&
			(FALSE == TEPM_aboTEPMChannelModePWM[u32TableIDX]))
		{
			if (0x04 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
			{
				if ((u32SequenceIDX == (TEPM_au32TEPMChannelSequence[u32TableIDX] & 0x0f)) ||
				 (u32SequenceIDX == ((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xf0) >> 4)) ||
				 (u32SequenceIDX == ((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xf00) >> 8)) ||
				 (u32SequenceIDX == ((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xf000) >> 12)))
				{
					boSyncProceed = true;
				}
			}
			else if (0x08 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
			{
				if (u32SequenceIDX == (TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff))
				{
					boSyncProceed = true;
				}
			}
			else
			{
				if ((u32SequenceIDX == (TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff)) ||
				 (u32SequenceIDX == ((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff00) >> 8)))
				{
					boSyncProceed = true;
				}
			}
		}

		if (true == boSyncProceed)
		{
			TEPM_aboSynchroniseEnable[u32TimerChannelIDX] = u32SyncRPMLimit < CAM_u32RPMRaw ? FALSE : TRUE;

			if (((TRUE == TEPM_aboTEPMChannelAsyncRequestEnable[u32TableIDX]) && (TRUE == boAsyncRequest)) ||
				(FALSE == boAsyncRequest))
			{
				//if ((TRUE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
				//		(TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)))
				/* EMPTY TEST MAYBE NOT REQUIRED */
				if ((TRUE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
                    (TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)) &&
                    (FALSE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX)))
				{
					pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
					u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	
					u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);
			
					/* Queue didn't end normally last cycle */			
					TEPMHA_vForceQueueTerminate(pvModule, u32ChannelIDX, u32SubChannelIDX);				
					CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);	
					//TEPM_aboQueueOverflow[u32TableIDX] = TRUE;
				}										

				if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
						TRUE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX))
				{
					/* This queue is populated and head is at zero (static head) */
					pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
					u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	

					TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, 
						u32SequenceIDX, false);

					TEPM_aboQueueOverflow[u32TimerChannelIDX] = FALSE;
				}
			}

			boSyncProceed = false;
		}
	}
}

void TEPM_vRunEventToothProgramKernelQueues(bool boAsyncRequest, uint32 u32ToothCount, uint32 u32ToothTime)
{
	IOAPI_tenEHIOResource enEHIOResource;
	void* pvModule;
	uint32 u32ChannelIDX;
	uint32 u32SubChannelIDX;
	const IOAPI_tenEHIOResource raenResourceList[] = TEPMHA_nChannelResourceList;
	uint32 u32TimerChannelIDX;
	uint32 u32TableIDX;
	TEPMAPI_tstTimedKernelEvent* pstToothTimedEvent;
	TEPMAPI_ttEventTime tEventTimeScheduled;
	uint32 u32Temp;
	uint32 u32ModulePhaseCorrect;

	if (TRUE == boAsyncRequest)
	{
		CEM_u32GlobalCycleTime = 0x8000;
	}

	for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
	{
		enEHIOResource = raenResourceList[u32TimerChannelIDX];

#ifndef TEPM_SPEED_MK6X
		u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
#else
		u32TableIDX = enEHIOResource - EH_IO_TMR1;
#endif //TEPM_SPEED_MK6X

		if (TRUE == TEPM_aboTEPMChannelModeToothScheduled[u32TableIDX])
		{
			pstToothTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][0];

			if (TRUE == pstToothTimedEvent->boToothScheduled)
			{
				if (((pstToothTimedEvent->tFractionalEventTime >> 16) == u32ToothCount) &&
						(0 != (pstToothTimedEvent->tFractionalEventTime & 0xffff)))
				{
					if ((TRUE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
						(TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)) &&
						(FALSE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX)))
					{
						pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
						u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);
						u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);

						/* Queue didn't end normally last cycle */
						TEPMHA_vForceQueueTerminate(pvModule, u32ChannelIDX, u32SubChannelIDX);
						CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);
					}

					if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
							TRUE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX))
					{
						/* This queue is populated and head is at zero (static head) */
						pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
						u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);

						u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);

						/* Timed event pointer is NULL for a failed synchronise */
						switch (pstToothTimedEvent->enMethod)
						{
							case TEPMAPI_enGlobalLinkedFraction:
							{
								u32Temp = (pstToothTimedEvent->tFractionalEventTime & 0xffff) * u32ToothTime;
								u32Temp /= 0x10000;
								u32Temp += u32ToothTime;
								u32ModulePhaseCorrect = TEPMHA_u32GetModulePhaseCorrect(TEPMHA_enTimerEnumFromModule(pvModule), u32ChannelIDX);
								tEventTimeScheduled = u32Temp + CEM_tToothEventTimeLast + u32ModulePhaseCorrect;
								tEventTimeScheduled &= TEPMHA_nCounterMask;

								TEPMHA_vCapComAction(pstToothTimedEvent->enAction, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);
							}
							default:
							{
								break;
							}
						}

						CQUEUE_xRemoveItem(TEPM_astProgramKernelQueue + u32TableIDX);
					}

					TEPM_aboQueueOverflow[u32TimerChannelIDX] = FALSE;
				}
			}

			pstToothTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][1];

			if (TRUE == pstToothTimedEvent->boToothScheduled)
			{
				if (((pstToothTimedEvent->tFractionalEventTime >> 16) == u32ToothCount) &&
						(0 != (pstToothTimedEvent->tFractionalEventTime & 0xffff)))
				{
					pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
					u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);

					u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);

					/* Timed event pointer is NULL for a failed synchronise */
					switch (pstToothTimedEvent->enMethod)
					{
						case TEPMAPI_enGlobalLinkedFraction:
						{
							u32Temp = (pstToothTimedEvent->tFractionalEventTime & 0xffff) * u32ToothTime;
							u32Temp /= 0x10000;
							u32Temp += u32ToothTime;
							u32ModulePhaseCorrect = TEPMHA_u32GetModulePhaseCorrect(TEPMHA_enTimerEnumFromModule(pvModule), u32ChannelIDX);
							tEventTimeScheduled = u32Temp + CEM_tToothEventTimeLast + u32ModulePhaseCorrect;
							tEventTimeScheduled &= TEPMHA_nCounterMask;

							TEPMHA_vCapComAction(pstToothTimedEvent->enAction, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);
						}
						default:
						{
							break;
						}
					}

					CQUEUE_xRemoveItem(TEPM_astProgramKernelQueue + u32TableIDX);
					TEPM_aboQueueOverflow[u32TimerChannelIDX] = FALSE;
				}
			}
		}
	}
}


void TEPM_vGetTimerVal(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32Val)
{
	*pu32Val = TEPMHA_u32GetTimerVal(enEHIOResource);
}

void TEPM_vSynchroniseEventProgramKernelQueues(void)
{
	IOAPI_tenEHIOResource enEHIOResource;		
	void* pvModule;
	const IOAPI_tenEHIOResource raenResourceList[] = TEPMHA_nChannelResourceList;
	uint32 u32ChannelIDX;
	uint32 u32TimerChannelIDX;
	
	for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
	{
		enEHIOResource = raenResourceList[u32TimerChannelIDX];
		uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

		if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
			(FALSE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX)) &&
			(TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX]))
		{
			/* If not at static head which means queue is done and reset */
			pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
			u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	

			TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, 
				TEPM_au32TEPMChannelSequence[u32TableIDX], true);
		}
	}
}


static void TEPM_vRunEventProgramKernelQueue(void* pvModule, uint32 u32ChannelIDX, uint32 u32TableIDX, uint32 u32SequenceIDX, bool boSynchroniseUpdate)
{
	TEPMAPI_tstTimedKernelEvent* pstTimedEvent = NULL;
	TEPMAPI_tstTimedKernelEvent* pstToothTimedEvent;
	TEPMAPI_tstTimedKernelEvent* pstToothSecondTimedEvent;
	TEPMAPI_ttEventTime tEventTimeScheduled;
	TEPMAPI_ttEventTime tEventTimeRemains;	
	uint32 u32Temp;
	uint32 u32EdgeCount;
	uint32 u32ToothDelta;
	uint32 u32ModulePhaseCorrect = 0;
	uint32 u32SubChannelIDX;
	volatile bool boSynchroniseAbort = FALSE;
	uint32 u32GlobalSequenceFraction;
	uint32 u32EventTime;
	TEPMAPI_tstTimedKernelEvent stTempKernelEvent;

	/* Mask out the sequence index  alternate origin index and windows span */
	u32SequenceIDX &= 0xff;

#define DEBUG_TEPM_OFF

#ifdef DEBUG_TEPM
	static volatile uint32 test[32];
	static uint32 test_idx;
#endif

	
	if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX))
	{
		if (TRUE == boSynchroniseUpdate)
		{			
			if ((1 == TEPM_astProgramKernelQueue[u32TableIDX].u32Head) &&
					(FALSE == TEPM_aboTEPMChannelModeToothScheduled[u32TableIDX]))
			{
				/* If waiting for first event, let's look at it as long as it isn't a tooth scheduled channel */
				pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head - 1];					
				
				switch (pstTimedEvent->enMethod)
				{
					case TEPMAPI_enGlobalLinkedFraction:
					{
						if (FALSE == TEPM_aboSynchroniseEnable[u32TableIDX])
						{				
							/* Not eligible for a synchronise so abort */
							pstTimedEvent = NULL;
						}							
						break;
					}
					default:
					{
						/* Not eligible for a synchronise so abort */
						pstTimedEvent = NULL;
					}
				}
			}
		}
		else
		{		
			if (FALSE == TEPM_aboTEPMChannelModeToothScheduled[u32TableIDX])
			{
				pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head];
			}
			else
			{
				/* Start schedule to the tooth */
				if (0 == TEPM_astProgramKernelQueue[u32TableIDX].u32Head)
				{
					pstToothTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head];

					if (1 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
					{
						pstToothTimedEvent->tFractionalEventTime = TEPM_u32GetToothFractionTime(*pstToothTimedEvent->ptEventTime, u32SequenceIDX);

						/* Let's go look at second event */
						pstToothSecondTimedEvent = pstToothTimedEvent + 1;

						if (TRUE == pstToothSecondTimedEvent->boToothScheduled)
						{
							pstToothSecondTimedEvent->tFractionalEventTime = TEPM_u32GetToothFractionTime(*pstToothSecondTimedEvent->ptEventTime, u32SequenceIDX);
							u32ToothDelta = pstToothSecondTimedEvent->tFractionalEventTime - pstToothTimedEvent->tFractionalEventTime;

							if (0x20000 >= u32ToothDelta)
							{
								/* a problem - minimum tooth delta of 2 */
								u32EdgeCount = CEM_u32GetAllEdgesCount();

								u32Temp = pstToothSecondTimedEvent->tFractionalEventTime;
								u32Temp /= 0x10000;
								u32Temp -= 2;
								u32Temp = u32Temp % u32EdgeCount;
								u32Temp *= 0x10000;
								pstToothTimedEvent->tFractionalEventTime = u32Temp + (pstToothSecondTimedEvent->tFractionalEventTime & 0xffff);
							}

							TEPM_vFirstStartLinkToothFractions(0, u32TableIDX, pstToothTimedEvent->tFractionalEventTime);
							TEPM_vFirstStartLinkToothFractions(1, u32TableIDX, pstToothSecondTimedEvent->tFractionalEventTime);
						}
					}
					else if (2 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
					{
						pstToothTimedEvent->tFractionalEventTime = TEPM_u32GetToothFractionTime(*pstToothTimedEvent->ptEventTime, u32SequenceIDX);

						/* Let's go look at second event */
						pstToothSecondTimedEvent = pstToothTimedEvent + 1;

						if (TRUE == pstToothSecondTimedEvent->boToothScheduled)
						{
							pstToothSecondTimedEvent->tFractionalEventTime = TEPM_u32GetToothFractionTime(*pstToothSecondTimedEvent->ptEventTime, u32SequenceIDX);
							u32ToothDelta = pstToothSecondTimedEvent->tFractionalEventTime - pstToothTimedEvent->tFractionalEventTime;

							if (0x8000000 < u32ToothDelta)
							{
								/* If wrapped around */
								u32Temp = CEM_u32GetAllEdgesCount();
								u32Temp *= 0x10000;
								u32ToothDelta += u32Temp;
							}

							if (0x20000 >= u32ToothDelta)
							{
								/* a problem - minimum tooth delta of 2 */
								u32EdgeCount = CEM_u32GetAllEdgesCount();
								u32EdgeCount *= CEM_u8PhaseRepeats;

								u32Temp = pstToothSecondTimedEvent->tFractionalEventTime;
								u32Temp /= 0x10000;
								u32Temp -= 2;
								u32Temp = u32Temp % u32EdgeCount;
								u32Temp *= 0x10000;
								pstToothTimedEvent->tFractionalEventTime = u32Temp + (pstToothSecondTimedEvent->tFractionalEventTime & 0xffff);
							}
						}

						TEPM_vFirstStartLinkToothFractions(0, u32TableIDX, pstToothTimedEvent->tFractionalEventTime);
						TEPM_vFirstStartLinkToothFractions(1, u32TableIDX, pstToothSecondTimedEvent->tFractionalEventTime);
					}
				}
				else
				{
					pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head];
				}
			}
		}
		
		if (NULL != pstTimedEvent)
		{
			u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);

			/* Timed event pointer is NULL for a failed synchronise */
			switch (pstTimedEvent->enMethod)
			{
				case TEPMAPI_enGlobalLinkedFraction:
				{				
#ifdef DEBUG_TEPM
					test[test_idx] = CEM_u32GlobalCycleTime;
					test_idx++;

					if (test_idx == 0x20)
					{
						test_idx = 0;
					}
#endif

				    u32ModulePhaseCorrect = TEPMHA_u32GetModulePhaseCorrect(TEPMHA_enTimerEnumFromModule(pvModule), u32ChannelIDX);

					if (1 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
					{
						u32GlobalSequenceFraction = 0xffff & (CEM_u32GlobalCycleFraction - (u32SequenceIDX * 0x10000 / CEM_u32SyncPoints));

						/* Divide global cycle time by 8 for scale again by phase repeats this might be half global window */
						u32Temp = CEM_u32GlobalCycleTime / (TEPMHA_nCounterGlobalDiv * CEM_u8PhaseRepeats);
					}
					else if (2 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
					{
						u32GlobalSequenceFraction = 0xffff & ((CEM_u32GlobalCycleFraction >> 1) - (u32SequenceIDX * 0x8000 / CEM_u32SyncPoints));

						/* Divide global cycle time by TEPMHA_nCounterGlobalDiv for scale */
						u32Temp = CEM_u32GlobalCycleTime / TEPMHA_nCounterGlobalDiv;
					}
					else
					{
						u32GlobalSequenceFraction = 0;

						/* Divide global cycle time by TEPMHA_nCounterGlobalDiv for scale */
						u32Temp = CEM_u32GlobalCycleTime / (2 * TEPMHA_nCounterGlobalDiv * CEM_u8PhaseRepeats);
					}

					if (u32GlobalSequenceFraction < *(pstTimedEvent->ptEventTime))
					{
						if (((0 == u32GlobalSequenceFraction) && (FALSE == boSynchroniseUpdate)) || (TRUE == boSynchroniseUpdate))
						{						
							u32Temp *= (*(pstTimedEvent->ptEventTime) - u32GlobalSequenceFraction);
							u32Temp = u32Temp / (0x10000 / TEPMHA_nCounterGlobalDiv);
							u32Temp = TEPMHA_nLongestDelay < u32Temp ? TEPMHA_nLongestDelay : u32Temp;
										
							tEventTimeScheduled = CEM_tSyncTimeLast + u32ModulePhaseCorrect + u32Temp;
							tEventTimeScheduled &= TEPMHA_nCounterMask;
							tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
							tEventTimeRemains &= TEPMHA_nCounterMask;

							if (FALSE == boSynchroniseUpdate)
							{
								if ((TEPM_nSoonCounts > tEventTimeRemains) || ((TEPMHA_nCounterMask - TEPM_nSoonCounts) < tEventTimeRemains))
								//if (TEPM_nSoonCountsLoose > tEventTimeRemains)
								{
									tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + TEPM_nSoonCountsLoose;
									tEventTimeScheduled &= TEPMHA_nCounterMask;
								}
							}
							else
							{
								if ((TEPM_nSoonCounts > tEventTimeRemains) || ((TEPMHA_nCounterMask - TEPM_nSoonCounts) < tEventTimeRemains))
								{
									boSynchroniseAbort = TRUE;
								}												
							}
						}
						else
						{
							boSynchroniseAbort = TRUE;
						}
					}
					else
					{
                        boSynchroniseAbort = TRUE;
					}

					break;
				}	
				case TEPMAPI_enGlobalLinkedTimeStep:
				{
				    u32ModulePhaseCorrect = TEPMHA_u32GetModulePhaseCorrect(TEPMHA_enTimerEnumFromModule(pvModule), u32ChannelIDX);

					if (FALSE == boSynchroniseUpdate)
					{
						u32Temp = *(pstTimedEvent->ptEventTime);

						tEventTimeScheduled = CEM_tSyncTimeLast + u32ModulePhaseCorrect + u32Temp;
						tEventTimeScheduled &= TEPMHA_nCounterMask;
						tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
						tEventTimeRemains &= TEPMHA_nCounterMask;

						if ((TEPM_nSoonCounts > tEventTimeRemains) || ((TEPMHA_nCounterMask - TEPM_nSoonCounts) < tEventTimeRemains))
						{
							tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + TEPM_nSoonCounts;
							tEventTimeScheduled &= TEPMHA_nCounterMask;
						}
					}
					else
					{
						boSynchroniseAbort = TRUE;
					}

					break;
				}
				case TEPMAPI_enGlobalLinkedFractionStep:
				{					
					u32Temp = CEM_u32GlobalCycleTime / 2;
					u32Temp *= *(pstTimedEvent->ptEventTime);
					u32Temp = MIN(0x7fc00000, u32Temp);	
					u32Temp /= 0x8000;					
					tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + u32Temp;
					tEventTimeScheduled &= TEPMHA_nCounterMask;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					if ((TEPM_nSoonCounts > tEventTimeRemains) || (-TEPM_nSoonCounts < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + TEPM_nSoonCounts;
						tEventTimeScheduled &= TEPMHA_nCounterMask;
					}

					break;
				}	
				case TEPMAPI_enHardLinkedTimeStep:
				{
					if (0 != pstTimedEvent->ptAccumulate)
					{
						if (*(pstTimedEvent->ptEventTime) > (*pstTimedEvent->ptAccumulate - pstTimedEvent->tAccumulate))
						{
							u32EventTime = *pstTimedEvent->ptAccumulate - pstTimedEvent->tAccumulate;
							memcpy(&stTempKernelEvent, pstTimedEvent, sizeof(stTempKernelEvent));
							pstTimedEvent = &stTempKernelEvent;
							pstTimedEvent->enAction = TEPMAPI_enSetLow;
							TEPM_astProgramKernelQueue[u32TableIDX].u32Head = TEPM_astProgramKernelQueue[u32TableIDX].u32Tail - 1;
						}
						else
						{
							u32EventTime = *(pstTimedEvent->ptEventTime);
						}
					}
					else
					{
						u32EventTime = *(pstTimedEvent->ptEventTime);
					}

					if (-TEPM_nSoonCounts > *(pstTimedEvent->ptEventTime))
					{
						tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + u32EventTime;
						tEventTimeScheduled &= TEPMHA_nCounterMask;
						tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					    if ((TEPM_nSoonCounts > tEventTimeRemains) || (-TEPM_nSoonCounts < tEventTimeRemains))
						{
							tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + TEPM_nSoonCounts;
							tEventTimeScheduled &= TEPMHA_nCounterMask;
						}
					}
					else
					{
						tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + u32EventTime;
					}
					break;
				}
				default:
				{
					break;
				}
			}		

			if (FALSE == boSynchroniseAbort)
			{
				if (FALSE == TEPM_boDisableSequences)
				{
					if (TEPMAPI_enSetLow == pstTimedEvent->enAction)
					{
						if (0 != (TEPM_u32FuelCutsMask & MATH_u32IDXToMask(u32TableIDX)))
						{
							TEPM_u32FuelCutsCounter = (TEPM_u32FuelCutsCounter + 1) % 5;

							if (5 > TEPM_u32FuelCutsPercent)
							{
								if (TEPM_u32FuelCutsPercent <= TEPM_u32FuelCutsCounter)
								{
									tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + TEPM_nSoonCountsLoose;
									tEventTimeScheduled &= TEPMHA_nCounterMask;
								}
							}
						}
					}

					TEPMHA_vCapComAction(pstTimedEvent->enAction, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);

					if (EH_IO_Invalid != pstTimedEvent->enEHIOBitMirrorResource)
					{
						if (TEPMAPI_enSetLow == pstTimedEvent->enAction)
						{
							IO_vAssertDIOResource(pstTimedEvent->enEHIOBitMirrorResource, IOAPI_enHigh);
						}
					}
				}
				else
				{
					TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);	
				}
			}
			
			if (FALSE == boSynchroniseUpdate)
			{
				TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;

				if (0 != pstTimedEvent->ptAccumulate)
				{
					/* Accumulate time into next event */
					pstTimedEvent[1].tAccumulate = pstTimedEvent->tAccumulate + *(pstTimedEvent->ptEventTime);
				}

				CQUEUE_xRemoveItem(TEPM_astProgramKernelQueue + u32TableIDX);
			}
		}
	}
	else if (FALSE == boSynchroniseUpdate)
	{
		if (TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX))
		{		
			CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);	
			pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][0];

			if (EH_IO_Invalid != pstTimedEvent->enEHIOBitMirrorResource)
			{
				IO_vAssertDIOResource(pstTimedEvent->enEHIOBitMirrorResource, IOAPI_enLow);
			}
		}

		/* The queue is empty so go ahead and disable interrupts and connection */
		TEPMHA_vDisconnectEnable(pvModule, u32ChannelIDX);
	}
}

static uint32 TEPM_u32GetToothFractionTime(TEPMAPI_ttEventTime tEventTime, uint32 u32SequenceIDX)
{
	uint32 u32CycleToothCount;
	uint32 u32Temp;
	uint16 u16Temp;

	u32CycleToothCount = CEM_u32GetAllEdgesCount();
	u32CycleToothCount *= CEM_u8PhaseRepeats;

	u32Temp = tEventTime * u32CycleToothCount;
	u32Temp += (0x10000 * (u32SequenceIDX * CEM_u32GetAllEdgesCount()) / CEM_u32SyncPoints);
	u16Temp = u32Temp / 0x10000;

	/* Cheeky prior tooth here */
	u16Temp--;
	u16Temp = u16Temp % u32CycleToothCount;

	/* Mask tooth fraction */
	u32Temp &= 0xffff;

	/* Set is active bit */
	u32Temp |= 1;
	u32Temp += (u16Temp << 16);

	return u32Temp;
}

static void* TEPM_pvGetModule(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	void* pvModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPM_pstGetModuleFromEnum(TEPM_rastTEPMChannel[u32ChannelIDX].enModule);
	
	return pvModule;
}

static void* TEPM_pstGetModuleFromEnum(TEPMHA_tenTimerModule enTimerModule)
{
     return TEPMHA_pvGetModuleFromEnum(enTimerModule);
}

static uint32 TEPM_u32GetTimerHardwareChannel(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	return TEPM_rastTEPMChannel[u32ChannelIDX].u32Channel;
}

uint32 TEPM_u32GetFTMTableIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	return TEPMHA_u32GetFTMTableIndex(enEHIOResource);
}

uint32 TEPM_u32GetTimerVal(IOAPI_tenEHIOResource enEHIOResource, void* pvData)
{
	return 0;
}

void TEPM_vSetFuelCutsMask(uint32 u32CutsPercent, uint32 u32CutsMask, uint32 u32ChannelCount)
{
	TEPM_u32FuelCutsPercent = 5 - (u32CutsPercent / 20);
	TEPM_u32FuelCutsMask = u32CutsMask;
	TEPM_u32FuelChannelCount = u32ChannelCount;
}

void TEPM_vSetSparkCutsMask(uint32 u32CutsPercent, uint32 u32CutsMask, uint32 u32ChannelCount)
{
	TEPM_u32SparkCutsPercent = 5 - (u32CutsPercent / 20);
	TEPM_u32SparkCutsMask = u32CutsMask;
	TEPM_u32SparkChannelCount = u32ChannelCount;
}

void TEPM_vConfigureMissingToothInterrupt()
{
	TEPMHA_vConfigureMissingToothInterrupt();
}

void TEPM_vSetNextMissingToothInterrupt(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tLastGap, uint32 u32Repeats)
{
	uint32 u32ModuleDelta;
	uint32 u32TableIDX;
	void* pvModule;

	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);
	u32ModuleDelta = TEPMHA_u32GetFreeVal((void*)FTM1, 0) - TEPMHA_u32GetFreeVal(pvModule, 0);

	(void)TEPMHA_u32SetNextMissingToothInterrupt(CEM_tEventTimeLast + u32ModuleDelta, tLastGap, u32Repeats);
}

static void TEPM_vFirstStartLinkToothFractions(uint32 u32EventIDX, uint32 u32TableIDX, TEPMAPI_ttEventTime tFractionalEventTime)
{

	uint32 u32InputBitMask;
	uint32 u32Temp;
	uint32 u32IDX;
	uint32 u32TempTooth = (tFractionalEventTime & 0xff0000) >> 16;
	uint32 u32Fraction = tFractionalEventTime & 0xffff;
	uint32 u32ToothTotal = CEM_u8PhaseRepeats * CEM_u32GetAllEdgesCount();
	uint32 u32BitMask = 1;

	if (0 != TEPM_u32StartLinkPending)
	{
		/* Get the bit mask of the donor */
		u32InputBitMask = MATH_u32IDXToMask(u32TableIDX);

		/* Get tooth for origin channel */
		u32Temp = u32ToothTotal * (TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff);
		u32Temp /= 8;
		u32Temp = ~u32Temp;
		u32Temp++;
		u32Temp += u32TempTooth;
		u32TempTooth = u32Temp;

		while (0x8000 < u32TempTooth) {u32TempTooth += u32ToothTotal;}
		while (u32ToothTotal <= u32TempTooth) {u32TempTooth -= u32ToothTotal;}

		while (0x10000 > u32BitMask)
		{
			if (0 != (u32BitMask & TEPM_u32StartLinkPending))
			{
				u32IDX = MATH_u32MaskToIDX(u32BitMask);

				u32Temp = u32TempTooth + ((u32ToothTotal * (TEPM_au32TEPMChannelSequence[u32IDX] & 0xff)) / 8);
				u32Temp = u32Temp % u32ToothTotal;
				u32Temp *= 0x10000;
				u32Temp += u32Fraction;

				TEPM_aastTimedKernelEvents[u32IDX][u32EventIDX].tFractionalEventTime = u32Temp;

				if (0 < u32EventIDX)
				{
					TEPM_u32StartLinkPending &= ~ u32BitMask;
				}
			}

			u32BitMask *= 2;
		}
	}
}




