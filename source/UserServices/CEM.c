/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _CEM_C

#include "CEM.h"
#include "declarations.h"
#include "EST.h"//matthew breaking layers
#include "SETUP.h"//matthew breaking layers
#include "os.h"
#include "tepmha.h"
#include "KERNELIO.h"

uint16 CEM_au16RisingCrankEdge[80];
uint16 CEM_au16FallingCrankEdge[80];
uint16 CEM_au16AllEdge[160];
bool CEM_aboSyncEdge[160];
uint16 CEM_au16SyncPoints[36];
uint8 CEM_u8RisingEdgesCount;
uint8 CEM_u8FallingEdgesCount;
uint32 CEM_u32CrankEdgeCounter;
uint32 CEM_u32ToothEdgeCounter;
uint32 CEM_u32CycleToothEdgeCounter;
bool CEM_boCrankEdgeFirstRising;
uint32 CEM_u32CrankEdgeTimeoutCount;
uint8 CEM_au8AutocorrPeak[4];
uint8 CEM_au8ConfirmedAutocorrPeak[4];
IOAPI_tenEdgePolarity CEM_enEdgePolarity;
TEPMAPI_tstTimerMeasurements CEM_stTimerMeasurements[TEPMHA_nEventChannels];
bool CEM_boEdgesReady;
uint32 CEM_au32TimerOffsets[9];
CEM_tenTriggerType CEM_enTriggerType;
uint8 CEM_u8SimpleMissingSync;
uint8 CEM_u8MissingToothCount;
uint8 CEM_u8MissingToothCountMax;
uint32 CEM_u32SmallestToothTime;
TEPMAPI_tstSimpleCamSync stSimpleCamSync;
extern IOAPI_tenEHIOResource TEPM_astTEPMVVTInputs[];
TEPMAPI_ttEventTime CEM_atLastVVTTimer[4];
TEPMAPI_ttEventTime CEM_atLastVVTHighTime[4];
TEPMAPI_ttEventTime CEM_atLastVVTLowTime[4];

static void CEM_vSequenceReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint32 u32OriginGlobalCycleFraction, bool boLatePhase);
static TEPMAPI_ttEventTime CEM_tCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction, bool boGlobalTimeEnable);
static void CEM_vProcessAllEdges(void);
static void CEM_vPhaseError(int);
static bool CEM_boGetLatePhaseSimpleCamSync(void);

void CEM_vStart(uint32* const u32Stat)
{
	CEM_u32GlobalCycleFraction = 0;
	CEM_u32GlobalCycleTime = 0x0000ffff;
	CEM_u32GlobalCycleOriginCount = 0;
	CEM_u32CrankEdgeTimeoutCount = 0;
	CEM_tEventTimeLast = ~0;
	CEM_u32CrankEdgeCounter = 0xff;
	CEM_boEdgesReady = FALSE;
	CTS_boCTSReady = FALSE;
	CEM_u8SimpleMissingSync = 0;
	CEM_u32CrankErrorCounts = 0;
	//CEM_u8PhaseRepeats = 1;//matthew must be dynamic!!!
	
	memset((void*)&CEM_au16RisingCrankEdge, 0, sizeof(CEM_au16RisingCrankEdge)); 
	memset((void*)&CEM_au16FallingCrankEdge, 0, sizeof(CEM_au16FallingCrankEdge));	
	CEM_u8RisingEdgesCount = 0;
	CEM_u8FallingEdgesCount	= 0;
	CEM_u32SyncPoints = 0;
	CEM_boCrankEdgeFirstRising = FALSE;
	CEM_au8AutocorrPeak[0] = 128;
	CEM_au8AutocorrPeak[1] = 128;
	CEM_au8AutocorrPeak[2] = 128;
	CEM_au8AutocorrPeak[3] = 128;
	CEM_au8ConfirmedAutocorrPeak[0] = 128;
	CEM_au8ConfirmedAutocorrPeak[1] = 128;
	CEM_au8ConfirmedAutocorrPeak[2] = 128;
	CEM_au8ConfirmedAutocorrPeak[3] = 128;
}

void CEM_vRun(uint32* const u32Stat)
{
	if (10 < CEM_u32CrankEdgeTimeoutCount)
	{
		CEM_tEventTimeLast = ~0;		
		CEM_u32GlobalCycleOriginCount	= 0;	
	}
	else
	{
		CEM_u32CrankEdgeTimeoutCount++;
	}
}

void CEM_vTerminate(uint32* const u32Stat)
{

}

bool CEM_boPopulateSyncPointsArray(puint16 const pu16EdgeFractionArray)
{
    bool boRetVal = true;

    CEM_u32SyncPoints = 0;

	while (((0 != *(pu16EdgeFractionArray + CEM_u32SyncPoints)) && (36 > CEM_u32SyncPoints)) | (0 == CEM_u32SyncPoints))
	{
		CEM_au16SyncPoints[CEM_u32SyncPoints] = *(pu16EdgeFractionArray + CEM_u32SyncPoints);
		CEM_u32SyncPoints++;
	}

	CEM_u32SequenceFraction = 0x2000ul / CEM_u32SyncPoints;

	return boRetVal;
}


bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray, const bool boFirstRising, IOAPI_tenEdgePolarity enEdgePolarity, uint32_t u32TriggerType)
{
	uint32 u32EdgeCount = 0;
	bool boStat = FALSE;
	uint8 u8EdgeCount = 0;
	CEM_u8RisingEdgesCount = 0;
	CEM_u8FallingEdgesCount = 0;
	
	CEM_boCrankEdgeFirstRising = boFirstRising;
	CEM_enEdgePolarity = enEdgePolarity;

	while (((0 != *(pu16EdgeFractionArray + u32EdgeCount)) && (80 > u8EdgeCount)) | (0 == u32EdgeCount))
	{
		if (IOAPI_enEdgeBoth == enEdgePolarity)
		{
	        if (TRUE == boFirstRising)
		    {
		        CEM_au16RisingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			else
		    {
			    CEM_au16FallingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
			u32EdgeCount++;

	        if (TRUE == boFirstRising)
		    {
			    CEM_au16FallingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			else
		    {
			    CEM_au16RisingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
			u32EdgeCount++;	
			CEM_u8RisingEdgesCount++;
			CEM_u8FallingEdgesCount++;
		}
		if (IOAPI_enEdgeFalling == enEdgePolarity)
		{
		    CEM_au16FallingCrankEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    u32EdgeCount++;	
		    CEM_u8FallingEdgesCount++;
		}
		if (IOAPI_enEdgeRising == enEdgePolarity)
		{
		    CEM_au16RisingCrankEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    u32EdgeCount++;	
		    CEM_u8RisingEdgesCount++;
		}
	}

	CEM_enTriggerType = (0 != u32TriggerType) ? CEM_enTypeSuzukiM15A + u32TriggerType - 1 : 0;
		
	CEM_vProcessAllEdges();
	boStat = TRUE;
	CEM_boEdgesReady = TRUE;
		
	return boStat;
}

static void CEM_vProcessAllEdges(void)
{
    uint32 u32OldData;
	uint32 u32DeltaCount = 0;
    sint32 s32Temp;

    for (uint32 u32ArrayIDX = 0; u32ArrayIDX < (CEM_xEdgesCount - 1); u32ArrayIDX++)
	{
        u32OldData = CEM_au16AllEdge[u32ArrayIDX];
	    CEM_au16AllEdge[u32ArrayIDX] = CEM_au16AllEdge[u32ArrayIDX + 1] - u32OldData;
	}

	CEM_au16AllEdge[CEM_xEdgesCount - 1] = ~CEM_au16AllEdge[CEM_xEdgesCount - 1] + 1;

	CEM_aboSyncEdge[0] = TRUE;

    for (uint32 u32ArrayIDX = 1; u32ArrayIDX < CEM_xEdgesCount; u32ArrayIDX++)
	{
	    s32Temp = CEM_au16AllEdge[u32ArrayIDX] - CEM_au16AllEdge[u32ArrayIDX - 1];

        if (1 < s32Temp)
        {
            u32DeltaCount++;
			CEM_u8SimpleMissingSync = (u32ArrayIDX + 1) % CEM_xEdgesCount;
			CEM_u8MissingToothCountMax = (CEM_au16AllEdge[u32ArrayIDX] + 10) / CEM_au16AllEdge[u32ArrayIDX - 1];
			CEM_u8MissingToothCountMax--;
        }

		CEM_aboSyncEdge[u32ArrayIDX] = TRUE;
	}

	if (CEM_enTriggerType < CEM_enTypeSuzukiM15A)
	{
		CEM_enTriggerType = (1 == u32DeltaCount) && (8 < CEM_xEdgesCount) ? CEM_enOneGroupMissing : CEM_enAutocorrelationMode;
	}

	if (CEM_enTriggerType == CEM_enOneGroupMissing)
	{
		CEM_aboSyncEdge[CEM_u8SimpleMissingSync] = FALSE;
		CEM_aboSyncEdge[CEM_u8SimpleMissingSync - 1] = FALSE;
	}
}

void CEM_vSetSyncPhaseRepeats(uint32 u32SyncPhaseRepeats)
{
    CEM_u8PhaseRepeats = (uint8)u32SyncPhaseRepeats;
}


TEPMAPI_ttEventTime CEM_ttGetModulePhase(uint32 u32ChannelIDX)
{
    TEPMAPI_ttEventTime tPhaseOffset = 0;

	tPhaseOffset = CEM_au32TimerOffsets[u32ChannelIDX];

    return  tPhaseOffset;
}

void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint16 u16LastGapFraction;
	TEPMAPI_ttEventTime tLastGapTime;
	static uint32 au32AutoCorr[CEM_nEdgesMax];
	static uint32 au32InputArray[CEM_nEdgesMax];
	static bool aboConfirmedFlags[4] = {0, 0, 0, 0};
	static uint8 au8ErrorCount[4];
	uint8 u8PhasePrev;
	uint32 u32Temp;
	static uint32 u32EdgeCount = 0;
	bool boPhaseLocked = FALSE;
	static bool boLatePhase;
	static uint8 u8SameGapCount;

	if (FALSE == CEM_boEdgesReady) return;

	u8PhasePrev = CEM_u32CrankEdgeCounter;

	/* Sanity check the edges count */
	if (CEM_xEdgesCount > CEM_nEdgesMax) return;

	u32EdgeCount++;

#ifdef CEM_CHECK_POL
	/* Sanity check edge polarity */
	if (IOAPI_enEdgeFalling == CEM_enEdgePolarity)
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);

		/* Invert the polarity because the circuit inverts */
	    if (IOAPI_enLow == enTriState)
		{
	    	u32EdgePolAbortCount++;
			return;
		}
	}
	else if (IOAPI_enEdgeRising == CEM_enEdgePolarity)
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);

		/* Invert the polarity because the circuit inverts */
	    if (IOAPI_enHigh == enTriState)
		{
	    	u32EdgePolAbortCount++;
			return;
		}
	}
#endif //CEM_CHECK_POL

#ifdef BUILD_SAM3X8E
    CEM_au32TimerOffsets[8] = ((tstTimerModule*)TC2)->TC_CHANNEL[2].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[7] = ((tstTimerModule*)TC2)->TC_CHANNEL[1].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[6] = ((tstTimerModule*)TC2)->TC_CHANNEL[0].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[5] = ((tstTimerModule*)TC1)->TC_CHANNEL[2].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[4] = ((tstTimerModule*)TC1)->TC_CHANNEL[1].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[3] = ((tstTimerModule*)TC1)->TC_CHANNEL[0].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[2] = ((tstTimerModule*)TC0)->TC_CHANNEL[2].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[1] = ((tstTimerModule*)TC0)->TC_CHANNEL[1].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[0] = 0;
#endif //BUILD_SAM3X8E

	/* Increment the confirmed channels */
	uint32 u32PhaseMin = 0;
	uint32 u32PhaseMax = (CEM_xEdgesCount / CEM_u8PhaseRepeats) - 1;
	

	if	(CEM_enAutocorrelationMode == CEM_enTriggerType)
	{
		for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
		{
			if (TRUE == aboConfirmedFlags[u8ArrayIDX])
			{
				CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX]++;
	
				if (u32PhaseMax < CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX])
				{
					CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] -= (CEM_xEdgesCount / CEM_u8PhaseRepeats);
				}
			}
			else
			{
				CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] = 128;
			}
	
			u32PhaseMin += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
			u32PhaseMax += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
			u32PhaseMin = u32PhaseMin % CEM_xEdgesCount;
			u32PhaseMax = u32PhaseMax % CEM_xEdgesCount;
		}
	}

    u16LastGapFraction = CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount] / CEM_u8PhaseRepeats;
	tLastGapTime = CEM_tCalculateGlobalTime(tEventTime, u16LastGapFraction, CEM_aboSyncEdge[CEM_u32CrankEdgeCounter]);

	if	(CEM_enAutocorrelationMode == CEM_enTriggerType)
	{
		/* Increment the edge counter if possible */
		for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
		{
			if (TRUE == aboConfirmedFlags[u8ArrayIDX])
			{
				CEM_u32CrankEdgeCounter = (CEM_u32CrankEdgeCounter + 1) % CEM_xEdgesCount;
				break;
			}
		}
	}

	CEM_tEventTimeLast = tEventTime;
	CEM_tToothEventTimeLast = tEventTime;
	CEM_u32CrankEdgeTimeoutCount = 0;

	/* Shift the input array */
	puint32 pu32 = au32InputArray;
	pu32 += (CEM_nEdgesMax - CEM_xEdgesCount);
	memcpy(pu32, pu32 + 1, sizeof(uint32) * CEM_xEdgesCount);
	
	/* Buffer the new gap time */
	au32InputArray[CEM_nEdgesMax - 1] = tLastGapTime;
	
	/* If enough edges have arrived to proceed */
	if ((5 / CEM_u8PhaseRepeats) <= u32EdgeCount)//matthew
	{
	    switch (CEM_enTriggerType)
		{
            case CEM_enAutocorrelationMode:
			{
				CEM_au8AutocorrPeak[0] = 0x80;
				CEM_au8AutocorrPeak[1] = 0x80;
				CEM_au8AutocorrPeak[2] = 0x80;
				CEM_au8AutocorrPeak[3] = 0x80;
	
				/* Calculate the autocorrelation array */
				memset((void*)&au32AutoCorr, 0, CEM_nEdgesMax * sizeof(uint32));
				uint32 u32ControlArrayIDX = 0;
	
				for (uint32 u32AutocorrPhase = 0; u32AutocorrPhase < CEM_xEdgesCount; u32AutocorrPhase++)
				{
					uint32 u32InputArrayIDX = CEM_nEdgesMax - CEM_xEdgesCount;
	
					for (uint32 u32ShiftCount = 0; u32ShiftCount < CEM_xEdgesCount; u32ShiftCount++)
					{
						u32Temp = (au32InputArray[u32InputArrayIDX] * (uint32)CEM_au16AllEdge[u32ControlArrayIDX]);
						u32Temp /= 0x100u;
						au32AutoCorr[u32AutocorrPhase] += u32Temp;
						u32ControlArrayIDX = (u32ControlArrayIDX + 1) % CEM_xEdgesCount;
						u32InputArrayIDX++;
					}
					u32ControlArrayIDX++;
				}	
	
				/* Find the autocorrelation peak */
				uint32 u32AutoPeak = 0;
	
				for (uint32 u32ArrayIDX = 0; u32ArrayIDX < CEM_xEdgesCount; u32ArrayIDX++)
				{
					if (u32AutoPeak < au32AutoCorr[u32ArrayIDX])
					{
						u32AutoPeak = au32AutoCorr[u32ArrayIDX];
					}
				}
	
				/* Set the auto-peaks indices array */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < CEM_xEdgesCount; u8ArrayIDX++)
				{
					if (u32AutoPeak == au32AutoCorr[u8ArrayIDX])
					{
						if (128u == CEM_au8AutocorrPeak[0])
						{
							CEM_au8AutocorrPeak[0] = u8ArrayIDX;
						}
						else if (128u == CEM_au8AutocorrPeak[1])
						{
							CEM_au8AutocorrPeak[1] = u8ArrayIDX;
						}
						else if (128u == CEM_au8AutocorrPeak[2])
						{
							CEM_au8AutocorrPeak[2] = u8ArrayIDX;
						}
						else if (128u == CEM_au8AutocorrPeak[3])
						{
							CEM_au8AutocorrPeak[3] = u8ArrayIDX;
						}
					}
				}
	
				u32PhaseMin = 0;
				u32PhaseMax = (CEM_xEdgesCount / CEM_u8PhaseRepeats) - 1;
	
				/* Buffer the confirmed array if is not initialized */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
				{
					if (128u > CEM_au8AutocorrPeak[u8ArrayIDX])
    				{
						if (127 < CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX])
						{
							if ((u32PhaseMin <= CEM_au8AutocorrPeak[u8ArrayIDX]) &&
							(u32PhaseMax >= CEM_au8AutocorrPeak[u8ArrayIDX]))
							{
								CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] = CEM_au8AutocorrPeak[u8ArrayIDX];
	
								if (0xff == CEM_u32CrankEdgeCounter)
								{
									CEM_u32CrankEdgeCounter = CEM_au8AutocorrPeak[u8ArrayIDX];
								}
							}
						}
					}
	
					u32PhaseMin += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
					u32PhaseMax += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
					u32PhaseMin = u32PhaseMin % CEM_xEdgesCount;
					u32PhaseMax = u32PhaseMax % CEM_xEdgesCount;
				}
	
				/* Clear the confirmed flags */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
				{
					aboConfirmedFlags[u8ArrayIDX] = FALSE;
				}
	
				/* Check if the confirmed array is still confirmed */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
				{
				    for (uint8 u8ConfirmedArrayIDX = 0; u8ConfirmedArrayIDX < 4; u8ConfirmedArrayIDX++)
					{
						if (CEM_au8ConfirmedAutocorrPeak[u8ConfirmedArrayIDX] == CEM_au8AutocorrPeak[u8ArrayIDX])
						{
							aboConfirmedFlags[u8ConfirmedArrayIDX] = TRUE;
							au8ErrorCount[u8ConfirmedArrayIDX] = 0;
						}
					}
				}
	
				/* Increment error counter of unconfirmed */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
				{
					if (FALSE == aboConfirmedFlags[u8ArrayIDX])
					{
						au8ErrorCount[u8ArrayIDX] = (au8ErrorCount[u8ArrayIDX] + 1) % 0x80;
					}
				}
	
				/* Check for phase lock on any index */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
				{
					if (CEM_u32CrankEdgeCounter == CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX]) {boPhaseLocked = TRUE;}
				}
	
				/* Buffer and edge count if we are phase locked */
				if (FALSE == boPhaseLocked)
				{
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						if (TRUE == aboConfirmedFlags[u8ArrayIDX])
						{
							CEM_u32CrankEdgeCounter = CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX];
						}
					}
				}

				CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];
	
				/* Are we at a calculation edge? */
				if (0 == CEM_u32CrankEdgeCounter)
				{
				    CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
				    TEPM_vSynchroniseEventProgramKernelQueues();
				}

				if (1 == CEM_u8PhaseRepeats)
				{
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}
				break;
			}
			case CEM_enOneGroupMissing:
			case CEM_enTypeWVEA888:
			{
				CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];

		        if (au32InputArray[CEM_nEdgesMax - 1] > au32InputArray[CEM_nEdgesMax - 2] * 2)
				{
					if ((CEM_u32CrankEdgeCounter != CEM_u8SimpleMissingSync) &&
						(100 < u32EdgeCount))
					{
						CEM_vPhaseError(CEM_u32CrankEdgeCounter);
					} 


			        CEM_u32CrankEdgeCounter = CEM_u8SimpleMissingSync;
			        CEM_u32ToothEdgeCounter = (CEM_u8SimpleMissingSync + CEM_u8MissingToothCountMax) %
			        		CEM_xAllEdgesCount;

					if (1 == CEM_u8PhaseRepeats)
					{
						CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
					}
					else
					{
						if ((CEM_u32CycleToothEdgeCounter != CEM_u32ToothEdgeCounter) &&
								(CEM_u32CycleToothEdgeCounter != (CEM_u32ToothEdgeCounter + CEM_xAllEdgesCount)))
						{
							CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
						}
					}
				}
		        else
		        {
		        	CEM_u32SmallestToothTime = au32InputArray[CEM_nEdgesMax - 1];
		        }

		        /* Is tooth prior to gap/gaps */
		        if (CEM_u8SimpleMissingSync ==
		        		((CEM_u32CrankEdgeCounter + 1) % (CEM_xEdgesCount + CEM_u8MissingToothCountMax)))
		        {
					TEPM_vSetNextMissingToothInterrupt(enEHIOResource, CEM_u32SmallestToothTime, CEM_u8MissingToothCountMax);
		        }

		        /* Sync over 720 degrees */
		        if (CEM_enTypeWVEA888 == CEM_enTriggerType)
		        {
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						if (0x10000 <= CEM_u32GlobalCycleFraction)
						{
							if ((CEM_atLastVVTHighTime[0] < (CEM_u32GlobalCycleTime / 8))
									&& (CEM_atLastVVTHighTime[0] > (CEM_u32GlobalCycleTime / 32)))
							{
								boLatePhase = false;
							}
							else if ((CEM_atLastVVTLowTime[0] < (CEM_u32GlobalCycleTime / 8))
									&& (CEM_atLastVVTLowTime[0] > (CEM_u32GlobalCycleTime / 32)))
							{
								boLatePhase = true;
							}
							else
							{
								boLatePhase = !boLatePhase;
							}
						}
						else
						{
							if (CEM_atLastVVTHighTime[0] < (CEM_u32GlobalCycleTime / 8))
							{
								boLatePhase = false;
							}
							else if (CEM_atLastVVTLowTime[0] < (CEM_u32GlobalCycleTime / 8))
							{
								boLatePhase = true;
							}
							else
							{
								boLatePhase = !boLatePhase;
							}
						}
					}
		        }
		        else if (CEM_u32CrankEdgeCounter == stSimpleCamSync.u32CamSyncSampleToothCount)
				{
					boLatePhase = CEM_boGetLatePhaseSimpleCamSync();
				}

				if (0 == CEM_u32CrankEdgeCounter)
				{
				    CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
				    TEPM_vSynchroniseEventProgramKernelQueues();
				}

				if (1 == CEM_u8PhaseRepeats)
				{
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x4000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x8000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0xc000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}
				else if (2 == CEM_u8PhaseRepeats)
				{
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}

				if ((25 == CEM_u32CrankEdgeCounter) || (53 == CEM_u32CrankEdgeCounter))
				{
					TEPM_vStartEventProgramKernelQueues(FALSE, 0x80);
				}

				CEM_u32CrankEdgeCounter = (CEM_u32CrankEdgeCounter + 1) % CEM_xEdgesCount;

				if (1 == CEM_u8PhaseRepeats)
				{
					CEM_u32ToothEdgeCounter = (CEM_u32ToothEdgeCounter + 1) % CEM_xAllEdgesCount;
					CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
				}
				else
				{
					TEPM_vRunEventToothProgramKernelQueues(FALSE, CEM_u32CycleToothEdgeCounter, CEM_u32SmallestToothTime);
					CEM_u32CycleToothEdgeCounter = (CEM_u32CycleToothEdgeCounter + 1) % (2 * CEM_xAllEdgesCount);
					CEM_u32ToothEdgeCounter = (CEM_u32ToothEdgeCounter + 1) % CEM_xAllEdgesCount;
				}

				break;
			}
			case CEM_enTypeSuzukiM15A:
			{
				CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];

		        if (au32InputArray[CEM_nEdgesMax - 1] > (au32InputArray[CEM_nEdgesMax - 2] * 2))
				{
					/* Crank sensor gap */
					if (((CEM_u32CrankEdgeCounter != 21) && (CEM_u32CrankEdgeCounter != 7)) &&
						(100 < u32EdgeCount))
					{
						CEM_vPhaseError(CEM_u32CrankEdgeCounter);
					} 

					CEM_u32CrankEdgeCounter++;

					u8SameGapCount = 0;

					CEM_u32CrankEdgeCounter = 30 <= CEM_u32CrankEdgeCounter ? 1 : CEM_u32CrankEdgeCounter;
				}
		        else if (au32InputArray[CEM_nEdgesMax - 1] < (au32InputArray[CEM_nEdgesMax - 2] / 2))
				/* Crank sensor end gap */
		        {
			        if (((CEM_u32CrankEdgeCounter != 22) && (CEM_u32CrankEdgeCounter != 9)) &&
			        (100 < u32EdgeCount))
			        {
				        CEM_vPhaseError(CEM_u32CrankEdgeCounter);
			        }

					if (0 < u8SameGapCount)
					{
						CEM_u32CrankEdgeCounter = 10;
					}
					else
					{
						CEM_u32CrankEdgeCounter = 23;
						CEM_u32GlobalCycleFraction = CEM_au16AllEdge[0] + CEM_au16AllEdge[1];
					}

					u8SameGapCount = 0;
		        }
				else
				{
					/* Crank sensor same gap */
					u8SameGapCount++;
					CEM_u32CrankEdgeCounter++;
				}


				if (31 == CEM_u32CrankEdgeCounter)
				{
					CEM_u32GlobalCycleFraction &= 0x10000;
				    CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
				    TEPM_vSynchroniseEventProgramKernelQueues();
					CEM_u32CrankEdgeCounter = 1;
				}

				if (1 == CEM_u8PhaseRepeats)
				{
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x4000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x8000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0xc000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}
				else if (2 == CEM_u8PhaseRepeats)
				{
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}

				break;
			}
			default:
			{
				break;
			}
		}
	

		if ((1u != ((CEM_u32CrankEdgeCounter - u8PhasePrev) % CEM_xEdgesCount)) && (TRUE == boPhaseLocked))
		{
			CEM_vPhaseError(0);
		}
	}
}

static bool CEM_boGetLatePhaseSimpleCamSync(void)
{
	bool boLatePhase;
	IOAPI_tenTriState enTriState;

	enTriState = IO_enGetDIOResourceState(stSimpleCamSync.enEHIOResource);

	if (true == stSimpleCamSync.boCamSyncHighLate)
	{
		boLatePhase = (IOAPI_enHigh == enTriState);
	}
	else
	{
		boLatePhase = (IOAPI_enLow == enTriState);
	}
	
	return boLatePhase;
}

static void CEM_vPhaseError(int code_in)
{
	/* TODO
	volatile static int code;
	code = code_in; 
	*/
	CEM_u32CrankErrorCounts++;
}


static TEPMAPI_ttEventTime CEM_tCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction, bool boGlobalTimeEnable)
{
	TEPMAPI_ttEventTime tTemp;
	TEPMAPI_ttEventTime tGlobalTimeNew = 0;
	uint32 u32Temp;
	
    tTemp = (tEventTime - CEM_tEventTimeLast) & TEPMHA_nCounterMask;

	if (0xffff >= tTemp)
	{
		tGlobalTimeNew = (0x10000u * tTemp) / (uint32)u16LastGapFraction;
		//tGlobalTimeNew = 220000;
	}
	else if (0xfffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 4;
		tGlobalTimeNew = (0x1000u * tTemp) / (uint32)u16LastGapFraction;
	}
	else if (0xffffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 8;
		tGlobalTimeNew = (0x100u * tTemp) / (uint32)u16LastGapFraction;
	}	
	else if (0xfffffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 12;
		tGlobalTimeNew = (0x10u * tTemp) / (uint32)u16LastGapFraction;
	}	
	else
	{
	    CEM_u32GlobalCycleTime = ~0u;
	}

	if (TRUE == boGlobalTimeEnable)
	{
		if (TRUE == TPS_boThrottleClosed)
		{
			u32Temp = USERCAL_stRAMCAL.u16ESTFilterClosed * CEM_u32GlobalCycleTime;
			u32Temp += ((0x100 - USERCAL_stRAMCAL.u16ESTFilterClosed) * tGlobalTimeNew);
			u32Temp /= 0x100;
		}
		else
		{
			u32Temp = USERCAL_stRAMCAL.u16ESTFilterOpen * tGlobalTimeNew;
			u32Temp += ((0x100 - (uint8)USERCAL_stRAMCAL.u16ESTFilterOpen) * CEM_u32GlobalCycleTime);
			u32Temp /= 0x100;
		}

		CEM_u32GlobalCycleTime = u32Temp;

		if (tGlobalTimeNew < CEM_u32GlobalCycleTime)
		{
			CEM_u32GlobalCycleTime--;
		}
		else if (tGlobalTimeNew > CEM_u32GlobalCycleTime)
		{
			CEM_u32GlobalCycleTime++;
		}
	}

	return tTemp;
}

static void CEM_vSequenceReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint32 u32OriginGlobalCycleFraction, bool boLatePhase)
{
	uint32 u32Temp;
	static uint32 u32SequenceIDX;
	static IOAPI_tenTriState enTriState;
	IOAPI_tenEHIOResource enLinkedResource;

	CEM_tSyncTimeLast = tEventTime;

	u32SequenceIDX++;	
	u32Temp = CEM_u32GlobalCycleTime / (4 * CEM_u8PhaseRepeats);
	u32Temp = 0x10000 > u32Temp ? u32Temp : 0xffff;

	if (1 == CEM_u8PhaseRepeats)
	{
		if (0 == u32OriginGlobalCycleFraction)
		{
			u32SequenceIDX = 0;

			CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
			CEM_u32GlobalCycleOriginCount++;
		}
	}
	else
	{
		if (0 == u32OriginGlobalCycleFraction)
		{
			if (((0x10000 <= CEM_u32GlobalCycleFraction) && (false == boLatePhase)) ||
			    ((0x10000 > CEM_u32GlobalCycleFraction) && (true == boLatePhase)))
			{
				CEM_u32CamErrorCounts++;
				CEM_u32CamRunningErrorCounts = 5 > CEM_u32CamRunningErrorCounts ? CEM_u32CamRunningErrorCounts + 1 : 5;
			}
			
			
			if (5 == CEM_u32CamRunningErrorCounts)	
			/* Many cam errors */
			{
				//if (0 == (CEM_u32GlobalCycleOriginCount % 8))
				if (true)
				/* Try swap phase */
				{
					if (0x10000 > CEM_u32GlobalCycleFraction)
					{
						u32SequenceIDX = 4;
						CEM_u32GlobalCycleFraction = 0x10000 + u32OriginGlobalCycleFraction;
					}
					else
					{
						u32SequenceIDX = 0;
						CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
					}

					CEM_u32CamRunningErrorCounts = 0;
				}
				else
				/* Keep phase and reset */
				{
					if (0x10000 <= CEM_u32GlobalCycleFraction)
					{
						u32SequenceIDX = 0;
						CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
					}
					else
					{
						u32SequenceIDX = 4;
						CEM_u32GlobalCycleFraction = 0x10000 + u32OriginGlobalCycleFraction;
					}
				}
			}
			else if ((0x10000 <= CEM_u32GlobalCycleFraction) && (true == boLatePhase))
			{
				u32SequenceIDX = 0;
				CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
				CEM_u32CycleToothEdgeCounter = 0;
			}
			else
			{
				CEM_u32GlobalCycleFraction = 0x10000 + u32OriginGlobalCycleFraction;
			}

			CEM_u32GlobalCycleOriginCount++;

			if (0 == (CEM_u32GlobalCycleOriginCount % 8))
			{
				CEM_u32CamRunningErrorCounts = 0 < CEM_u32CamRunningErrorCounts ? CEM_u32CamRunningErrorCounts - 1 : 0;
			}

			/* grab the primary linked resource for the phase tell-tale to slave uP */
			enLinkedResource = TEPM_enGetPrimaryLinkedResource();

			/* set the phase tell-tale */
			enTriState = true == boLatePhase ? IOAPI_enHigh : IOAPI_enLow;
			IO_vAssertDIOResource(enLinkedResource, enTriState);
		}
	}

	
	if (1 < CEM_u32GlobalCycleOriginCount)
	{
		TEPM_vStartEventProgramKernelQueues(FALSE, u32SequenceIDX);	
	}

	if ((0 == (0xffff & CEM_u32GlobalCycleFraction)) || (0x8000 == (0xffff & CEM_u32GlobalCycleFraction)))
	{
		TEPM_vInitiateUserCallBack(enEHIOResource, (uint16)u32Temp);
	}
}

void CEM_vPhaseEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	IOAPI_tenTriState enTriState;


	if (enEHIOResource == TEPM_astTEPMVVTInputs[0])
	{
		enTriState = IO_enGetDIOResourceState(enEHIOResource);

		if (IOAPI_enLow == enTriState)
		{
			/* Note signal is internally inverted */
			CEM_atLastVVTLowTime[0] = TEPMHA_nCounterMask & (tEventTime - CEM_atLastVVTTimer[0]);
		}
		else
		{
			CEM_atLastVVTHighTime[0] = TEPMHA_nCounterMask & (tEventTime - CEM_atLastVVTTimer[0]);
		}

		CEM_atLastVVTTimer[0] = tEventTime;
	}
	else if (enEHIOResource == TEPM_astTEPMVVTInputs[1])
	{

	}
	else if (enEHIOResource == TEPM_astTEPMVVTInputs[2])
	{

	}
	else if (enEHIOResource == TEPM_astTEPMVVTInputs[3])
	{

	}
}


void CEM_vSetupSimpleCamSync(IOAPI_tenEHIOResource enEHIOResource, bool boCamSyncHighLate, uint32 u32CamSyncSampletoothCount)
{
	stSimpleCamSync.enEHIOResource = enEHIOResource;
	stSimpleCamSync.boCamSyncHighLate = boCamSyncHighLate;
	stSimpleCamSync.u32CamSyncSampleToothCount = u32CamSyncSampletoothCount;
}

void CEM_vFreqEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	static uint32 u32OldOSTick;
	static uint32 u32TimeOld;
	static uint32 u32Sum;
	static uint32 u32SampleCount;
	uint32 u32Sample;
	static uint32 u32ErrCount;
	static uint32 u32SampleOld;

	u32Sample = (tEventTime - u32TimeOld) & TEPMHA_nCounterMask;

	u32SampleOld -= 5;
	u32SampleOld *= 2;

	if ((u32Sample > u32SampleOld) &&
		(2 > u32ErrCount))
	{
		u32Sample /= 2;
		u32ErrCount++;
	}
	else
	{
		u32ErrCount = 0;
	}

	u32SampleOld = u32Sample;
	u32Sum += u32Sample;
	u32SampleCount++;

	if (OS_u32TickCounter > (u32OldOSTick + 1))
	{
		TEPM_vInitiateUserCallBack(enEHIOResource, u32Sum / u32SampleCount);
		u32OldOSTick = OS_u32TickCounter;
		u32Sum = 0;
		u32SampleCount = 0;
	}

	u32TimeOld = tEventTime;
}

uint32 CEM_u32GetAllEdgesCount(void)
{
	return CEM_xAllEdgesCount;
}
