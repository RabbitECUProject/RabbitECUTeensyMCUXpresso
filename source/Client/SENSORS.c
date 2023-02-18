/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Sensors                                                */
/* DESCRIPTION:        This code manages the sensors AD conversions           */
/*                                                                            */
/*                                                                            */
/* FILE NAME:          SENSORS.c                                              */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _SENSORS_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "SENSORS.h"
#include "USERMATH.h"
#include "IAC.h"
#include "FME.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 SENSORS_u32RunCounter;
uint8 SENSORS_au8CANBufferMap[] = SENSORS_nCANBufferMap;
SENSORS_tenADConfig SENSORS_aenADConfig[SENSORS_enADCount];
uint16 SENSORS_au16ADSensorValue[SENSORS_enADCount];
uint16 SENSORS_au16ADSensorFiltered[SENSORS_enADCount];
SPREADAPI_ttSpreadIDX SENSORS_tSpreadPedalTransferIDX;
TABLEAPI_ttTableIDX SENSORS_tTablePedalTransferIDX;
uint32 SENSORS_au32HertzSamples[SENSORS_nHertzSamplesMax];
uint32 SENSORS_au32HertzSamplesSafe[SENSORS_nHertzSamplesMax];
bool SENSORS_boHertzCalcPending;


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void SENSORS_vCEMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);
static void SENSORS_vGetCANSensorData(void);
static void SENSORS_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);
static void SENSORS_vAddConfiguredAD(SENSORS_tenADConfig enADConfig);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void SENSORS_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;	
	TEPMAPI_tstTimedUserEvent stTimedEvent;
	SENSORS_u32RunCounter = 0;
	uint32 u32IDX;
	bool boOK;


#ifdef BUILD_CTS_PULLUP_SENSE
	enEHIOResource = CTS_nSwitchPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
#endif


	/* Request and initialise required Kernel managed spread for pedal transfer */
	SENSORS_tSpreadPedalTransferIDX = SETUP_tSetupSpread((void*)&SENSORS_u32PPSMVoltsRamp, (void*)&USERCAL_stRAMCAL.aUserPedalTransferSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for coolant sensor */
	SENSORS_tTablePedalTransferIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPedalTransferTable, (void*)&SENSORS_u16TPSSafeMax, TYPE_enUInt16, 17, SENSORS_tSpreadPedalTransferIDX, NULL);

	
	/* Request and initialise FTM for CRANK_nInput */
	enEHIOResource = EH_VIO_TPM0;
	enEHIOType = IOAPI_enTEPM;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* ONLY CONFIGURE THE TC3 MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TPM0;
		stTEPMResourceCB.enPreScalar = SENSORS_nFastFTMDivisor;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;
					
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);
	}	
		
	/* Request and initialise CRANK_nInput */
	enEHIOResource = CRANK_nInput;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
	/* Initialise the TEPM channel CRANK_nInput */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		if (0 == USERCAL_stRAMCAL.u8UserPrimaryEdgeSetup)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapFalling;
		}
		else if (1 == USERCAL_stRAMCAL.u8UserPrimaryEdgeSetup)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapRising;
		}
		else
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapAny;
		}

		stTEPMChannelCB.boInterruptEnable = TRUE;
		stTEPMChannelCB.enLinkedResource = VR_nPhaseTelltalePin;
		stTEPMChannelCB.boRecursive = TRUE;
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}	

	stTimedEvent.enMethod = TEPMAPI_enLinkPrimaryProgram;
	stTimedEvent.pfEventCB = &SENSORS_vCEMCallBack;
	
	USER_vSVC(SYSAPI_enConfigureUserTEPMInput, (void*)&enEHIOResource, 
		(void*)&stTimedEvent, (void*)NULL);	


#if defined(BUILD_BSP_AFM_FREQ) || defined(BUILD_BSP_SINGLE_CAM_INPUT)
	/* Request and initialise AFM_FREQ_nInput */

#if defined(BUILD_BSP_AFM_FREQ)
	enEHIOResource = AFM_FREQ_nInput;
#endif

#if defined(BUILD_BSP_SINGLE_CAM_INPUT)
	enEHIOResource = CAM_nInput;
#endif

	enEHIOType = IOAPI_enCaptureCompare;
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* Initialise the TEPM channel AFM_FREQ_nInput (PF-DI) or CAM_nInput (TEENSY_ADAPT)*/
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		if (0 == USERCAL_stRAMCAL.u8UserSecondaryEdgeSetup)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapFalling;
		}
		else if (1 == USERCAL_stRAMCAL.u8UserSecondaryEdgeSetup)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapRising;
		}
		else
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapAny;
		}

		stTEPMChannelCB.boInterruptEnable = TRUE;
		stTEPMChannelCB.boRecursive = FALSE;
		
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}

#if defined(BUILD_SPARKDOG_TEENSY_ADAPT) || defined(BUILD_SPARKDOG_MKS20)
	stTimedEvent.enMethod = TEPMAPI_enLinkVVT1Input;
#else
	stTimedEvent.enMethod = TEPMAPI_enLinkFreqInput;
#endif

	stTimedEvent.pfEventCB = &SENSORS_vCEMCallBack;

	USER_vSVC(SYSAPI_enConfigureUserTEPMInput, (void*)&enEHIOResource,
	(void*)&stTimedEvent, (void*)NULL);
#endif //BUILD_BSP_AFM_FREQ

	/* Enable the Hall-Effect sensor type */
	enEHIOResource = VRA_nPullupEnablePin;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRB_nPullupEnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRA_nVREnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRB_nVREnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nHystLowPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VVTAB_nPullupEnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VVTCD_nPullupEnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nPhaseTelltalePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nHystHighPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	SETUP_vSetDigitalIOHigh(enEHIOResource);

	if (TRUE == USERCAL_stRAMCAL.u8UserPrimaryVREnable)
	{
		SETUP_vSetDigitalIOHigh(VRA_nVREnablePin);
		SETUP_vSetDigitalIOLow(VRA_nPullupEnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOLow(VRA_nVREnablePin);
		SETUP_vSetDigitalIOHigh(VRA_nPullupEnablePin);
	}

	if (TRUE == USERCAL_stRAMCAL.u8UserSecondaryVREnable)
	{
		SETUP_vSetDigitalIOLow(VRB_nPullupEnablePin);
		SETUP_vSetDigitalIOHigh(VRB_nVREnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOHigh(VRB_nPullupEnablePin);
		SETUP_vSetDigitalIOLow(VRB_nVREnablePin);
	}

	if (TRUE == USERCAL_stRAMCAL.u8VCTABPullupEnable)
	{
		SETUP_vSetDigitalIOHigh(VVTAB_nPullupEnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOLow(VVTAB_nPullupEnablePin);
	}

	if (TRUE == USERCAL_stRAMCAL.u8VCTCDPullupEnable)
	{
		SETUP_vSetDigitalIOHigh(VVTCD_nPullupEnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOLow(VVTCD_nPullupEnablePin);
	}

	SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
	SETUP_vSetDigitalIOLow(VR_nHystLowPin);


	SENSORS_u8CANCTS = 0xff;
	SENSORS_u8CANATS = 0xff;
	SENSORS_u8CANSTT[0] = 0xff;
	SENSORS_u8CANSTT[1] = 0xff;
	SENSORS_u8CANLTT[0] = 0xff;
	SENSORS_u8CANLTT[1] = 0xff;
	DIAG_u8PowerModeActive = 0;

	for (u32IDX = 0; u32IDX < SENSORS_enADCount; u32IDX++)
	{
		SENSORS_aenADConfig[u32IDX] = SENSORS_enADInvalid;
	}

	/* Start CAN right away */
	DIAG_u16PowerModeActiveCount = 1000;

	if (EH_IO_Invalid != USERCAL_stRAMCAL.u16AFRADResource)
	{
		/* Request and initialise the AFR ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.u16AFRADResource, IOAPI_enGPSE, ADCAPI_en1Sample, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enADAFR);
		}
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.u16PPSMADResource)
	{
		/* Request and initialise the PPSM ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.u16PPSMADResource, IOAPI_enGPSE, ADCAPI_en1Sample, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enADPPSM);
		}
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.u16PPSSADResource)
	{
		/* Request and initialise the PPSS ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.u16PPSSADResource, IOAPI_enGPSE, ADCAPI_en1Sample, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enADPPSS);
		}
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.au16AuxInputResource[0])
	{
		/* Request and initialise the PPSS ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.au16AuxInputResource[0], IOAPI_enGPSE, ADCAPI_en1Sample, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enAUX1);
		}
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.au16AuxInputResource[1])
	{
		/* Request and initialise the PPSS ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.au16AuxInputResource[1], IOAPI_enGPSE, ADCAPI_en1Sample, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enAUX2);
		}
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.au16AuxInputResource[2])
	{
		/* Request and initialise the PPSS ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.au16AuxInputResource[2], IOAPI_enGPSE, ADCAPI_en1Sample, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enAUX3);
		}
	}

	if (EH_IO_Invalid != USERCAL_stRAMCAL.au16AuxInputResource[3])
	{
		/* Request and initialise the PPSS ADC input channel */
		boOK = SETUP_boSetupADSE(USERCAL_stRAMCAL.au16AuxInputResource[3], IOAPI_enGPSE, ADCAPI_en4Samples, &SENSORS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);

		if (TRUE == boOK)
		{
			SENSORS_vAddConfiguredAD(SENSORS_enAUX4);
		}
	}

	for (u32IDX = 0; u32IDX < SENSORS_enADCount; u32IDX++)
	{
		SENSORS_au16ADSensorValue[u32IDX] = 0xffff;
	}
}

static void SENSORS_vAddConfiguredAD(SENSORS_tenADConfig enADConfig)
{
	uint32 u32IDX;

	for (u32IDX = 0; u32IDX < SENSORS_enADCount; u32IDX++)
	{
		if (SENSORS_enADInvalid == SENSORS_aenADConfig[u32IDX])
		{
			SENSORS_aenADConfig[u32IDX] = enADConfig;
			break;
		}
	}
}

void SENSORS_vRun(puint32 const pu32Arg)
{
	ADCAPI_tenTrigger enTrigger;
	uint8 u8TriggerHystLow = 2 - USERCAL_stRAMCAL.u8TriggerPullStrength;
	uint8 u8TriggerHystHigh = 4 + USERCAL_stRAMCAL.u8TriggerPullStrength;
	uint32 u32AUXIDX = SENSORS_u32RunCounter & 0x7;
	uint32 u32Temp;
	static bool boSensorRPMHystLimit;
	uint32 u32DeltaMax;

	boSensorRPMHystLimit = USERCAL_stRAMCAL.u16SensorHystLimitRPM > CAM_u32RPMFiltered ? FALSE : boSensorRPMHystLimit;
	boSensorRPMHystLimit = ((9 * USERCAL_stRAMCAL.u16SensorHystLimitRPM) / 8) < CAM_u32RPMFiltered ?
			TRUE : boSensorRPMHystLimit;
		
	if (0 == (SENSORS_u32RunCounter % SENSORS_nFastPeriod))
	{
		enTrigger = ADCAPI_enTrigger4;	
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, (void*)NULL, (void*)NULL);/*CR1_12*/
	}	

#ifdef BUILD_CTS_PULLUP_SENSE
	static uint32 u32Count;

	if (0 == (SENSORS_u32RunCounter % SENSORS_nSlowPeriod))
	{
		if (0 == (u32Count++ % 2))
		{
			SENSORS_boCTSACBiasHigh = TRUE;
			SETUP_vSetDigitalIOHigh(CTS_nSwitchPin);
		}
		else
		{
			SENSORS_boCTSACBiasHigh = FALSE;
			SETUP_vSetDigitalIOLow(CTS_nSwitchPin);
		}
	}	
#endif

	if (1 == (SENSORS_u32RunCounter % SENSORS_nSlowPeriod))
	{
		enTrigger = ADCAPI_enTrigger2;	
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, (void*)NULL, (void*)NULL);/*CR1_12*/
	}
	
	if (FALSE == boSensorRPMHystLimit)
	{
		if (0 == SENSORS_u32RunCounter % 0x05)
		{
			if (0 < u8TriggerHystLow)
			{
				SETUP_vSetDigitalIOHigh(VR_nHystLowPin);
			}
			else
			{
				SETUP_vSetDigitalIOLow(VR_nHystLowPin);
			}

			if (0 < u8TriggerHystHigh)
			{
				SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
			}
			else
			{
				SETUP_vSetDigitalIOLow(VR_nHystHighPin);
			}
		}
		else if (u8TriggerHystLow == SENSORS_u32RunCounter % 0x05)
		{
			SETUP_vSetDigitalIOLow(VR_nHystLowPin);
		}
		if (u8TriggerHystHigh == SENSORS_u32RunCounter % 0x05)
		{
			SETUP_vSetDigitalIOLow(VR_nHystHighPin);
		}
	}
	else
	{
		SETUP_vSetDigitalIOLow(VR_nHystLowPin);
		SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
	}


	if (0 == SENSORS_u32RunCounter % 0x08)
	{
		SENSORS_vGetCANSensorData();
	}

	SENSORS_au16ADSensorFiltered[u32AUXIDX] =
		(SENSORS_au16ADSensorValue[u32AUXIDX] +
		3 * SENSORS_au16ADSensorFiltered[u32AUXIDX]) / 4;


	SENSORS_aboAUXActive[u32AUXIDX] =
			SENSORS_au16ADSensorValueFiltered[u32AUXIDX] <
			USERCAL_stRAMCAL.au16AuxInputLowThres[u32AUXIDX - SENSORS_enAUX1] ? FALSE : SENSORS_aboAUXActive[u32AUXIDX];

	SENSORS_aboAUXActive[u32AUXIDX] =
			SENSORS_au16ADSensorValueFiltered[u32AUXIDX] >
			USERCAL_stRAMCAL.au16AuxInputHighThres[u32AUXIDX - SENSORS_enAUX1] ? TRUE : SENSORS_aboAUXActive[u32AUXIDX];
	
	if (SENSORS_enADAFR == u32AUXIDX)
	{
		SENSORS_au16ADSensorValueFiltered[u32AUXIDX] =
			CONV_tADCToVolts(USERCAL_stRAMCAL.u16AFRADResource, SENSORS_au16ADSensorFiltered[u32AUXIDX]);
	}

	if (SENSORS_enADPPSM == u32AUXIDX)
	{
		SENSORS_au16ADSensorValueFiltered[u32AUXIDX] =
			CONV_tADCToVolts(USERCAL_stRAMCAL.u16PPSMADResource, SENSORS_au16ADSensorFiltered[u32AUXIDX]);

		SENSORS_u32PPSMVolts = SENSORS_au16ADSensorValueFiltered[u32AUXIDX];

		u32Temp = CONV_tADCToVolts(USERCAL_stRAMCAL.u16PPSMADResource, SENSORS_au16ADSensorValue[u32AUXIDX]);

		if (u32Temp < SENSORS_u32PPSMVoltsRamp)
		{
			if ((SENSORS_u32PPSMVoltsRamp - u32Temp) > PPSM_RAMP_MAX_NEG)
			{
				if (SENSORS_u32PPSMVoltsRamp > PPSM_RAMP_MAX_NEG)
				{
					SENSORS_u32PPSMVoltsRamp -= PPSM_RAMP_MAX_NEG;
				}
			}
			else
			{
				SENSORS_u32PPSMVoltsRamp = u32Temp;
			}
		}
		else
		{
			if ((u32Temp - SENSORS_u32PPSMVoltsRamp) > PPSM_RAMP_MAX_POS)
			{
				SENSORS_u32PPSMVoltsRamp += PPSM_RAMP_MAX_POS;
			}
			else
			{
				SENSORS_u32PPSMVoltsRamp = u32Temp;
			}
		}


		/* Calculate the current spread for pedal transfer */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&SENSORS_tSpreadPedalTransferIDX,
				NULL, NULL);

		/* Lookup the current spread for pedal transfer */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&SENSORS_tTablePedalTransferIDX,
			NULL, NULL);

		if (3000 > SENSORS_u32PPSMVoltsRamp)
		{
			if ((0 != TORQUE_u16GearShiftCount) && (TRUE == TORQUE_boDownShift))
			{
				SENSORS_u16TPSSafeMaxModified = SENSORS_u16TPSSafeMax + FME_nBLIP_THROTTLE_DELTA_MAX;
			}
			else if (0x1234 != USERCAL_stRAMCAL.u16ETCOverrideKeys)
			{
				/* No higher than pedal authority */
				SENSORS_u16TPSSafeMaxModified = (SENSORS_u32PPSMVoltsRamp + 150) > (uint32)SENSORS_u16TPSSafeMax ?
					SENSORS_u16TPSSafeMax : (uint16)(SENSORS_u32PPSMVoltsRamp + 150);
			}
			else
			{
				/* Else in short overrides by diag */
				SENSORS_u16TPSSafeMaxModified = SENSORS_u16TPSSafeMax + FME_nOVERRIDE_DELTA_MAX;
			}
		}
		else
		{
			SENSORS_u16TPSSafeMaxModified = (5 * SENSORS_u32PPSMVoltsRamp) / 4;
		}
	}

	if (SENSORS_enADPPSS == u32AUXIDX)
	{
		SENSORS_au16ADSensorValueFiltered[u32AUXIDX] =
			CONV_tADCToVolts(USERCAL_stRAMCAL.u16PPSSADResource, SENSORS_au16ADSensorFiltered[u32AUXIDX]);
	}

	if ((SENSORS_enAUX1 <= u32AUXIDX) && (SENSORS_enAUX4 <= u32AUXIDX))
	{
		SENSORS_au16ADSensorValueFiltered[u32AUXIDX] =
			CONV_tADCToVolts(USERCAL_stRAMCAL.au16AuxInputResource[u32AUXIDX - SENSORS_enAUX1], SENSORS_au16ADSensorFiltered[u32AUXIDX]);
	}

#ifdef BUILD_FME
	/* FME calls */
	if ((EH_IO_Invalid != USERCAL_stRAMCAL.u16PPSMADResource) &&
		(EH_IO_Invalid != USERCAL_stRAMCAL.u16PPSSADResource))
	{
		FME_enUpdateDiagState(FME_enPPSPair, SENSORS_au16ADSensorFiltered[SENSORS_enADPPSM],
				SENSORS_au16ADSensorFiltered[SENSORS_enADPPSS]);
		FME_enUpdateDiagState(FME_enPPSMSingle, SENSORS_au16ADSensorFiltered[SENSORS_enADPPSM], 0);
		FME_enUpdateDiagState(FME_enPPSSSingle, SENSORS_au16ADSensorFiltered[SENSORS_enADPPSS], 0);
	}

	/* FME calls */
	if ((EH_IO_Invalid != USERCAL_stRAMCAL.u16PPSMADResource) &&
		(EH_IO_Invalid != USERCAL_stRAMCAL.u16TPSADResource))
	{
		FME_enUpdateDiagState(FME_enPedalTransfer, SENSORS_u16TPSSafeMaxModified,
				TPS_tSensorVolts);
	}
#endif //BUILD_FME

	if ((TRUE == SENSORS_boHertzCalcPending) || (400 > CAM_u32RPMRaw))
	{
		/* discard and average from last 5 */
		u32Temp = USERMATH_u32DiscardAndAverage32(&SENSORS_au32HertzSamplesSafe[1], 7, 2);

		if (0 == USERCAL_stRAMCAL.u16DiagType)
		{
			u32Temp = SENSORS_nSlowFTMFreq / u32Temp;

			if (u32Temp > AFM_tSensorHertz)
			{
				/* Increase in frequency */
				u32DeltaMax = 200 + (CAM_u32RPMRaw / 5);

				if ((u32Temp - AFM_tSensorHertz) > u32DeltaMax)
				{
					AFM_tSensorHertz += u32DeltaMax;
				}
				else
				{
					AFM_tSensorHertz = u32Temp;
				}
			}
			else if (u32Temp < AFM_tSensorHertz)
			{
			    /* Decrease in frequency */
				u32DeltaMax = 200 + (CAM_u32RPMRaw / 5);

				if ((AFM_tSensorHertz - u32Temp) > u32DeltaMax)
				{
					AFM_tSensorHertz -= u32DeltaMax;
				}
				else
				{
					AFM_tSensorHertz = u32Temp;
				}
			}
		}
		else
		{
			AFM_tSensorHertz = SENSORS_nSlowFTMFreq / u32Temp;
		}

		SENSORS_boHertzCalcPending = FALSE;
	}

	SENSORS_u32RunCounter++;
}

void SENSORS_vInvalidateCAN16Data(uint32 u32MSGIDX, uint32 u32DataOffset)
{
	IOAPI_tenEHIOResource enEHIOResource;
	puint8 pu8CANDataBuffer;
	enEHIOResource = EH_VIO_CAN1;

	USER_vSVC(SYSAPI_enGetRawCommsBuffer, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		pu8CANDataBuffer = (puint8)pstSVCDataStruct->pvArg1;
		pu8CANDataBuffer += (8 * u32MSGIDX);
		pu8CANDataBuffer += u32DataOffset;
		memset(pu8CANDataBuffer, 0xff, 2);
	}
}

static void SENSORS_vGetCANSensorData()
{
	IOAPI_tenEHIOResource enEHIOResource;
	uint8* pu8CANDataBuffer = {0};
	enEHIOResource = EH_VIO_CAN1;
	uint16 u16Temp;
	sint16 s16Temp;
	sint32 s32Temp;
	static uint8 u8OldATXStat;
	static uint8 u8OldGear;
	static uint8 u8OldDistCount;
	static sint16 s16OldDistance;
	static uint32 u32VSSTimeout = 0;

	USER_vSVC(SYSAPI_enGetRawCommsBuffer, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		pu8CANDataBuffer = (puint8)pstSVCDataStruct->pvArg1;
	}

	if (NULL != pu8CANDataBuffer)
	{
		SENSORS_boCANNewGearPositionSample = pu8CANDataBuffer[11] != 0xff;

		if (TRUE == SENSORS_boCANNewGearPositionSample)
		{
			switch ((pu8CANDataBuffer[11] & 0xf0) >> 4)
			{
				case 8: DIAG_u8GearSelectorReport = 0; break;
				case 7: DIAG_u8GearSelectorReport = 1; break;
				case 6: DIAG_u8GearSelectorReport = 2; break;
				case 5: DIAG_u8GearSelectorReport = 3; break;
				case 12: DIAG_u8GearSelectorReport = 4; break;
				case 10: DIAG_u8GearSelectorReport = 5; break;
				case 11: DIAG_u8GearSelectorReport = 6; break;
				default: DIAG_u8GearSelectorReport = 15; break;
			}

			SENSORS_boCANNewGearPositionSample = FALSE;
		}


		SENSORS_boCANNewBrakePressedSample = pu8CANDataBuffer[30] != 0xff;

		if (TRUE == SENSORS_boCANNewBrakePressedSample)
		{
			SENSORS_boBrakePedalPressed = 0 != (pu8CANDataBuffer[30] & 0x40);
			SENSORS_boCANNewBrakePressedSample = FALSE;
		}

		if (USERCAL_stRAMCAL.u16ETCOverrideKeys == 0xffff)
		{
			pu8CANDataBuffer[18] = USERCAL_stRAMCAL.u16ETCOverride & 0x00ff;
			pu8CANDataBuffer[19] = USERCAL_stRAMCAL.u16ETCOverride >> 8;
			TORQUE_boVehicleMovingUS = TRUE;
			TORQUE_boManualShiftMode = TRUE;
		}

		/* Check CAN torque reduction requests */
		SENSORS_boCANNewTorqueRequestSample = pu8CANDataBuffer[19] != 0xff;

		if (TRUE == SENSORS_boCANNewTorqueRequestSample)
		{
			TORQUE_boManualShiftMode = 0 != (pu8CANDataBuffer[18] & 0x80);
			TORQUE_u8ATXSelectedGear = 0xf & pu8CANDataBuffer[18];
			u8OldGear = 0xf & pu8CANDataBuffer[18];

			/* Look for status change */
			if (pu8CANDataBuffer[19] != u8OldATXStat)
			{
				u8OldATXStat = pu8CANDataBuffer[19];

				if (0x04 == (u8OldATXStat & 0x04))
				{
					/* Up-shifting pre or post */
					TORQUE_boDownShift = FALSE;

					if ((0x01 == (u8OldATXStat & 0x01)) && (TRUE == TORQUE_boVehicleMovingUS))
					{
						if (FALSE == TORQUE_boPostShift)
						{
							TORQUE_u16GearShiftCount = USERCAL_stRAMCAL.u16ShiftUpCountLimit;
							TORQUE_boPostShift = TRUE;

							if (TRUE == TORQUE_boManualShiftMode)
							{
								FUEL_vQuickCut(TORQUE_u32QuickCutPercent, TORQUE_u32QuickCutDuration);
							}
						}
					}

					if ((TRUE == TORQUE_boPostShift) && (FALSE == TORQUE_boESTTorqueModify) && (0x80 == (u8OldATXStat & 0x80)))
					{
						/* Shifting up, DSG torque reduction request */
						EST_enIgnitionTimingRequest = EST_nIgnitionReqDSGStage1;
						TORQUE_boESTTorqueModify = TRUE;

						/* Try finishing fuel cuts */
						FUEL_vQuickCut(0,0);
					}
				}
				else if (0x01 == (u8OldATXStat & 0x05))
				{
					/* Shifting down, DSG torque reduction request */
					EST_enIgnitionTimingRequest = EST_nIgnitionReqDSGStage1;
					TORQUE_boDownShift = TRUE;
					TORQUE_boPostShift = TRUE;
					TORQUE_boESTTorqueModify = TRUE;
					TORQUE_u16GearShiftCount = USERCAL_stRAMCAL.u16ShiftDownCountLimit;
				}
				else if (0x10 == (u8OldATXStat & 0x10))
				{
					/* DSG normal */
					EST_enIgnitionTimingRequest = EST_nIgnitionReqPrimary;
					TORQUE_u16GearShiftCount = 0;
					TORQUE_u16GearShiftPressureControlCount = 0;
					TORQUE_boDownShift = FALSE;
					TORQUE_boPostShift = FALSE;
					TORQUE_boESTTorqueModify = FALSE;
					FUEL_vQuickCut(0, 0);
				}
			}
			else
			{
				if ((TRUE == TORQUE_boPostShift) && (TRUE == TORQUE_boESTTorqueModify))
				{
					TORQUE_u16GearShiftCount = 0 != TORQUE_u16GearShiftCount ? TORQUE_u16GearShiftCount - 1 : 0;
					TORQUE_u16GearShiftPressureControlCount = 0 != TORQUE_u16GearShiftPressureControlCount ?
							TORQUE_u16GearShiftPressureControlCount - 1 : 0;

					EST_enIgnitionTimingRequest = 0 == TORQUE_u16GearShiftCount ? EST_nIgnitionReqPrimary : EST_enIgnitionTimingRequest;
				}
			}

			TORQUE_u32ATXTorqueLimit = pu8CANDataBuffer[16];
		}


		/* Get power mode */
		u16Temp = pu8CANDataBuffer[0] + (pu8CANDataBuffer[1] << 8);

		if (DIAG_u16PowerModeCode != u16Temp)
		{
			DIAG_u8PowerModeActive = TRUE;
			DIAG_u16PowerModeActiveCount = 200;
			DIAG_u16PowerModeCode = u16Temp;
		}
		else
		{
			DIAG_u16PowerModeActiveCount = 0 < DIAG_u16PowerModeActiveCount ?
					DIAG_u16PowerModeActiveCount - 1 : DIAG_u16PowerModeActiveCount;

			DIAG_u8PowerModeActive = 0 < DIAG_u16PowerModeActiveCount;
		}

		if (USERCAL_stRAMCAL.u16ETCOverrideKeys != 0xffff)
		{
			/* Get VSS */
			if (u8OldDistCount != (0x80 & pu8CANDataBuffer[8]))
			{
				u8OldDistCount = 0x80 & pu8CANDataBuffer[8];

				s16Temp = pu8CANDataBuffer[13] * 0x100 + pu8CANDataBuffer[14];

				if (s16Temp >= s16OldDistance)
				{
					s32Temp = (sint32)s16Temp - (sint32)s16OldDistance;
				}
				else if (s16Temp < s16OldDistance)
				{
					s32Temp = (sint32)s16Temp - (sint32)s16OldDistance;
					s32Temp += 2048;
				}

				s32Temp *= USERCAL_stRAMCAL.u16VSSCANCal;
				s32Temp /= 100;

				s32Temp = 0x10000 > s32Temp ? s32Temp : 0xffff;
				SENSORS_u16CANVSS = ((uint16)s32Temp / 2) + (SENSORS_u16CANVSS / 2);

				s16OldDistance = s16Temp;
				u32VSSTimeout = 0;
			}
			else if (u32VSSTimeout > 125)
			{
				SENSORS_u16CANVSS = 0;
			}
			else
			{
				u32VSSTimeout++;
			}
		}


		/* Calculate gear */

		/* Calculate km/h per 1000 rpm */
		s32Temp = SENSORS_u16CANVSS * 1000 / CAM_u32RPMFiltered;

		/* Default to no gear */
		SENSORS_u8VSSCalcGear = 0;
		SENSORS_u16VSSCalcGearRPMSlip = 65535;

		for (u16Temp = 0; u16Temp < 8; u16Temp++)
		{
			s16Temp = (sint16)s32Temp - USERCAL_stRAMCAL.u16VSSPerRPM[u16Temp];

			if ((USERCAL_stRAMCAL.u16VSSPerRPM[0] / 4) > (ABS(s16Temp)))
			{
				/* If small error then this is correct gear */
				SENSORS_u8VSSCalcGear = u16Temp + 1;

				/* Calculate RPM slip */
				s32Temp = (1000 * SENSORS_u16CANVSS) / USERCAL_stRAMCAL.u16VSSPerRPM[u16Temp];
				s32Temp -= (sint32)CAM_u32RPMFiltered;
				s32Temp = ABS(s32Temp);

				SENSORS_u16VSSCalcGearRPMSlip = (uint16)s32Temp;
				break;
			}
		}

		if (USERCAL_stRAMCAL.u16ETCOverrideKeys == 0x5678)
		{
			TORQUE_u8ATXSelectedGear = USERCAL_stRAMCAL.u16ETCOverride & 0x000f;
			SENSORS_u16CANVSS = 10 * (USERCAL_stRAMCAL.u16ETCOverride >> 8);
		}

		/* Calculate rpm slips based on DSG reported engaged gear */
		u16Temp = 6 > TORQUE_u8ATXSelectedGear ? TORQUE_u8ATXSelectedGear : 6;

		/* zero based index */
		if (u16Temp) u16Temp--;

		/* Calculate RPM slip */
		s32Temp = (1000 * SENSORS_u16CANVSS) / USERCAL_stRAMCAL.u16VSSPerRPM[u16Temp];
		s32Temp -= (sint32)CAM_u32RPMFiltered;
		s32Temp = ABS(s32Temp);

		SENSORS_u16VSSDSGGearRPMSlip = (uint16)s32Temp;

		if (5 > u16Temp)
		{
			u16Temp++;

			/* Calculate RPM slip next gear */
			s32Temp = (1000 * SENSORS_u16CANVSS) / USERCAL_stRAMCAL.u16VSSPerRPM[u16Temp];
			s32Temp -= (sint32)CAM_u32RPMFiltered;
			s32Temp = ABS(s32Temp);

			SENSORS_u16VSSDSGGearRPMSlipNext = (uint16)s32Temp;
		}
		else
		{
			SENSORS_u16VSSDSGGearRPMSlipNext = SENSORS_u16VSSDSGGearRPMSlip;
		}
	}
}


void SENSORS_vTerminate(puint32 const pu32Arg)
{

}


void SENSORS_vCallBack(puint32 const pu32Arg)
{

}

static void SENSORS_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	sint32 s32Temp;

	if (USERCAL_stRAMCAL.u16AFRADResource == enEHIOResource)
	{
		s32Temp = USERCAL_stRAMCAL.s32AFRADInputGain * (sint32)u32ADCResult;
		s32Temp /= 1000;
		s32Temp += USERCAL_stRAMCAL.s32AFRADInputOffset;
		CLO2_au16AFRMeasured[0] = (uint32)s32Temp;
		s32Temp *= (sint32)USERCAL_stRAMCAL.u16AFRFuelTypeGain;
		s32Temp /= 1000;

		if (65536 > s32Temp)
		{
			CLO2_au16AFRDisplayMeasured[0] = (uint16)(s32Temp);
		}
		else
		{
			CLO2_au16AFRDisplayMeasured[0] = 0xffff;
		}
	}
	else if (USERCAL_stRAMCAL.u16PPSMADResource == enEHIOResource)
	{
		SENSORS_au16ADSensorValue[SENSORS_enADPPSM] = u32ADCResult;
	}
	else if (USERCAL_stRAMCAL.u16PPSSADResource == enEHIOResource)
	{
		SENSORS_au16ADSensorValue[SENSORS_enADPPSS] = u32ADCResult;
	}
	else if (USERCAL_stRAMCAL.au16AuxInputResource[0] == enEHIOResource)
	{
		SENSORS_au16ADSensorValue[SENSORS_enAUX1] = (uint16)u32ADCResult;
	}
	else if (USERCAL_stRAMCAL.au16AuxInputResource[1] == enEHIOResource)
	{
		SENSORS_au16ADSensorValue[SENSORS_enAUX2] = (uint16)u32ADCResult;
	}
	else if (USERCAL_stRAMCAL.au16AuxInputResource[2] == enEHIOResource)
	{
		SENSORS_au16ADSensorValue[SENSORS_enAUX3] = (uint16)u32ADCResult;
	}
	else if (USERCAL_stRAMCAL.au16AuxInputResource[3] == enEHIOResource)
	{
		SENSORS_au16ADSensorValue[SENSORS_enAUX4] = (uint16)u32ADCResult;
	}
}

static void SENSORS_vCEMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	switch (enEHIOResource)
	{
		case CRANK_nInput:
		{
		    CAM_vEngineSpeedCB(tEventTime);
			break;	
		}
		case CAM_nInput:
		{

		}
#ifdef BUILD_BSP_AFM_FREQ
		case AFM_FREQ_nInput:
		{
			memcpy(SENSORS_au32HertzSamples, (uint32*)tEventTime, SENSORS_nHertzSamplesMax * sizeof(uint32));
			break;
		}
#endif //BUILD_BSP_AFM_FREQ
		default:
		{
			break;
		}
	}	
}

bool SENSORS_boGetAuxActive(SENSORS_tenAUXConfig enAUXConfig)
{
	uint32 u32IDX;
	bool boFound = FALSE;
	bool boActiveLow;
	bool boActive = FALSE;

	for (u32IDX = 0; u32IDX < SENSORS_nAuxChannelCount; u32IDX++)
	{
		if (USERCAL_stRAMCAL.au16AuxInputFunction[u32IDX] == enAUXConfig)
		{
			boFound = TRUE;
			break;
		}
	}

	if (TRUE == boFound)
	{
		boActiveLow = (0 == (USERCAL_stRAMCAL.au16AuxInputFunction[u32IDX] & 1));

		if (TRUE == boActiveLow)
		{
			boActive = !SENSORS_aboAUXActive[u32IDX + SENSORS_enAUX1];
		}
		else
		{
			boActive = SENSORS_aboAUXActive[u32IDX + SENSORS_enAUX1];
		}
	}

	return boActive;
}

void SENSORS_vCycleUpdate(void)
{
	USER_xEnterCritical();
	memcpy(SENSORS_au32HertzSamplesSafe, SENSORS_au32HertzSamples, sizeof(SENSORS_au32HertzSamples));
	USER_xExitCritical();
	SENSORS_boHertzCalcPending = TRUE;
}


#endif //BUILD_USER
