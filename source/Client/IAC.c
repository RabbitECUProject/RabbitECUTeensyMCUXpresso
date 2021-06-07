/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Idle Air Control                                       */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for idle air control           */
/*                                                                            */
/* FILE NAME:          IAC.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _IAC_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "IAC.h"

#include "usercal.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
#ifdef BUILD_BSP_IAC_STEPPER
uint32 IAC_u32TargetStep;
uint32 IAC_u32TargetStepNormalised;
uint32 IAC_u32FeedbackStep;
// mode=readvalue name="IAC Step" type=uint32 offset=0 min=0 max=255 m=1 b=0 units="dl" format=5.3 help="IAC Current Stepper Position"
uint32 IAC_u32StepsLearnedMin;
// mode=readvalue name="IAC Step Learned Min" type=uint32 offset=0 min=0 max=255 m=1 b=0 units="dl" format=5.3 help="IAC Learned Min Position"
IAC_tenStepperState IAC_enStepperState;
uint32 IAC_u32StepCounter;
uint32 IAC_u32StepCounterLimit;
uint8 IAC_u8StepArray[4];
#endif

uint32 IAC_u32IdleEntryTimer;
uint32 IAC_u32RPMLongAverage;
SPREADAPI_ttSpreadIDX IAC_tSpreadISCTargetIDX;
TABLEAPI_ttTableIDX IAC_tTableISCTargetIDX;
SPREADAPI_ttSpreadIDX IAC_tSpreadOpenLoopPosIDX;
TABLEAPI_ttTableIDX IAC_tTableOpenLoopPosIDX;
uint16 IAC_u16ISCTarget;
uint16 IAC_u16OpenLoopPos;
uint16 IAC_u16ISCTargetRamp;
//ASAM mode=readvalue name="IAC Target RPM" type=uint16 offset=0 min=0 max=4095 m=1 b=0 units="RPM" format=4.0 help="IAC Current Target RPM"
uint16 IAC_u16ISCTargetRampOld;
bool IAC_boRun;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
#ifdef BUILD_BSP_IAC_STEPPER
static bool IAC_boUnderStepMins(void);
static uint32 IAC_u32GetOpenStepsTempNormalised(uint32 u32Temp);
#endif //BUILD_BSP_IAC_STEPPER

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void IAC_vStart(puint32 const pu32Arg)
{
#ifdef BUILD_BSP_IAC_STEPPER
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;	

	IAC_u32TargetStep = 0;
	IAC_u32FeedbackStep = 0;
	IAC_enStepperState = IAC_enResetHome;
	IAC_u32StepCounter = 0;
	IAC_u32StepCounterLimit = 0;	

	enEHIOType = IOAPI_enDIOOutput;	
	enDriveStrength = IOAPI_enStrong;
			
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[0])	
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[0];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}	
	
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[1])
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[1];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}
	
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[2])
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[2];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}
	
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[3])
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[3];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}				


#endif

	IAC_u32IdleEntryTimer = 0;
	IAC_u32RPMLongAverage = 0;
	IAC_enControlState = IAC_enOpenLoop;

	/* Request and initialise required Kernel managed spread for ISC target spread*/
	IAC_tSpreadISCTargetIDX = SETUP_tSetupSpread((void*)&CTS_tTempCFiltered, (void*)&USERCAL_stRAMCAL.aUserISCSpeedTargetSpread , TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for ISC target */
	IAC_tTableISCTargetIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserISCSpeedTargetTable, (void*)&IAC_u16ISCTarget, TYPE_enUInt16, 17, IAC_tSpreadISCTargetIDX, NULL);

	/* Request and initialise required Kernel managed spread for ISC open loop position spread*/
	IAC_tSpreadOpenLoopPosIDX = SETUP_tSetupSpread((void*)&CTS_tTempCFiltered, (void*)&USERCAL_stRAMCAL.aUserISCOpenLoopPosSpread , TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for ISC open loop position  */
	IAC_tTableOpenLoopPosIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserISCOpenLoopPosTable, (void*)&IAC_u16OpenLoopPos, TYPE_enUInt16, 17, IAC_tSpreadOpenLoopPosIDX, NULL);
}

void IAC_vRun(puint32 const pu32Arg)
{
#ifdef BUILD_BSP_IAC_STEPPER
	IOAPI_tenTriState enTriState;	
	IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32OpenLoopTargetStep;
	uint32 u32OpenLoopTargetStepNormalised;
	uint32 u32TempClipped;
	static uint32 u32StepsLearnedCount;
#endif

	uint32 u32Temp;
	sint32 s32Temp;
	static sint32 s32RPMErrOld;
	sint32 s32ErrDelta;
	sint32 s32ErrDeltaAbs;
	uint32 u32IdleEntryRPM;
	static uint32 u32ISCCount;
	static uint16 u16TargetRamp = 200;
	static uint16 u16AfterStartTargetRamp = 200;
	static uint32 u32OBDAdvFiltered = 500 * (0x80 + 40);
	static uint16 u16ISCMin = 0;
	static uint16 u16ISCMax = 4096;
	uint16 u16TempMin;
	uint16 u16TempMax;

	if (TRUE == IAC_boRun)
	{
		/* Calculate the current spread for ISC target */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&IAC_tSpreadISCTargetIDX,
		NULL, NULL);

		/* Lookup the current value for ISC target */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&IAC_tTableISCTargetIDX,
		NULL, NULL);

		/* Calculate the current spread for ISC target */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&IAC_tSpreadOpenLoopPosIDX,
		NULL, NULL);

		/* Lookup the current value for ISC open loop position */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&IAC_tTableOpenLoopPosIDX,
		NULL, NULL);

		/* Correct open loop position for after start alternator load */
		IAC_u16OpenLoopPos += ((13000 - BVM_tCrankBattVolts) / 30);
		IAC_u16OpenLoopPos = 256 > IAC_u16OpenLoopPos ? IAC_u16OpenLoopPos : 255;

		if ((TRUE == USERCAL_stRAMCAL.u8VehicleStoppedFuelCutEnable) ||
				((FALSE == USERCAL_stRAMCAL.u8VehicleStoppedFuelCutEnable) &&
				(TRUE == TORQUE_boVehicleMovingUS)))
		{
			IAC_boOverrunCutRPMEnable = (IAC_u16ISCTarget + USERCAL_stRAMCAL.u16OverrunCutEnableRPM) > CAM_u32RPMRaw ? FALSE : IAC_boOverrunCutRPMEnable;
			IAC_boOverrunCutRPMEnable = (IAC_u16ISCTarget + USERCAL_stRAMCAL.u16OverrunCutDisableRPM) < CAM_u32RPMRaw ? TRUE : IAC_boOverrunCutRPMEnable;
		}
		else
		{
			IAC_boOverrunCutRPMEnable = FALSE;
		}

		u32IdleEntryRPM = IAC_u16ISCTarget + USERCAL_stRAMCAL.u16IdleEntryOffset;
		IAC_u16ISCTargetRamp = IAC_u16ISCTarget + u16TargetRamp + u16AfterStartTargetRamp;
	
		if (USERCAL_stRAMCAL.u16RPMRunThreshold < CAM_u32RPMRaw)
		{
			(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)CAM_u32RPMRaw, 
				0x10, &IAC_u32RPMLongAverage);	
		}
		else
		{
			IAC_u32RPMLongAverage = 0x100 * CAM_u32RPMRaw;
		}

		
		if ((u32IdleEntryRPM > CAM_u32RPMRaw) && 
				(TRUE == TPS_boThrottleClosed) &&
				(0 != CAM_u32RPMRaw))
		{
			s32Temp = (sint32)(IAC_u32RPMLongAverage / 0x100) - (sint32)CAM_u32RPMRaw;
			s32Temp = ABS(s32Temp);
		
			if (300 > s32Temp)
			{
				if (10 < IAC_u32IdleEntryTimer)
				{				
					IAC_enControlState = IAC_enClosedLoop;
				}
				else
				{
					IAC_u32IdleEntryTimer++;	
				}
			}
			else
			{
				IAC_u32IdleEntryTimer = 0;
			}
		}
		else
		{
			IAC_u32IdleEntryTimer = 0;
			IAC_enControlState = IAC_enOpenLoop;
		}

		if (0 > CTS_tTempCFiltered)
		{
			u16TempMin = USERCAL_stRAMCAL.u16ISCColdISCMin;
			u16TempMax = USERCAL_stRAMCAL.u16ISCColdISCMax;
		}
		else if (50000 < CTS_tTempCFiltered)
		{
			u16TempMin = USERCAL_stRAMCAL.u16ISCHotISCMin;
			u16TempMax = USERCAL_stRAMCAL.u16ISCHotISCMax;
		}
		else if ((USERCAL_stRAMCAL.u16ISCColdISCMin >= USERCAL_stRAMCAL.u16ISCHotISCMin) &&
				(USERCAL_stRAMCAL.u16ISCColdISCMax >= USERCAL_stRAMCAL.u16ISCHotISCMax))
		{
			u16TempMin = USERCAL_stRAMCAL.u16ISCHotISCMin +
					((USERCAL_stRAMCAL.u16ISCColdISCMin - USERCAL_stRAMCAL.u16ISCHotISCMin) * (50 - (CTS_tTempCFiltered / 1000)) /
						50);

			u16TempMax = USERCAL_stRAMCAL.u16ISCHotISCMax +
					((USERCAL_stRAMCAL.u16ISCColdISCMax - USERCAL_stRAMCAL.u16ISCHotISCMax) * (50 - (CTS_tTempCFiltered / 1000)) /
						50);
		}

		/* Ramp the min/max */
		if (400 > CAM_u32RPMFiltered)
		{
			u16ISCMin = 2048;
			u16ISCMax = 4096;
		}
		else
		{
			if ((u16TempMin + 10) < u16ISCMin)
			{
				u16ISCMin -= 10;
			}
			else if (u16TempMin > (u16ISCMin + 10))
			{
				u16ISCMin += 10;
			}

			if ((u16TempMax + 10) < u16ISCMax)
			{
				u16ISCMax -= 10;
			}
			else if (u16TempMax > (u16ISCMax + 10))
			{
				u16ISCMax += 10;
			}
		}


		if (IAC_enClosedLoop == IAC_enControlState)
		{
			if (TRUE == USERCAL_stRAMCAL.boOBDISCADV)
			{
				if (TRUE == SENSORS_boOBDAdvNewSample)
				{
					s32Temp = 500 * SENSORS_u8OBDAdv;
					u32OBDAdvFiltered = ((7 * u32OBDAdvFiltered) / 8) + s32Temp / 8;

					if ((128 * 500) < s32Temp)
					{
						IAC_s32ISCESTTrim[0] = s32Temp - 128 * 500;
						IAC_s32ISCESTTrim[1] = s32Temp - 128 * 500;
					}
					else
					{
						IAC_s32ISCESTTrim[0] = 0;
						IAC_s32ISCESTTrim[1] = 0;
					}

					SENSORS_boOBDAdvNewSample = FALSE;
				}
				else
				{
					if (((sint32)u32OBDAdvFiltered - 128 * 500 + 2000) < IAC_s32ISCESTTrim[0])
					{
						IAC_s32ISCESTTrim[0] -= 2000;
						IAC_s32ISCESTTrim[1] -= 2000;
					}
					if (((sint32)u32OBDAdvFiltered - 128 * 500 - 2000) > IAC_s32ISCESTTrim[0])
					{
						IAC_s32ISCESTTrim[0] += 2000;
						IAC_s32ISCESTTrim[1] += 2000;
					}
				}
			}
			else
			{
				if ((IAC_u16ISCTargetRamp - USERCAL_stRAMCAL.u16StallSaverRPMThreshold) > (uint16)CAM_u32RPMRaw)
				{
					/* Stall saver */
					u16TargetRamp = USERCAL_stRAMCAL.u16ISCEntryRamp;
				}

				/* Hold the rpm error in s32Temp */
				s32Temp = (sint32)IAC_u16ISCTargetRamp - (sint32)CAM_u32RPMRaw;

				if (IAC_u16ISCTargetRamp <= IAC_u16ISCTargetRampOld)
				{
					if ((IAC_u16ISCTargetRampOld - IAC_u16ISCTargetRamp) < 20)
					{
						s32ErrDelta = s32Temp - s32RPMErrOld;
					}
					else
					{
						s32ErrDelta = 0;
					}
				}
				else
				{
					if ((IAC_u16ISCTargetRamp - IAC_u16ISCTargetRampOld) < 20)
					{
						s32ErrDelta = s32Temp - s32RPMErrOld;
					}
					else
					{
						s32ErrDelta = 0;
					}
				}

				s32ErrDeltaAbs = ABS(s32ErrDelta);
				s32RPMErrOld = (sint32)IAC_u16ISCTargetRamp - (sint32)CAM_u32RPMRaw;
				IAC_u16ISCTargetRampOld = IAC_u16ISCTargetRamp;

				if (0 <= s32Temp)
				{
					s32Temp = 40 < s32Temp ? s32Temp - 40 : 0;
				}
				else
				{
					s32Temp = -40 > s32Temp ? s32Temp + 40 : 0;
				}

				if (20 > s32ErrDeltaAbs)
				{
					s32ErrDelta = 0;
				}
				else
				{
					if (0 > s32ErrDelta)
					{
						s32ErrDelta += 20;
						s32ErrDelta *= 2;
					}
					else
					{
						s32ErrDelta -= 20;
						s32ErrDelta *= 2;
					}
				}

				s32Temp /= 4;
				s32Temp += s32ErrDelta;

				if ((0 == TPS_u16CANTPSDeltaNegCount) && (0 == TPS_u16CANTPSDeltaPosCount))
				{
					if (0 <= s32Temp)
					{
						CPU_xEnterCritical();
						IAC_s32ISCESTTrim[0] = (sint32)USERCAL_stRAMCAL.u32ISCESTTrimPos > (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : USERCAL_stRAMCAL.u32ISCESTTrimPos;
						IAC_s32ISCESTTrim[1] = (sint32)USERCAL_stRAMCAL.u32ISCESTTrimPos > (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : USERCAL_stRAMCAL.u32ISCESTTrimPos;
						CPU_xExitCritical();
					}
					else
					{
						CPU_xEnterCritical();
						IAC_s32ISCESTTrim[0] = (sint32)~USERCAL_stRAMCAL.u32ISCESTTrimNeg < (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : (sint32)~USERCAL_stRAMCAL.u32ISCESTTrimNeg;
						IAC_s32ISCESTTrim[1] = (sint32)~USERCAL_stRAMCAL.u32ISCESTTrimNeg < (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : (sint32)~USERCAL_stRAMCAL.u32ISCESTTrimNeg;
						CPU_xExitCritical();
					}	
				}				
			}


#ifndef BUILD_BSP_IAC_STEPPER
			/* Closed loop airflow */
			s32Temp = (sint32)IAC_u16ISCTargetRamp - (sint32)CAM_u32RPMRaw;
			s32Temp = ABS(s32Temp);

			if (60 < s32Temp)
			{
				if (IAC_u16ISCTargetRamp > CAM_u32RPMRaw)
				{
					IAC_u32ISCDuty += (IAC_u16ISCTargetRamp - CAM_u32RPMRaw - 60);
					IAC_u32ISCDuty = 0x1000 < IAC_u32ISCDuty ? 0x1000 : IAC_u32ISCDuty;
				}
				else
				{
					u32Temp = CAM_u32RPMRaw - IAC_u16ISCTargetRamp - 60;
					u32Temp = 50 < u32Temp ? 50 : u32Temp;

					if (IAC_u32ISCDuty > u32Temp)
					{

						IAC_u32ISCDuty -= (u32Temp / 8);
					}
				}

				IAC_u32ISCDuty = u16ISCMin <= IAC_u32ISCDuty ? IAC_u32ISCDuty : u16ISCMin;
				IAC_u32ISCDuty = u16ISCMax >= IAC_u32ISCDuty ? IAC_u32ISCDuty : u16ISCMax;
			}

			/* Weight FF and FB components */
			s32Temp = (IAC_u32ISCDuty - 2048) * USERCAL_stRAMCAL.u16ISCCLWeight;
			s32Temp += (IAC_u16OpenLoopPos << 4) * (0x100 - USERCAL_stRAMCAL.u16ISCCLWeight);
			s32Temp /= 0x100;

			s32Temp = 4096 > s32Temp ? s32Temp : 4095;
			s32Temp = 0 > s32Temp ? 0 : s32Temp;
			IAC_u8SlaveTarget = 0x10 + (s32Temp / 0x100);
#endif //BUILD_BSP_IAC_STEPPER

			if ((IAC_u16ISCTargetRamp + 250) > (uint16)CAM_u32RPMRaw)
			{
				u16TargetRamp = 0 < u16TargetRamp ? u16TargetRamp - 1 : 0;
			}
		}
		else
		{
			IAC_s32ISCESTTrim[0] = 0;
			IAC_s32ISCESTTrim[1] = 0;

			/* Open loop duty */
			IAC_u32ISCDuty = IAC_u16OpenLoopPos << 4;
			IAC_u8SlaveTarget = 0x10 + (IAC_u32ISCDuty / 0x100);
			u16TargetRamp = USERCAL_stRAMCAL.u16ISCEntryRamp;
		}

		IAC_boRun = FALSE;
	}

#ifdef BUILD_BSP_IAC_STEPPER
	if (TRUE == USERCAL_stRAMCAL.u8StepperIACEnable)
	{
		if (IAC_enResetHome == IAC_enStepperState)
		{
			if (0 == IAC_u32StepCounterLimit)
			{
				/* If we have just started the stepper reset sequence */
				IAC_u32StepCounter = 0;
				IAC_u32TargetStepNormalised = 0;
				IAC_u32TargetStep = 0;
				IAC_u32StepCounterLimit = 4 * USERCAL_stRAMCAL.u32UserStepperHomeSteps;
				IAC_u32StepsLearnedMin = ~0;
				u32StepsLearnedCount = 0;
				IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperCloseTable[0];
				IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperCloseTable[1];
				IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperCloseTable[2];
				IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperCloseTable[3];
			}
			else if (IAC_u32StepCounterLimit == IAC_u32StepCounter)
			{
				/* If the stepper reset sequence is complete */
				IAC_u32StepCounter = 0;


				if (0 <= CTS_tTempCFiltered)
				{
					u32Temp = 100000 > CTS_tTempCFiltered ? CTS_tTempCFiltered : 100000;
				}
				else
				{
					u32Temp = 0;
				}

				IAC_u32TargetStepNormalised = IAC_u32GetOpenStepsTempNormalised(u32Temp);
				IAC_u32TargetStep = (USERCAL_stRAMCAL.u16IACStepsOpenCold * IAC_u32TargetStepNormalised) / 256;

				IAC_u32FeedbackStep = 0;
				IAC_u32StepCounterLimit = 4 * IAC_u32TargetStep;
				IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperOpenTable[0];
				IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperOpenTable[1];
				IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperOpenTable[2];
				IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperOpenTable[3];
				IAC_enStepperState = IAC_enNormal;
			}
		}
		else if (IAC_enNormal == IAC_enStepperState)
		{
			if (IAC_u32StepCounterLimit == IAC_u32StepCounter)
			{
				if (0 <= CTS_tTempCFiltered)
				{
					u32TempClipped = 100000 > CTS_tTempCFiltered ? CTS_tTempCFiltered : 100000;
				}
				else
				{
					u32TempClipped = 0;
				} 

				u32OpenLoopTargetStepNormalised = IAC_u32GetOpenStepsTempNormalised(u32TempClipped);

				if (IAC_enOpenLoop == IAC_enControlState)
				{
					IAC_u32TargetStepNormalised = u32OpenLoopTargetStepNormalised;
					u32StepsLearnedCount = 0;
				}
				else
				{
					/* Closed loop */
					s32Temp = (sint32)IAC_u16ISCTarget - (sint32)CAM_u32RPMRaw;
					s32Temp = ABS(s32Temp);
			
					if (60 < s32Temp)
					{
						u32ISCCount++;
						u32StepsLearnedCount = 0;
						
						if (IAC_u16ISCTarget > CAM_u32RPMRaw)
						{
							if (0 == (u32ISCCount % 4))
							{
								IAC_u32TargetStepNormalised = u32OpenLoopTargetStepNormalised > IAC_u32TargetStepNormalised ?
								IAC_u32TargetStepNormalised + 1 : IAC_u32TargetStepNormalised;
							}
						}
						else
						{		
							if (USERCAL_stRAMCAL.u16IACStepsOpenHot > ((IAC_u32TargetStepNormalised * USERCAL_stRAMCAL.u16IACStepsOpenHot) / 256))
							{
								if (250 < s32Temp)
								{
									if (false == IAC_boUnderStepMins())
									{
										if (0 == (u32ISCCount % 8))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
									else
									{
										/* Proceed very slowly to learn when under min learn */
										if (0 == (u32ISCCount % 16))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
								}
								else
								{
									if (false == IAC_boUnderStepMins())
									{
										if (0 == (u32ISCCount % 16))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
									else
									{
										/* Proceed very slowly to learn when under min learn */
										if (0 == (u32ISCCount % 64))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
								}
							}
							else
							{
								if (0 == (u32ISCCount % 16))
								{
									IAC_u32TargetStep = (0 < IAC_u32TargetStep) ?
									IAC_u32TargetStep - 1 : IAC_u32TargetStep;
								}
							}		
						}
					}
					else
					{
						u32ISCCount = 0;
						u32StepsLearnedCount++;

						if ((IAC_LEARN_COUNTS - 1) == (u32StepsLearnedCount % IAC_LEARN_COUNTS))
						{
							if (~0 == IAC_u32StepsLearnedMin)
							{
								IAC_u32StepsLearnedMin = IAC_u32FeedbackStep;
							}
							else
							{
								if (IAC_u32FeedbackStep < IAC_u32StepsLearnedMin)
								{
									if ((IAC_u32StepsLearnedMin - IAC_u32FeedbackStep) > (USERCAL_stRAMCAL.u16IACStepsOpenHot / 2))
									{
										IAC_u32StepsLearnedMin -= (USERCAL_stRAMCAL.u16IACStepsOpenHot / 2);
									}
									else
									{
										IAC_u32StepsLearnedMin = IAC_u32StepsLearnedMin - 1;
									}
								}
								else if (IAC_u32FeedbackStep > IAC_u32StepsLearnedMin)
								{
									IAC_u32StepsLearnedMin = IAC_u32FeedbackStep;
								}
							}
						}
					}
				}
		
				IAC_u32TargetStep = IAC_u32TargetStepNormalised * USERCAL_stRAMCAL.u16IACStepsOpenCold / 0x100;
				s32Temp = (sint32)IAC_u32TargetStep - (sint32)IAC_u32FeedbackStep;
		
				if (1 < (ABS(s32Temp)))
				{
					if (IAC_u32TargetStep < IAC_u32FeedbackStep)
					{
						IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperCloseTable[0];
						IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperCloseTable[1];
						IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperCloseTable[2];
						IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperCloseTable[3];
						IAC_u32StepCounterLimit = 4;
						IAC_u32StepCounter	= 0;
					}
					else
					{
						IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperOpenTable[0];
						IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperOpenTable[1];
						IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperOpenTable[2];
						IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperOpenTable[3];
						IAC_u32StepCounterLimit = 4;
						IAC_u32StepCounter	= 0;
					}
				}
			}
		}

		if (IAC_u32StepCounterLimit > IAC_u32StepCounter)
		{
			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[0])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[0];
				enTriState = (0 == (0x01 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
	
			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[1])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[1];
				enTriState = (0 == (0x02 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
	
			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[2])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[2];
				enTriState = (0 == (0x04 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}

			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[3])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[3];
				enTriState = (0 == (0x08 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;	
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}

	
			IAC_u32StepCounter++;
	
			if (0 == (0x03 & IAC_u32StepCounter))
			{
				if (IAC_u32FeedbackStep > IAC_u32TargetStep)
				{
					IAC_u32FeedbackStep--;
				}
				else if (IAC_u32FeedbackStep < IAC_u32TargetStep)
				{
					IAC_u32FeedbackStep++;
				}
			}
		}
	}
#endif //BUILD_BSP_IAC_STEPPER


	if (400 > CAM_u32RPMFiltered)
	{
		u16AfterStartTargetRamp = USERCAL_stRAMCAL.u16ISCPostStartRamp;
	}
	else
	{
		if (0 == (u32ISCCount % 8))
		{
			if (0 < u16AfterStartTargetRamp)
			{
				u16AfterStartTargetRamp--;
			}
		}
	}


	u32ISCCount++;
}

#ifdef BUILD_BSP_IAC_STEPPER
static uint32 IAC_u32GetOpenStepsTempNormalised(uint32 u32Temp)
{
	static volatile uint32 u32OpenStepsNormalised;
	static volatile uint32 u32ColdNormalisedSteps = 255;
	static volatile uint32 u32HotNormalisedSteps;

	//u32HotNormalisedSteps = ((uint32)USERCAL_stRAMCAL.u16IACStepsOpenCold * 0x100ul) / (uint32)USERCAL_stRAMCAL.u16IACStepsOpenHot;
	u32HotNormalisedSteps = 50;
	u32OpenStepsNormalised = (((100000ul - u32Temp) * u32ColdNormalisedSteps) + (u32Temp * u32HotNormalisedSteps)) / 100000ul;

	return u32OpenStepsNormalised;
}
#endif //BUILD_BSP_IAC_STEPPER

#ifdef BUILD_BSP_IAC_STEPPER
static bool IAC_boUnderStepMins(void)
{
	bool boUnderMins = IAC_u32StepsLearnedMin >= IAC_u32TargetStep;

	return boUnderMins;
}
#endif //BUILD_BSP_IAC_STEPPER

void IAC_vTerminate(puint32 const pu32Arg)
{

}


void IAC_vCallBack(puint32 const pu32Arg)
{
	IAC_boRun = TRUE;
}

#endif //BUILD_USER
