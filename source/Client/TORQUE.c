
/******************************************************************************/
/*    Copyright (c) 2020 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Torque Control Module                                  */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for throttle position          */
/*                     measurement                                            */
/* FILE NAME:          TORQUE.c                                               */
/* REVISION HISTORY:   01-04-2020 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _TORQUE_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "TPS.h"
#include "SENSORS.h"
#include "usercal.h"
#include "EST.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 TORQUE_u32OutputTorqueEstimateScaled;
SPREADAPI_ttSpreadIDX TORQUE_tSpreadTorquePedalIDX;
TABLEAPI_ttTableIDX TORQUE_tTableTorquePedalIDX;
SPREADAPI_ttSpreadIDX TORQUE_tSpreadETCScaleIDX;
TABLEAPI_ttTableIDX TORQUE_tTableETCScaleIDX;
SPREADAPI_ttSpreadIDX TORQUE_tSpreadRevMatchIDX;
TABLEAPI_ttTableIDX TORQUE_tTableRevMatchIDX;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static uint16 TORQUE_u16GetAutoRevMatch(void);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void TORQUE_vStart(puint32 const pu32Arg)
{
	/* Request and initialise required Kernel managed spread for ETC scale function */
	TORQUE_tSpreadETCScaleIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserETCScaleSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for ETC scale function */
	TORQUE_tTableETCScaleIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserETCScaleTable, (void*)&TORQUE_u16ETCScale, TYPE_enUInt16, 17, TORQUE_tSpreadETCScaleIDX, NULL);

	/* Request and initialise required Kernel managed spread for ETC rev match */
	TORQUE_tSpreadRevMatchIDX = SETUP_tSetupSpread((void*)&TORQUE_u32RevMatchRPM, (void*)&USERCAL_stRAMCAL.aUserETCRPMMatchSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for launch and flat shift fuel cuts */
	TORQUE_tTableRevMatchIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserETCRPMMatchTable, (void*)&TORQUE_u16RevMatchPosition, TYPE_enUInt16, 17, TORQUE_tSpreadRevMatchIDX, NULL);
}

void TORQUE_vRun(puint32 const pu32Arg)
{
	uint32 u32Temp;
	uint16 u16Temp;
	uint16 u16AutoRevMatch;
	uint16 u16PostBlipDuration;
	static uint32 u32DSGCutsCount;

	TORQUE_boVehicleMovingDS = USERCAL_stRAMCAL.u16ATXTorqueOnVSS < SENSORS_u16CANVSS ? TRUE : TORQUE_boVehicleMovingDS;
	TORQUE_boVehicleMovingDS = USERCAL_stRAMCAL.u16ATXTorqueOffVSS > SENSORS_u16CANVSS ? FALSE : TORQUE_boVehicleMovingDS;
	TORQUE_boVehicleMovingUS = 150 < SENSORS_u16CANVSS ? TRUE : TORQUE_boVehicleMovingUS;
	TORQUE_boVehicleMovingUS = 50 > SENSORS_u16CANVSS ? FALSE : TORQUE_boVehicleMovingUS;

	if (10000 < MAP_tKiloPaFiltered)
	{
		if (239000 > MAP_tKiloPaFiltered)
		{
			TORQUE_u32TorqueModelEstimateScaled	= 2 * ((MAP_tKiloPaFiltered - 10000) / 7);
		}
		else
		{
			TORQUE_u32TorqueModelEstimateScaled	= 65500;
		}
	}
	else
	{
		TORQUE_u32TorqueModelEstimateScaled	= 0;
	}


	if (26000 >= TPS_tThetaFiltered)
	{
		if (6000 < TPS_tThetaFiltered)
		{
			u32Temp = (TPS_tThetaFiltered - 6000) / 120;
		}
		else
		{
			u32Temp = 0;
		}
	}
	else
	{
		u32Temp = 164 + (TPS_tThetaFiltered - 26000) / 700;
	}

	if (20000 < MAP_tKiloPaFiltered)
	{
		if (122000 > MAP_tKiloPaFiltered)
		{
			TORQUE_u32OutputTorqueEstimate = u32Temp / 2 + ((MAP_tKiloPaFiltered - 20000) / 800);
		}
		else
		{
			TORQUE_u32OutputTorqueEstimate = u32Temp / 2 + 127;
		}
	}
	else
	{
		TORQUE_u32OutputTorqueEstimate = u32Temp / 2;
	}

	/* Calculate pedal percentage scaled */
	u32Temp = MAX(USERCAL_stRAMCAL.userCalPPSCalMin, SENSORS_u32PPSMVolts);
	u32Temp = MIN(USERCAL_stRAMCAL.userCalPPSCalMax, u32Temp);
	u32Temp -= USERCAL_stRAMCAL.userCalPPSCalMin;

	u32Temp *= TORQUE_nPPSRange;
	u32Temp /= (USERCAL_stRAMCAL.userCalPPSCalMax -
							USERCAL_stRAMCAL.userCalPPSCalMin);

	TORQUE_u32TorquePedalEstimateScaled = u32Temp;

	/* Apply ATX torque mode */
	if ((EST_nIgnitionReqDSGStage1 == EST_enIgnitionTimingRequest)
			&& (TRUE == TORQUE_boVehicleMovingUS))
	{
		if (0 == TORQUE_boDownShift)
		{
			u16Temp = (3 * USERCAL_stRAMCAL.u16ShiftUpCountLimit) / 4;
		}
		else
		{
			u16Temp = USERCAL_stRAMCAL.u16ShiftDownCountLimit / 2;
		}

		if (u16Temp < TORQUE_u16GearShiftCount)
		{
			/* Early in the shift */
			TORQUE_u32ESTTorqueModifier = 10;
		}
		else
		{
			if (100 > SENSORS_u16VSSCalcGearRPMSlip)
			{
				/* OK slip is low, ramp up modifier */
				TORQUE_u32ESTTorqueModifier = 215 > TORQUE_u32ESTTorqueModifier ?
						TORQUE_u32ESTTorqueModifier + 40 : 0x100;
			}
			else if (200 < SENSORS_u16VSSCalcGearRPMSlip)
			{
				/* Uh oh slip is high */
				TORQUE_u32ESTTorqueModifier = 40 < TORQUE_u32ESTTorqueModifier ?
						TORQUE_u32ESTTorqueModifier - 40 : 10;
			}
		}


		TORQUE_u32FuelTorqueModifier = 220;

		TORQUE_u32OutputTorqueModified = (TORQUE_u32OutputTorqueEstimate *
				TORQUE_u32ESTTorqueModifier *
				TORQUE_u32DBWTorqueModifier *
				TORQUE_u32FuelTorqueModifier) / 0x1000000;

		u32DSGCutsCount = 0;
	}
	else if (((EST_nIgnitionReqDSGStage2 == EST_enIgnitionTimingRequest) ||
			(EST_nIgnitionReqDSGCutsStage3 == EST_enIgnitionTimingRequest))
					&& (TRUE == TORQUE_boVehicleMovingUS))
	{
		if (u32DSGCutsCount > USERCAL_stRAMCAL.u16TorqueReductionMaxDuration)
		{
			EST_enIgnitionTimingRequest = EST_nIgnitionReqDSGStage2;
		}
		else
		{
			u32DSGCutsCount++;
			EST_enIgnitionTimingRequest = EST_nIgnitionReqDSGCutsStage3;
		}

		TORQUE_u32FuelTorqueModifier = 200;

		TORQUE_u32OutputTorqueModified = (TORQUE_u32OutputTorqueEstimate *
				TORQUE_u32ESTTorqueModifier *
				TORQUE_u32DBWTorqueModifier *
				TORQUE_u32FuelTorqueModifier) / 0x1000000;

		if ((TORQUE_u32ATXTorqueLimit - 20) < TORQUE_u32OutputTorqueModified)
		{
			TORQUE_u32ESTTorqueModifier--;
		}
		else if ((TORQUE_u32ATXTorqueLimit - 20) > TORQUE_u32OutputTorqueModified)
		{
			if (200 > TORQUE_u32ESTTorqueModifier) TORQUE_u32ESTTorqueModifier++;
		}
	}
	else
	{
		/* No torque modification if no shift or vehicle not moving */
		TORQUE_u32OutputTorqueModified = TORQUE_u32OutputTorqueEstimate;
		TORQUE_u32ESTTorqueModifier = 0x100;
		TORQUE_u32DBWTorqueModifier = 0x100;
		TORQUE_u32FuelTorqueModifier = 0x100;
		TORQUE_u32IdleStabilisationTorque = 20;
		u32DSGCutsCount = 0;
	}

	/* Reverse lookup pedal from torque */
	TORQUE_u32OutputTorqueEstimateScaled = 100 * TORQUE_u32OutputTorqueEstimate;

	/* Lookup the current Torque Pedal */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&TORQUE_tTableTorquePedalIDX,
			NULL, NULL);

	/* Calculate the current spread for ETC scale */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&TORQUE_tSpreadETCScaleIDX,
			NULL, NULL);

	/* Lookup the current ETC scale */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&TORQUE_tTableETCScaleIDX,
			NULL, NULL);


	/* Compute Rev Match RPM */
	if (0 != TORQUE_u16GearShiftCount)
	{
		if ((TRUE == TORQUE_boDownShift) && (TRUE == TORQUE_boVehicleMovingDS))
		{
			if ((6 > TORQUE_u8ATXSelectedGear) && (0 < TORQUE_u8ATXSelectedGear))
			{
				if (TRUE == TORQUE_boManualShiftMode)
				{
					u32Temp = 1000 * SENSORS_u16CANVSS;

					TORQUE_u32RevMatchRPM = (uint16)(u32Temp / USERCAL_stRAMCAL.u16VSSPerRPM[TORQUE_u8ATXSelectedGear - 1]);

					/* Calculate the current spread for Rev Match scale */
					USER_vSVC(SYSAPI_enCalculateSpread, (void*)&TORQUE_tSpreadRevMatchIDX,
							NULL, NULL);

					/* Lookup the current Rev Match ETC position */
					USER_vSVC(SYSAPI_enCalculateTable, (void*)&TORQUE_tTableRevMatchIDX,
							NULL, NULL);

					if (USERCAL_stRAMCAL.u16ShiftDownCountLimit > USERCAL_stRAMCAL.u16ShiftDownBlipLimit)
					{
						/* How long into the down-shift? */
						u16Temp = USERCAL_stRAMCAL.u16ShiftDownCountLimit - TORQUE_u16GearShiftCount;

						if (u16Temp > USERCAL_stRAMCAL.u16ShiftDownBlipLimit)
						{
							/* Only proceed to modify TORQUE_u16RevMatchPosition AFTER the blip */
							u16Temp -= USERCAL_stRAMCAL.u16ShiftDownBlipLimit;
							u16PostBlipDuration = USERCAL_stRAMCAL.u16ShiftDownCountLimit - USERCAL_stRAMCAL.u16ShiftDownBlipLimit;
							u16PostBlipDuration = 60 > u16PostBlipDuration ? 60 : u16PostBlipDuration;

							if (30 > u16Temp)
							{
								TORQUE_u16RevMatchPosition *= (30 - u16Temp);
								TORQUE_u16RevMatchPosition /= 30;
							}
							else if ((30 <= u16Temp) && (50 > u16Temp))
							{
								TORQUE_u16RevMatchPosition = 0;
							}
							else
							{
								u16AutoRevMatch = TORQUE_u16GetAutoRevMatch();
								u16Temp -= 50;
								u16PostBlipDuration -= 50;

								TORQUE_u16RevMatchPosition *= u16Temp;
								TORQUE_u16RevMatchPosition /= u16PostBlipDuration;
							}
						}
					}
				}
				else
				{
					TORQUE_u16RevMatchPosition = TORQUE_u16GetAutoRevMatch();

					if (USERCAL_stRAMCAL.u16ShiftDownCountLimit > USERCAL_stRAMCAL.u16ShiftDownBlipLimit)
					{
						/* How long into the down-shift? */
						u16Temp = USERCAL_stRAMCAL.u16ShiftDownCountLimit - TORQUE_u16GearShiftCount;

						if (u16Temp > USERCAL_stRAMCAL.u16ShiftDownBlipLimit)
						{
							TORQUE_u16RevMatchPosition *= TORQUE_u16GearShiftCount;
							u16Temp = USERCAL_stRAMCAL.u16ShiftDownCountLimit - USERCAL_stRAMCAL.u16ShiftDownBlipLimit;
							TORQUE_u16RevMatchPosition /= u16Temp;
						}
					}
				}
			}
		}
		else
		{
			TORQUE_u32RevMatchRPM = 1500;
			TORQUE_u16RevMatchPosition = 0;
		}
	}
	else
	{
		TORQUE_u32RevMatchRPM = 1500;
		TORQUE_u16RevMatchPosition = 0;
	}
}

void TORQUE_vTerminate(puint32 const pu32Arg)
{

}

void TORQUE_vCallBack(puint32 const pu32Arg)
{

}

static uint16 TORQUE_u16GetAutoRevMatch(void)
{
	uint32 u32Temp;
	uint16 u16Ret;

	if (0 > CTS_tTempCFiltered)
	{
		u16Ret = USERCAL_stRAMCAL.u16ColdOffThrottleBlip;
	}
	else if (50000 < CTS_tTempCFiltered)
	{
		u16Ret = USERCAL_stRAMCAL.u16HotOffThrottleBlip;
	}
	else
	{
		u32Temp = USERCAL_stRAMCAL.u16HotOffThrottleBlip * CTS_tTempCFiltered;
		u32Temp += (USERCAL_stRAMCAL.u16ColdOffThrottleBlip * (50000 - CTS_tTempCFiltered));
		u16Ret = u32Temp / 50000;
	}

	return u16Ret;
}


#endif //BUILD_USER
