/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Manifold Pressure Sensor                               */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for manifold pressure          */
/*                     measurement                                            */
/* FILE NAME:          MAP.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _MAP_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "MAP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool MAP_boNewSample;

SPREADAPI_ttSpreadIDX MAP_tSpreadVExIDX;
SPREADAPI_ttSpreadIDX MAP_tSpreadVEyIDX;
TABLEAPI_ttTableIDX MAP_tMapVEIDX;
SPREADAPI_ttSpreadIDX MAP_tSpreadPseudoMAPxIDX;
SPREADAPI_ttSpreadIDX MAP_tSpreadPseudoMAPyIDX;
TABLEAPI_ttTableIDX MAP_tMapPseudoMAPIDX;


SPREADAPI_ttSpreadIDX MAP_tSpreadPressureValveFeedForwardIDX;
TABLEAPI_ttTableIDX MAP_tTablePressureValveFeedForwardIDX;

EXTERN GPM6_ttUg MAP_tManChargeMassOldUg;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : MAP_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void MAP_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void MAP_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	
	if (EH_IO_Invalid != USERCAL_stRAMCAL.u16MAPADResource)
	{
		enEHIOResource = USERCAL_stRAMCAL.u16MAPADResource;
		enEHIOType = IOAPI_enGPSE;
		stADCCB.enSamplesAv = ADCAPI_en32Samples;
		stADCCB.pfResultCB = &MAP_vADCCallBack;
		stADCCB.enTrigger = ADCAPI_enTrigger4;

		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stADCCB);

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult)
			{
				*pu32Arg |= (uint32)SYSAPI_enResourceRequestFailed;/*CR1_13*/
			}
		}
		else
		{
			*pu32Arg |= (uint32)SYSAPI_enResourceInitFailed;/*CR1_13*/
		}
	}
	
	MAP_tKiloPaFiltered = MAP_nVoltsFilteredInitVal;
	MAP_u32ADCRaw = MAP_nADCRawInitVal;
	MAP_tKiloPaRaw = 101300;
	
	MAP_tManChargeMassUg = 0;
	MAP_s32ManDeltaChargeMassPerSUg = 0;	
	
	/* Request and initialise required Kernel managed spread for VEx */
	MAP_tSpreadVExIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserVExSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for VEy */
	MAP_tSpreadVEyIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserVEySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for VE*/
	MAP_tMapVEIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserMAPVE, (void*)&MAP_u16VE, TYPE_enUInt16, 17, 17, MAP_tSpreadVExIDX, MAP_tSpreadVEyIDX, NULL);

	/* Request and initialise required Kernel managed spread for pressure control valve feedforward */
	MAP_tSpreadPressureValveFeedForwardIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaTargetError, (void*)&USERCAL_stRAMCAL.aUserPressureValveFeedForwardSpread, TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for pressure control valve feedforward */
	MAP_tTablePressureValveFeedForwardIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPressureValveFeedForwardTable, (void*)&MAP_u16PressureValveFeedForward, TYPE_enUInt16, 17, MAP_tSpreadPressureValveFeedForwardIDX, NULL);

	/* Request and initialise required Kernel managed spread for Pseudo MAP x */
	MAP_tSpreadPseudoMAPxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserPseudoMAPxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for Pseudo MAP y */
	MAP_tSpreadPseudoMAPyIDX = SETUP_tSetupSpread((void*)&TPS_tThetaFiltered, (void*)&USERCAL_stRAMCAL.aUserPseudoMAPySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Pseudo MAP */
	MAP_tMapPseudoMAPIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPseudoMAPMap, (void*)&MAP_u32PseudoMAP, TYPE_enUInt32, 17, 17, MAP_tSpreadPseudoMAPxIDX, MAP_tSpreadPseudoMAPyIDX, NULL);

	MAP_tBoostTarget = 150000;
}

void MAP_vRun(puint32 const pu32Arg)
{
    static GPM6_ttPa MAP_tKiloPaOld = 0;
	static uint32 u32Count;
	static sint32 s32ErrorDerivativeFiltered;
	static sint32 s32ErrorOld;
	static bool boEnableVELearn = false;
	sint32 s32DeltaMAP;
	sint32 s32DeltaManifoldMass;
	sint32 s32Temp;
	SPREADAPI_tstSpreadResult* pstSpreadResultVEx;
	SPREADAPI_tstSpreadResult* pstSpreadResultVEy;
	uint32 u32TableIDXx = ~0;
	uint32 u32TableIDXy = ~0;
	uint8 u8MapFilter = 0x40;
	uint32 u32Temp;
	uint8 u8Temp;
	uint32 u32TPSWeightLimit;


	/* Reduce filtering on RPM transitions */
	if (0 != CAM_u32RPMTransitionCounter)
	{
		u8MapFilter *= 2;
	}

	/* Reduce filtering on TPS transitions */
	if ((0 != TPS_u32TransitionCounter) || (0 != TPS_u32ThrottleMovingCounter))
	{
		u8MapFilter *= 2;
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.u16MAPADResource)
	{
		/* MAP sensor is configured go ahead and filter */
		USERMATH_u16SinglePoleLowPassFilter16(MAP_u32ADCRaw, u8MapFilter, &MAP_u32ADCFiltered);

		s32Temp = MAP_u32ADCFiltered * SENSORS_nADRefVolts;
		s32Temp /= SENSORS_nADScaleMax;
		s32Temp /= SENSORS_nVDivRatio;

		MAP_tSensorVolts = s32Temp;

		/* Calculate manifold pressure */
		s32Temp = (USERCAL_stRAMCAL.s32MapSensorGain * MAP_tSensorVolts) / 1000;
		u32Temp = 0 > (s32Temp + USERCAL_stRAMCAL.s32MapSensorOffset) ? 0 : (uint32)(s32Temp +  USERCAL_stRAMCAL.s32MapSensorOffset);

		if (FALSE == USERCAL_stRAMCAL.u8BoostChargeMAPEnable)
		{
			MAP_tKiloPaFiltered = u32Temp;
		}
		else
		{
			s32Temp = u32Temp;

			u32TPSWeightLimit = USERCAL_stRAMCAL.u16BoostChargeTPSLimit + 4 * CAM_u32RPMFiltered;

			/* 9923 * airflowUGPS / RPM * VE * CAPACITY = MAP */
			u32Temp = (11500 * (AFM_tAirFlowAFMUg / CAM_u32RPMFiltered)) /  (USERCAL_stRAMCAL.u16CylinderCC * USERCAL_stRAMCAL.u8CylCount);
			u32Temp = 10000 > u32Temp ? 10000 : u32Temp;
			u32Temp = 200000 < u32Temp ? 200000 : u32Temp;


			if (u32TPSWeightLimit < TPS_tThetaFiltered)
			{
				/* Low charge pipe pressure - use charge pressure MAP*/
				MAP_tKiloPaFiltered = (uint32)s32Temp;
			}
			else
			{
				/* Mix */
				s32Temp /= 10;
				s32Temp *= TPS_tThetaFiltered;
				s32Temp /= u32TPSWeightLimit;
				s32Temp *= 10;

				u32Temp /= 10;
				u32Temp *= (u32TPSWeightLimit - TPS_tThetaFiltered);
				u32Temp /= u32TPSWeightLimit;
				u32Temp *= 10;

				MAP_tKiloPaFiltered = (uint32)s32Temp + u32Temp;
			}
		}

		/* Calculate the current spread for VEx */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadVExIDX,
		NULL, NULL);
		
		/* Calculate the current spread for VEy */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadVEyIDX,
		NULL, NULL);

		/* Lookup the current value for VE */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&MAP_tMapVEIDX,
		NULL, NULL);
	}
	else if (TRUE == USERCAL_stRAMCAL.u8PseudoMAPEnable)
	{
		/* Calculate the current spread for Pseudo MAP x */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadPseudoMAPxIDX,
		NULL, NULL);

		/* Calculate the current spread for Pseudo MAP y */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadPseudoMAPyIDX,
		NULL, NULL);

		/* Lookup the current value for Pseudo MAP  */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&MAP_tMapPseudoMAPIDX,
		NULL, NULL);

		MAP_tKiloPaFiltered = MAP_u32PseudoMAP;

		/* Calculate the current spread for VEx */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadVExIDX,
		NULL, NULL);

		/* Calculate the current spread for VEy */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadVEyIDX,
		NULL, NULL);

		/* Lookup the current value for VE */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&MAP_tMapVEIDX,
		NULL, NULL);
	}
	else
	{
		/* Back calculate the manifold pressure from airflow */
		/* 9923 * airflowUGPS / RPM * VE * CAPACITY = MAP */
		u32Temp = (11500 * (AFM_tAirFlowAFMUg / CAM_u32RPMFiltered)) /  (USERCAL_stRAMCAL.u16CylinderCC * USERCAL_stRAMCAL.u8CylCount);
		u32Temp = 10000 > u32Temp ? 10000 : u32Temp;
		u32Temp = 200000 < u32Temp ? 200000 : u32Temp;
		MAP_tKiloPaFiltered = u32Temp;
	}
	

	if ((0 == TPS_u32ThrottleMovingCounter) && (400 < CAM_u32RPMRaw) && (0 == (u32Count % 5)))
	{
		/* Get the current x spread for VE */
           pstSpreadResultVEx = BOOSTED_pstGetSpread(MAP_tSpreadVExIDX);

		/* Get the current x spread for VE */
           pstSpreadResultVEy = BOOSTED_pstGetSpread(MAP_tSpreadVEyIDX);

        u32TableIDXx = (0x4000 > pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadIndex : u32TableIDXx;
        u32TableIDXx = (0xc000 < pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadIndex + 1 : u32TableIDXx;
        u32TableIDXy = (0x4000 > pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadIndex : u32TableIDXy;
        u32TableIDXy = (0xc000 < pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadIndex + 1 : u32TableIDXy;

		/* If the indexes are valid then learn the new VE */
		if ((0xffffffff != u32TableIDXx) && (0xffffffff != u32TableIDXy) && (true == boEnableVELearn))
		{
		    if (AFM_u16LearnVE < USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy])
			{
				USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy] = 400 < USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy] ?
					USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy] - 1 : 400;
			}
		    else if (AFM_u16LearnVE > USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy])
			{
				USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy] = 1000 > USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy] ?
					USERCAL_stRAMCAL.aUserMAPVE[u32TableIDXx][u32TableIDXy] + 1 : 1000;
			}
		}
	}


	if (0 == (u32Count % 5))
	{
		s32DeltaMAP = MAP_tKiloPaFiltered - MAP_tKiloPaOld;
		s32DeltaManifoldMass = (USERCAL_stRAMCAL.u16ManifoldVolumeCC * MAP_nAirDensMgpL) / 1000; 
		s32DeltaManifoldMass *= s32DeltaMAP;
        MAP_s32ManDeltaChargeMassPerSUg = (MAP_nRunFreq * -s32DeltaManifoldMass) / 10000;	
        MAP_tKiloPaOld = MAP_tKiloPaFiltered;
	}

	MAP_boHighVacuum = USERCAL_stRAMCAL.u16HighVacuumEnableKpa > MAP_tKiloPaFiltered ? TRUE : MAP_boHighVacuum;
	MAP_boHighVacuum = USERCAL_stRAMCAL.u16HighVacuumDisableKpa < MAP_tKiloPaFiltered ? FALSE : MAP_boHighVacuum;

	MAP_boBoostETCCutEnable = 140000 < MAP_tKiloPaFiltered ? TRUE : MAP_boBoostETCCutEnable;
	MAP_boBoostETCCutEnable = 101300 > MAP_tKiloPaFiltered ? FALSE : MAP_boBoostETCCutEnable;

	u8Temp = ((7 > TORQUE_u8ATXSelectedGear) && (0 < TORQUE_u8ATXSelectedGear)) ? TORQUE_u8ATXSelectedGear - 1 : 0;

	if (TRUE == USERCAL_stRAMCAL.u8LaunchBoostEnable)
	{
		if (USERCAL_stRAMCAL.u16LaunchMidVSS > SENSORS_u16CANVSS)
		{
			u32Temp = SENSORS_u16CANVSS * USERCAL_stRAMCAL.u16LaunchMidBoostMax;
			u32Temp += (USERCAL_stRAMCAL.u16LaunchMidVSS - SENSORS_u16CANVSS) * USERCAL_stRAMCAL.u16LaunchStoppedBoostMax;
			u32Temp /= USERCAL_stRAMCAL.u16LaunchMidVSS;
			u32Temp *= 10;
		}
		else if (USERCAL_stRAMCAL.u16LaunchEndVSS > SENSORS_u16CANVSS)
		{
			u32Temp = (SENSORS_u16CANVSS - USERCAL_stRAMCAL.u16LaunchMidVSS) * USERCAL_stRAMCAL.u16LaunchEndBoostMax;
			u32Temp += (USERCAL_stRAMCAL.u16LaunchEndVSS - SENSORS_u16CANVSS) * USERCAL_stRAMCAL.u16LaunchMidBoostMax;
			u32Temp /= (USERCAL_stRAMCAL.u16LaunchEndVSS - USERCAL_stRAMCAL.u16LaunchMidVSS);
			u32Temp *= 10;
		}
		else
		{
			/* Simple boost error based on gear boost target */
			u32Temp = USERCAL_stRAMCAL.au16BoostTarget[u8Temp] * 10;
		}
	}
	else
	{
		/* Simple boost error based on gear boost target */
		u32Temp = USERCAL_stRAMCAL.au16BoostTarget[u8Temp] * 10;
	}

	if (MAP_tBoostTarget > u32Temp)
	{
		MAP_tBoostTarget = u32Temp;
	}
	else
	{
		if (MAP_BOOST_POS_DELTA_PER_MS < (u32Temp - MAP_tBoostTarget))
		{
			if (0 != TORQUE_u16GearShiftCount)
			{
				MAP_tBoostTarget += 10;
			}
			else
			{
				MAP_tBoostTarget += MAP_BOOST_POS_DELTA_PER_MS;
			}
		}
		else
		{
			MAP_tBoostTarget = u32Temp;
		}
	}

	s32Temp = (sint32)MAP_tKiloPaFiltered - (sint32)MAP_tBoostTarget;

	if (s32Temp > (MAP_tKiloPaTargetError + USERCAL_stRAMCAL.u16PressureControlHyst))
	{
		MAP_tKiloPaTargetError = s32Temp - USERCAL_stRAMCAL.u16PressureControlHyst;
	}
	else if (s32Temp < (MAP_tKiloPaTargetError - USERCAL_stRAMCAL.u16PressureControlHyst))
	{
		MAP_tKiloPaTargetError = s32Temp - USERCAL_stRAMCAL.u16PressureControlHyst;
	}



	if (1 == (u32Count % 5))
	{
		s32ErrorDerivativeFiltered = (s32ErrorDerivativeFiltered / 2) +
				((MAP_tKiloPaTargetError - s32ErrorOld) / 2);

		s32ErrorOld = MAP_tKiloPaTargetError;

		if (0 > s32ErrorDerivativeFiltered)
		{
			s32Temp = s32ErrorDerivativeFiltered;
			s32Temp *= USERCAL_stRAMCAL.u16PressureControlDNeg;
		}
		else if (0 < s32ErrorDerivativeFiltered)
		{
			s32Temp = s32ErrorDerivativeFiltered;
			s32Temp *= USERCAL_stRAMCAL.u16PressureControlDPos;
		}
		else
		{
			s32Temp = 0;
		}


		/* Calculate the current spread for pressure valve feedforward */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadPressureValveFeedForwardIDX,
				NULL, NULL);

		/* Lookup the current pressure valve feedforward */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&MAP_tTablePressureValveFeedForwardIDX,
				NULL, NULL);

		CPU_vEnterCritical();

		if (0 < TORQUE_u16GearShiftPressureControlCount)
		{
			MAP_u16PressureValveDuty = USERCAL_stRAMCAL.u16ShiftPressureControl * 650;
		}
		else
		{
			if (0 < s32Temp)
			{
				if (0 < ((sint32)MAP_u16PressureValveFeedForward - s32Temp))
				{
					MAP_u16PressureValveDuty = (uint16)((sint32)MAP_u16PressureValveFeedForward - s32Temp);
				}
				else
				{
					MAP_u16PressureValveDuty = 0;
				}
			}
			else
			{
				if (0xffff > ((sint32)MAP_u16PressureValveFeedForward - s32Temp))
				{
					MAP_u16PressureValveDuty = (uint16)((sint32)MAP_u16PressureValveFeedForward - s32Temp);
				}
				else
				{
					MAP_u16PressureValveDuty = 0xffff;
				}

			}
		}

		CPU_vExitCritical();

		if (-USERCAL_stRAMCAL.u16PressureControlThreshold < MAP_tKiloPaTargetError)
		{
			/* Overboost */
			u32Temp = (USERCAL_stRAMCAL.u16ThrottlePressureControlCal *
					(MAP_tKiloPaTargetError + USERCAL_stRAMCAL.u16PressureControlThreshold)) / 160000;

			if (31 > u32Temp)
			{
				MAP_u8PressureThrottleTrim = 0x3f - u32Temp;
			}
			else
			{
				MAP_u8PressureThrottleTrim = 0x21;
			}
		}
		else
		{
			MAP_u8PressureThrottleTrim = 0x3f;
		}
	}

	u32Count++;
}

void MAP_vTerminate(puint32 const pu32Arg)
{

}


void MAP_vCallBack(puint32 const pu32Arg)
{

}


static void MAP_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	MAP_u32ADCRaw = u32ADCResult;
	MAP_boNewSample = TRUE;
}

#endif //BUILD_USER
