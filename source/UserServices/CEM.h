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
#ifndef CEM_H
#define CEM_H

#include "types.h"
#include "kernelio.h"
#include "IOAPI.h"
#include "TEPM.h"
#include "TEPMAPI.h"
#include "declarations.h"

#define CEM_xEdgesCount (CEM_u8FallingEdgesCount + CEM_u8RisingEdgesCount)
#define CEM_xAllEdgesCount (CEM_u8FallingEdgesCount + CEM_u8RisingEdgesCount + CEM_u8MissingToothCountMax)
#define CEM_nEdgesMax   80u
#define CEM_nSeqGroupCount 2
#define CEM_nSeqcount 4
#define CEM_nPeriodSamples 8

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _CEM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

typedef enum
{
   CEM_enAutocorrelationMode,
   CEM_enOneGroupMissing,
   CEM_enTypeSuzukiM15A,
   CEM_enTypeWVEA888,
   CEM_enToyota2NFE,
   CEM_enModeCount
} CEM_tenTriggerType;

typedef enum
{
	CEM_enStartGap,
	CEM_enEndGap,
	CEM_enSameGap
} CEM_enGapStatus;

EXTERN uint32 CEM_u32GlobalCycleFraction;
EXTERN uint8 CEM_u32SyncPoints;
EXTERN uint8 CEM_u8PhaseRepeats;
EXTERN uint32 CEM_u32GlobalCycleTime;
EXTERN uint32 CEM_u32GlobalCycleOriginCount;
EXTERN TEPMAPI_ttEventTime CEM_tEventTimeLast;
EXTERN TEPMAPI_ttEventTime CEM_tToothEventTimeLast;
EXTERN TEPMAPI_ttEventTime CEM_tSyncTimeLast;
EXTERN uint32 CEM_u32SequenceFraction;
EXTERN uint32 CEM_u32CrankNoiseCounts;
//ASAM mode=readvalue name="Crank Noise Counts" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="counts" format=5.3 help="Crank Noise Counts"
EXTERN uint32 CEM_u32CrankErrorCounts;
//ASAM mode=readvalue name="Crank Error Counts" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="counts" format=5.3 help="Crank Error Counts"
EXTERN uint32 CEM_u32CamErrorCounts;
//ASAM mode=readvalue name="Cam Error Counts" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="counts" format=5.3 help="Cam Error Counts"
EXTERN uint32 CEM_u32CrankRunningErrorCounts;
//ASAMREMOVED!! mode=readvalue name="Crank Running Error Counts" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="counts" format=5.3 help="Crank Running Error Counts"
EXTERN uint32 CEM_u32CamRunningErrorCounts;
//ASAM mode=readvalue name="Cam Running Error Counts" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="counts" format=5.3 help="Cam Running Error Counts"

void CEM_vStart(uint32* const);
void CEM_vRun(uint32* const);
void CEM_vTerminate(uint32* const);
bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray, const bool boFirstRising, IOAPI_tenEdgePolarity enEdgePolarity, uint32_t u32TriggerType);
bool CEM_boPopulateSyncPointsArray(puint16 const);
void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void CEM_vPhaseEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
TEPMAPI_ttEventTime CEM_ttGetModulePhase(uint32 u32ChannelIDX);
void CEM_vFreqEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void CEM_vSetupSimpleCamSync(IOAPI_tenEHIOResource, bool, uint32);
void CEM_vSetSyncPhaseRepeats(uint32 u32SyncPhaseRepeats);
uint32 CEM_u32GetAllEdgesCount(void);

#endif //CEM_H

