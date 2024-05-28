
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
#include "DIAG.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
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
	TORQUE_u32OutputTorqueEstimate = 0x10;

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
	uint32 s32Temp;
	uint16 u16Temp;
	uint16 u16AutoRevMatch;
	uint16 u16PostBlipDuration;

	CODE_UPDATE_EXIT();

	if (USERCAL_stRAMCAL.u16ETCOverrideKeys == 0xffff)
	{
		SENSORS_u16CANVSS = 500;
	}

	TORQUE_boVehicleMovingDS = USERCAL_stRAMCAL.u16ATXTorqueOnVSS < SENSORS_u16CANVSS ? TRUE : TORQUE_boVehicleMovingDS;
	TORQUE_boVehicleMovingDS = USERCAL_stRAMCAL.u16ATXTorqueOffVSS > SENSORS_u16CANVSS ? FALSE : TORQUE_boVehicleMovingDS;
	TORQUE_boVehicleMovingUS = 150 < SENSORS_u16CANVSS ? TRUE : TORQUE_boVehicleMovingUS;
	TORQUE_boVehicleMovingUS = 50 > SENSORS_u16CANVSS ? FALSE : TORQUE_boVehicleMovingUS;

	/* Calculate max torque */
	if (3000 > CAM_u32RPMFiltered)
	{
		if (1700 < CAM_u32RPMFiltered)
		{
			u32Temp = CAM_u32RPMFiltered - 1000;
			u32Temp *= 65000;
			u32Temp /= 2000;
		}
		else
		{
			u32Temp = 22700;
		}

		u16Temp = u32Temp;
	}
	else
	{
		u16Temp = 65000;
	}

	/* Store max torque */
	TORQUE_u32TorqueEstimateScale = u16Temp & 0xff00;

	/* Calculate min torque */
	if (2000 > CAM_u32RPMFiltered)
	{
		u16Temp = 2000 - CAM_u32RPMFiltered;
		u16Temp /= 64;
		u16Temp += 16;
	}
	else
	{
		u16Temp = 16;
	}

	/* Store min torque */
	TORQUE_u32TorqueEstimateScale += (u16Temp & 0xff);

	/* New torque estimate */
	if ((37000 - 2 * CAM_u32RPMFiltered) < MAP_tKiloPaFiltered)
	{
		s32Temp = (MAP_tKiloPaFiltered - (37000 - 2 * CAM_u32RPMFiltered)) / 660;
		u32Temp = s32Temp > 0 ? (uint32)s32Temp : 0;
		u32Temp += 5;
		u32Temp = 0xff > u32Temp ? u32Temp : 0xff;
	}
	else
	{
		u32Temp = 5;
	}

	/* Ramp torque estimate */

	/* Increasing torque */
	TORQUE_u32OutputTorqueEstimate =
			u32Temp > (TORQUE_u32OutputTorqueEstimate + 3) ?
					TORQUE_u32OutputTorqueEstimate + 1 : TORQUE_u32OutputTorqueEstimate;

	/* Decreasing torque */
	TORQUE_u32OutputTorqueEstimate =
			u32Temp < (TORQUE_u32OutputTorqueEstimate - 3) ?
					TORQUE_u32OutputTorqueEstimate - 1 : TORQUE_u32OutputTorqueEstimate;

	/* Safety catch */
	TORQUE_u32OutputTorqueEstimate &= 0xff;

	/* Calculate pedal percentage scaled */
#ifdef BUILD_USE_PPSS_AS_PPSM
	u32Temp = MAX(USERCAL_stRAMCAL.userCalPPSCalMin, SENSORS_u32PPSSVolts * 2);
#else
	u32Temp = MAX(USERCAL_stRAMCAL.userCalPPSCalMin, SENSORS_u32PPSMVolts);
#endif
	u32Temp = MIN(USERCAL_stRAMCAL.userCalPPSCalMax, u32Temp);
	u32Temp -= USERCAL_stRAMCAL.userCalPPSCalMin;

	u32Temp *= TORQUE_nPPSRange;
	u32Temp /= (USERCAL_stRAMCAL.userCalPPSCalMax -
							USERCAL_stRAMCAL.userCalPPSCalMin);

	TORQUE_u32TorquePedalEstimateScaled = u32Temp;

	TORQUE_u32PedalWOT = 24500 > TORQUE_u32TorquePedalEstimateScaled ? FALSE : TORQUE_u32PedalWOT;
	TORQUE_u32PedalWOT = 25200 < TORQUE_u32TorquePedalEstimateScaled ? TRUE : TORQUE_u32PedalWOT;

	/* Apply ATX torque mode */
	if ((EST_nIgnitionReqDSGStage1 == EST_enIgnitionTimingRequest)
			&& (TRUE == TORQUE_boVehicleMovingUS))
	{
		if (0 == TORQUE_boDownShift)
		{
			u16Temp = USERCAL_stRAMCAL.u16ShiftUpCountLimit - 5;
		}
		else
		{
			u16Temp = USERCAL_stRAMCAL.u16ShiftDownCountLimit / 2;
		}

		if (u16Temp < TORQUE_u16GearShiftCount)
		{
			/* Early in the shift */
			if (TRUE == TORQUE_boManualShiftMode)
			{
				TORQUE_u32ESTTorqueModifier = 10;
			}
			else
			{
				TORQUE_u32ESTTorqueModifier = 110;
			}
		}
		else
		{
			if (150 > SENSORS_u16VSSDSGGearRPMSlip)
			{
				if (TRUE == TORQUE_boManualShiftMode)
				{
					/* Manual mode */
					TORQUE_u32ESTTorqueModifier = 245 > TORQUE_u32ESTTorqueModifier ?
							TORQUE_u32ESTTorqueModifier + 5 : 0x100;
				}
				else
				{
					/* Auto mode */
					TORQUE_u32ESTTorqueModifier = 245 > TORQUE_u32ESTTorqueModifier ?
							TORQUE_u32ESTTorqueModifier + 2 : 0x100;
				}
			}
			else if (250 < SENSORS_u16VSSCalcGearRPMSlip)
			{
				if (TRUE == TORQUE_boManualShiftMode)
				{
					/* Manual mode */
					TORQUE_u32ESTTorqueModifier = 20 < TORQUE_u32ESTTorqueModifier ?
							TORQUE_u32ESTTorqueModifier - 5 : 20;
				}
				else
				{
					/* Auto mode */
					TORQUE_u32ESTTorqueModifier = 110 < TORQUE_u32ESTTorqueModifier ?
							TORQUE_u32ESTTorqueModifier - 5 : 110;
				}
			}
		}

		if (TRUE == TORQUE_boManualShiftMode)
		{
			TORQUE_u32FuelTorqueModifier = 220;
		}
		else
		{
			TORQUE_u32FuelTorqueModifier = 240;
		}

		TORQUE_u32OutputTorqueModified = (TORQUE_u32OutputTorqueEstimate *
				TORQUE_u32ESTTorqueModifier *
				TORQUE_u32DBWTorqueModifier *
				TORQUE_u32FuelTorqueModifier) / 0x1000000;
	}
	else
	{
		/* No torque modification if no shift or vehicle not moving */
		TORQUE_u32OutputTorqueModified = TORQUE_u32OutputTorqueEstimate;
		TORQUE_u32ESTTorqueModifier = 0x100;
		TORQUE_u32DBWTorqueModifier = 0x100;
		TORQUE_u32FuelTorqueModifier = 0x100;
		TORQUE_u32IdleStabilisationTorque = 20;
	}

	/* Lookup the current Torque Pedal */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&TORQUE_tTableTorquePedalIDX,
			NULL, NULL);

	/* Calculate the current spread for ETC scale */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&TORQUE_tSpreadETCScaleIDX,
			NULL, NULL);

	/* Lookup the current ETC scale */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&TORQUE_tTableETCScaleIDX,
			NULL, NULL);

	TORQUE_u16ETCScaleRamped = TORQUE_u16ETCScale < TORQUE_u16ETCScaleRamped ? TORQUE_u16ETCScaleRamped - 1 : TORQUE_u16ETCScaleRamped;
	TORQUE_u16ETCScaleRamped = TORQUE_u16ETCScale > TORQUE_u16ETCScaleRamped ? TORQUE_u16ETCScaleRamped + 1 : TORQUE_u16ETCScaleRamped;


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
								/* Calculate desired TPS */
								u32Temp = SENSORS_u32PPSMVolts;
								u32Temp *= 4;
								u32Temp /= 10;
								u32Temp += 250;

								/* Calculate target step */
								u32Temp -= 470;
								u32Temp /= 58;

								u16AutoRevMatch = TORQUE_u16GetAutoRevMatch();
								u16AutoRevMatch = u32Temp > u16AutoRevMatch ? u32Temp : u16AutoRevMatch;

								u16Temp -= 50;
								u16PostBlipDuration -= 50;

								TORQUE_u16RevMatchPosition = u16AutoRevMatch;
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

	if ((TORQUE_MIN_FCUTS_MAP > MAP_tKiloPaFiltered) ||
		(FALSE == TORQUE_boManualShiftMode) ||
		(TORQUE_MIN_FCUTS_RPM > CAM_u32RPMFiltered))
	{
		TORQUE_u32QuickCutPercent = 0;
		TORQUE_u32QuickCutDuration = 0;

	}
	else
	{
		CPU_vEnterCritical();

		/* Base engine speed related cut percent */
		TORQUE_u32QuickCutPercent = (CAM_u32RPMFiltered - TORQUE_MIN_FCUTS_RPM) / 40;

		/* Manifold pressure modification */
		if (TORQUE_MAX_FCUTS_MAP > MAP_tKiloPaFiltered)
		{
			u32Temp = MAP_tKiloPaFiltered - TORQUE_MIN_FCUTS_MAP;
			u32Temp = TORQUE_MAX_FCUTS_MAP - u32Temp;
			u32Temp /= ((TORQUE_MAX_FCUTS_MAP - TORQUE_MIN_FCUTS_MAP) / 100u);
		}
		else
		{
			u32Temp = 0;
		}

		TORQUE_u32QuickCutPercent *= u32Temp;
		TORQUE_u32QuickCutPercent /= 100;
		TORQUE_u32QuickCutPercent = 100 > TORQUE_u32QuickCutPercent ? TORQUE_u32QuickCutPercent : 100;

		TORQUE_u32QuickCutDuration = CAM_u32RPMFiltered >> 16;

		CPU_vExitCritical();
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
