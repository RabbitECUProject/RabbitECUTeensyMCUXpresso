/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      FME                                                    */
/* DESCRIPTION:        This code assist with failure mode effects             */
/*                     processing                                             */
/*                                                                            */
/* FILE NAME:          FME.c                                                  */
/* REVISION HISTORY:   24-06-2020 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _FME_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "FME.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
const FME_tstDiagControlData FME_astDiagControlData[FME_enDiagCount] = FME_nControlData;
FME_tstDiagRunningData FME_astDiagRunningData[FME_enDiagCount];
//ASAM mode=readvalue name="PPS Rationality Fault" parent="FME_astDiagRunningData" type=uint16 offset=8 min=0 max=2 units="ENUMERATION NO_FAULT=0 FAULT_ACTIVE=1 FAULT_CLEARED=2" format=3.0 help="FAULT"
//ASAM mode=readvalue name="Throttle Rationality Fault" parent="FME_astDiagRunningData" type=uint16 offset=20 min=0 max=2 units="ENUMERATION NO_FAULT=0 FAULT_ACTIVE=1 FAULT_CLEARED=2" format=3.0 help="FAULT"
//ASAM mode=readvalue name="TPSM Fault" parent="FME_astDiagRunningData" type=uint16 offset=32 min=0 max=2 units="ENUMERATION NO_FAULT=0 FAULT_ACTIVE=1 FAULT_CLEARED=2" format=3.0 help="FAULT"
//ASAM mode=readvalue name="PPSM Fault" parent="FME_astDiagRunningData" type=uint16 offset=44 min=0 max=2 units="ENUMERATION NO_FAULT=0 FAULT_ACTIVE=1 FAULT_CLEARED=2" format=3.0 help="FAULT"
//ASAM mode=readvalue name="PPSS Fault" parent="FME_astDiagRunningData" type=uint16 offset=56 min=0 max=2 units="ENUMERATION NO_FAULT=0 FAULT_ACTIVE=1 FAULT_CLEARED=2" format=3.0 help="FAULT"


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void FME_vConditionFault(FME_tenDiags, bool);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void FME_enUpdateDiagState(FME_tenDiags enDiags, uint32 u32Data1, uint32 u32Data2)
{
	switch (FME_astDiagControlData[enDiags].enDiagtype)
	{
		case FME_enRelationship:
		{
			if (FME_enPPSPair == enDiags)
			{
				uint32 u32Temp = (500 * u32Data1) / u32Data2;

				FME_astDiagRunningData[enDiags].u32FilterData =
						u32Temp / 8 + 7 * (FME_astDiagRunningData[enDiags].u32FilterData / 8);

				if ((800 > FME_astDiagRunningData[enDiags].u32FilterData) ||
						(1200 < FME_astDiagRunningData[enDiags].u32FilterData))
				{
					FME_vConditionFault(enDiags, TRUE);
				}
				else
				{
					FME_vConditionFault(enDiags, FALSE);
				}
			}
			if (FME_enPedalTransfer == enDiags)
			{
				if (u32Data1 < u32Data2)
				{
					FME_vConditionFault(enDiags, TRUE);
				}
				else
				{
					FME_vConditionFault(enDiags, FALSE);
				}
			}
			break;
		}
		case FME_enSingle:
		{
			FME_astDiagRunningData[enDiags].u32FilterData =
					u32Data1 / 4 + 3 * (FME_astDiagRunningData[enDiags].u32FilterData / 4);

			if ((FME_astDiagControlData[enDiags].u32FilterDataMin > FME_astDiagRunningData[enDiags].u32FilterData) ||
					(FME_astDiagControlData[enDiags].u32FilterDataMax < FME_astDiagRunningData[enDiags].u32FilterData))
			{
				FME_vConditionFault(enDiags, TRUE);
			}
			else
			{
				FME_vConditionFault(enDiags, FALSE);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

inline FME_tenFaultState FME_enGetDiagState(FME_tenDiags enDiags)
{
	return FME_astDiagRunningData[enDiags].enFaultState;
}

static void FME_vConditionFault(FME_tenDiags enDiags, bool boFaultActive)
{
	static bool boEngineStart;
	static uint32 u32EngineStartTime = 0;

	if (600 < CAM_u32RPMFiltered)
	{
		boEngineStart = TRUE;
	}
	else if (0 == CAM_u32RPMFiltered)
	{
		boEngineStart = FALSE;
		u32EngineStartTime = USERDIAG_u32GlobalTimeTick;
	}

	if ((TRUE == boFaultActive) && (5000 < (USERDIAG_u32GlobalTimeTick - u32EngineStartTime))
			&& (TRUE == boEngineStart))
	{
		/* Only if engine running more than 5 seconds - cranking can cause sensor VCC brown-out faults */
		FME_astDiagRunningData[enDiags].u32FaultCounts =
			FME_astDiagControlData[enDiags].u32FaultCountLimit > FME_astDiagRunningData[enDiags].u32FaultCounts ?
				FME_astDiagRunningData[enDiags].u32FaultCounts + 1 :
				FME_astDiagRunningData[enDiags].u32FaultCounts;
	}
	else
	{
		FME_astDiagRunningData[enDiags].u32FaultCounts =
			0 < FME_astDiagRunningData[enDiags].u32FaultCounts ?
				FME_astDiagRunningData[enDiags].u32FaultCounts - 1 :
				FME_astDiagRunningData[enDiags].u32FaultCounts;
	}

	if ((FME_enNoFault == FME_astDiagRunningData[enDiags].enFaultState) &&
			(FME_astDiagControlData[enDiags].u32FaultCountLimit == FME_astDiagRunningData[enDiags].u32FaultCounts))
	{
		FME_astDiagRunningData[enDiags].enFaultState = FME_enFaultActive;
	}
	else if ((FME_enFaultCleared == FME_astDiagRunningData[enDiags].enFaultState) &&
			(FME_astDiagControlData[enDiags].u32FaultCountLimit == FME_astDiagRunningData[enDiags].u32FaultCounts))
	{
		FME_astDiagRunningData[enDiags].enFaultState = FME_enFaultActive;
	}
	else if ((FME_enFaultActive == FME_astDiagRunningData[enDiags].enFaultState) &&
			(0 == FME_astDiagRunningData[enDiags].u32FaultCounts))
	{
		FME_astDiagRunningData[enDiags].enFaultState = FME_enFaultCleared;
	}
}

void FME_vDiagClearHistory(void)
{
	uint32 u32FaultIDX;

	for (u32FaultIDX = 0; u32FaultIDX < FME_enDiagCount; u32FaultIDX++)
	{
		FME_astDiagRunningData[u32FaultIDX].enFaultState = FME_enNoFault;
	}
}

#endif //BUILD_USER
