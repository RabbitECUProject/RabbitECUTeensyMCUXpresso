/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Fuelling                                               */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for fuelling calculations      */
/*                     measurement                                            */
/* FILE NAME:          FUEL.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _FUEL_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "FUEL.h"

#ifdef BUILD_FME
#include "FME.h"
#endif

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
TEPMAPI_tstTimedKernelEvent FUEL_astTimedHoldKernelEvents[2];
TEPMAPI_tstTimedKernelEvent FUEL_astTimedFuelPumpEvents[100];
SPREADAPI_ttSpreadIDX FUEL_tSpreadAfmTFIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadTAFRxIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadTAFRyIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadFuelFlowIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadTFuelPressurexIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadTFuelPressureyIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadCrankingAirflowIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadInjResponseIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadInjShortOpeningIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadFuelCutsIDX;
TABLEAPI_ttTableIDX FUEL_tTableCrankingAirflowIDX;
TABLEAPI_ttTableIDX FUEL_tTableAfmTFIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjResponseIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjShortOpeningIDX;
TABLEAPI_ttTableIDX FUEL_tTableFuelFlowIDX;
TABLEAPI_ttTableIDX FUEL_tTableFuelCutsIDX;
MAPSAPI_ttMapIDX FUEL_tMapTAFRIDX;
MAPSAPI_ttMapIDX FUEL_tMapTFuelPressureIDX;
uint16 FUEL_u16InjResponse;
uint16 FUEL_u16TFuelPressure;
uint16 FUEL_u16CrankingAirflow;
uint32 FUEL_u32SensorStateBank2;
bool FUEL_boCalculatePending;
bool FUEL_bo720Injection;
uint32 u32SequenceIDX;
uint32 FUEL_u32ADCRaw;
uint32 FUEL_u32ADCFiltered;
bool FUEL_boNewSample;
uint16 FUEL_u16FuelFlowRate;
uint32 FUEL_u32FuelChannelsMask;
uint32 FUEL_u32FuelChannelsCount;


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void FUEL_vTEPMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);
static void FUEL_vCyclicCalculate(void);
static void FUEL_vADCCallBack(IOAPI_tenEHIOResource, uint32);
static void FUEL_vCalcFuelChannels(void);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void FUEL_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;	
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;	
	IOAPI_tenDriveStrength enDriveStrength;
	uint32 u32EventCount;
	ADCAPI_tstADCCB stADCCB;

	FUEL_u32ADCRaw = 0;
	FUEL_boNewSample = false;
	FUEL_u32FuelChannelsMask = 0;

	if (EH_IO_Invalid != USERCAL_stRAMCAL.u16FRSADResource)
	{
		enEHIOResource = USERCAL_stRAMCAL.u16FRSADResource;
		enEHIOType = IOAPI_enADSE;
		stADCCB.enSamplesAv = ADCAPI_en32Samples;
		stADCCB.pfResultCB = &FUEL_vADCCallBack;
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


	/* Setup fuel pump relay */
	if ((EH_IO_Invalid != USERCAL_stRAMCAL.enFuelPumpRelay) && (EH_VIO_REL1 > USERCAL_stRAMCAL.enFuelPumpRelay))
	{
		enEHIOResource = USERCAL_stRAMCAL.enFuelPumpRelay;
		enEHIOType = IOAPI_enDIOOutput;
		enDriveStrength = IOAPI_enWeak;

		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
		SETUP_vSetDigitalIOHigh(enEHIOResource);
	}

		
	/* Both peak and hold have a switch on and switch off event per cycle */
	TEPMAPI_ttEventCount tEventCount = 2;

	/* Initialise the GDI fuel pump control parameters */
	FUEL_tStartFPSolenoidDelay = 100;
	FUEL_tStartFPSolenoidPeak = 15000;
	FUEL_tStartFPSolenoidDutyHigh = 180;//160;
	FUEL_tStartFPSolenoidDutyLow = 100;//140;
	FUEL_tFPAccumulate = 9000;
	FUEL_u16TFuelPressure = 12000;
		
	/* Set injection time to Xms */
	FUEL_tTimeHoldUs[0] = 1000;
	FUEL_tTimeHoldUs[1] = 1000;
	FUEL_tTimeHoldUs[2] = 1000;
	FUEL_tTimeHoldUs[3] = 1000;
	FUEL_tTimeHoldUs[4] = 1000;
	FUEL_tTimeHoldUs[5] = 1000;
	FUEL_tTimeHoldUs[6] = 1000;
	FUEL_tTimeHoldUs[7] = 1000;
	
	FUEL_tTimeHold[0] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[0]);
	FUEL_tTimeHold[1] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[1]);
	FUEL_tTimeHold[2] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[2]);
	FUEL_tTimeHold[3] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[3]);
	FUEL_tTimeHold[4] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[4]);
	FUEL_tTimeHold[5] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[5]);
	FUEL_tTimeHold[6] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[6]);
	FUEL_tTimeHold[7] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[7]);
	
	/* Set injection phase to 360 degrees */
	FUEL_tStartHoldFraction[0] = (0x10000 * 2) / 720;		
	FUEL_tStartHoldFraction[1] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[2] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[3] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[4] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[5] = (0x10000 * 5) / 720;	
	FUEL_tStartHoldFraction[6] = (0x10000 * 5) / 720;	
	FUEL_tStartHoldFraction[7] = (0x10000 * 5) / 720;									

	/* Request and initialise Fuel Injector group A */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[0]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[0]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[0];     //FUEL_nInj1Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel Fuel Injector group A */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[0];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}			
		
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[0];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[0];	
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);	

		if ((EH_IO_TMR1 <= USERCAL_stRAMCAL.aFuelIOResource[0]) &&
			(EH_IO_TMR16 >= USERCAL_stRAMCAL.aFuelIOResource[0]))
		{
			FUEL_u32FuelChannelsMask |=
					(1 << (USERCAL_stRAMCAL.aFuelIOResource[0] - EH_IO_TMR1));
		}
	}	
	
	/* Request and initialise Fuel Injector group B */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[1]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[1]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[1];     //FUEL_nInj2Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel Fuel Injector group B */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;	
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[1];		
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}	
	
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[1];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[1];
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;	
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;						
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);	

		if ((EH_IO_TMR1 <= USERCAL_stRAMCAL.aFuelIOResource[1]) &&
			(EH_IO_TMR16 >= USERCAL_stRAMCAL.aFuelIOResource[1]))
		{
			FUEL_u32FuelChannelsMask |=
					(1 << (USERCAL_stRAMCAL.aFuelIOResource[1] - EH_IO_TMR1));
		}
	}
		
	/* Request and initialise Fuel Injector group C */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[2]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[2]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[2];     //FUEL_nInj3Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel Fuel Injector group C */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[2];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}		
	
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[2];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[2];
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;	
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;					
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);

		if ((EH_IO_TMR1 <= USERCAL_stRAMCAL.aFuelIOResource[2]) &&
			(EH_IO_TMR16 >= USERCAL_stRAMCAL.aFuelIOResource[2]))
		{
			FUEL_u32FuelChannelsMask |=
					(1 << (USERCAL_stRAMCAL.aFuelIOResource[2] - EH_IO_TMR1));
		}
	}	
	
	/* Initialise the TEPM channel Fuel Injector group D */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[3]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[3]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[3];     //FUEL_nInj4Output;
		enEHIOType = IOAPI_enCaptureCompare;
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

		/* Initialise the TEPM channel Fuel Injector group D */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[3];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}					
	
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[3];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[3];
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;

		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);		

		if ((EH_IO_TMR1 <= USERCAL_stRAMCAL.aFuelIOResource[3]) &&
			(EH_IO_TMR16 >= USERCAL_stRAMCAL.aFuelIOResource[3]))
		{
			FUEL_u32FuelChannelsMask |=
					(1 << (USERCAL_stRAMCAL.aFuelIOResource[3] - EH_IO_TMR1));
		}
	}

	/* Initialise the Fuel Pressure Solenoid free-wheel enable*/
	enEHIOResource = EH_IO_GP5;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);

	/* Initialise the TEPM channel Fuel Pressure Solenoid */
	if (EH_IO_Invalid > USERCAL_stRAMCAL.u16FuelPressureSolenoidResource)
	{
		enEHIOResource = USERCAL_stRAMCAL.u16FuelPressureSolenoidResource;
		enEHIOType = IOAPI_enCaptureCompare;
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

		/* Initialise the TEPM channel Fuel Pressure Solenoid */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = 0x0000080;

			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}

		/* Switch injector on at a fraction of global time */
		FUEL_astTimedFuelPumpEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedFuelPumpEvents[0].enMethod = TEPMAPI_enGlobalLinkedTimeStep;//TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedFuelPumpEvents[0].ptEventTime = &FUEL_tStartFPSolenoidDelay;
		FUEL_astTimedFuelPumpEvents[0].enEHIOBitMirrorResource = EH_IO_GP5;

		FUEL_astTimedFuelPumpEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedFuelPumpEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedFuelPumpEvents[1].ptEventTime = &FUEL_tStartFPSolenoidPeak;
		FUEL_astTimedFuelPumpEvents[1].pfEventCB = FUEL_vTEPMCallBack;
		FUEL_astTimedFuelPumpEvents[1].enEHIOBitMirrorResource = EH_IO_GP5;
		FUEL_astTimedFuelPumpEvents[1].ptAccumulate = &FUEL_tFPAccumulate;

		for (u32EventCount = 1; u32EventCount <= 48; u32EventCount++)
		{
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount].enAction = TEPMAPI_enSetHigh;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount].enMethod = TEPMAPI_enHardLinkedTimeStep;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount].ptEventTime = &FUEL_tStartFPSolenoidDutyLow;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount].pfEventCB = FUEL_vTEPMCallBack;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount].enEHIOBitMirrorResource = EH_IO_GP5;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount].ptAccumulate = &FUEL_tFPAccumulate;

			FUEL_astTimedFuelPumpEvents[2 * u32EventCount + 1].enAction = TEPMAPI_enSetLow;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount + 1].enMethod = TEPMAPI_enHardLinkedTimeStep;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount + 1].ptEventTime = &FUEL_tStartFPSolenoidDutyHigh;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount + 1].pfEventCB = FUEL_vTEPMCallBack;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount + 1].enEHIOBitMirrorResource = EH_IO_GP5;
			FUEL_astTimedFuelPumpEvents[2 * u32EventCount + 1].ptAccumulate = &FUEL_tFPAccumulate;
		}

		tEventCount = 98;

		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource,
			(void*)&FUEL_astTimedFuelPumpEvents[0], (void*)&tEventCount);
	}
					
	/* Request and initialise required Kernel managed spread for AfmTF */
	FUEL_tSpreadAfmTFIDX = SETUP_tSetupSpread((void*)&FUEL_nXAFMAxisRef, (void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for AfmTF */
	FUEL_tTableAfmTFIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFTable, (void*)&AFM_tAirFlowAFMRawUg, TYPE_enUInt32, 17, FUEL_tSpreadAfmTFIDX, NULL);
	
	/* Request and initialise required Kernel managed spread for Fuel Flow */
	FUEL_tSpreadFuelFlowIDX = SETUP_tSetupSpread((void*)&FUEL_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserFuelFlowRateSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Fuel Flow */
	FUEL_tTableFuelFlowIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserFuelFlowRateTable, (void*)&FUEL_u16FuelFlowRate, TYPE_enUInt16, 17, FUEL_tSpreadFuelFlowIDX, NULL);

	/* Request and initialise required Kernel managed spread for TAFRx */
	FUEL_tSpreadTAFRxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserTAFRxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for TAFRy */
	FUEL_tSpreadTAFRyIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserTAFRySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed map for TAFR */
	FUEL_tMapTAFRIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserMAPTAFR, (void*)&FUEL_u16TAFR, TYPE_enUInt16, 17, 17, FUEL_tSpreadTAFRxIDX, FUEL_tSpreadTAFRyIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for injector response */
	FUEL_tSpreadInjResponseIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserInjResponseSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for injector response */
	FUEL_tTableInjResponseIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserInjResponseTable, (void*)&FUEL_u16InjResponse, TYPE_enUInt16, 17, FUEL_tSpreadInjResponseIDX, NULL);			

	/* Request and initialise required Kernel managed spread for cranking airflow */
	FUEL_tSpreadCrankingAirflowIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserCrankingAirflowSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for cranking airflow */
	FUEL_tTableCrankingAirflowIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserCrankingAirflowTable, (void*)&FUEL_u16CrankingAirflow, TYPE_enUInt16, 17, FUEL_tSpreadCrankingAirflowIDX, NULL);
	
	/* Request and initialise required Kernel managed spread for injector short opening */
	FUEL_tSpreadInjShortOpeningIDX = SETUP_tSetupSpread((void*)&FUEL_tTimePredictedUsInput, (void*)&USERCAL_stRAMCAL.aUserInjShortOpeningSpread, TYPE_enUInt32, 11, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for injector short opening */
	FUEL_tTableInjShortOpeningIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserInjShortOpeningTable, (void*)&FUEL_tTimePredictedShortOpeningUs, TYPE_enUInt32, 11, FUEL_tSpreadInjShortOpeningIDX, NULL);

	/* Request and initialise required Kernel managed spread for launch and flat shift fuel cuts */
	FUEL_tSpreadFuelCutsIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserFuelCutsSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for launch and flat shift fuel cuts */
	FUEL_tTableFuelCutsIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserFuelCutsTable, (void*)&FUEL_u16FuelCutsPercent, TYPE_enUInt16, 17, FUEL_tSpreadFuelCutsIDX, NULL);


	FUEL_bo720Injection = FALSE;

	for (u32SequenceIDX = 0; u32SequenceIDX < FUEL_nFuelSequenceCount; u32SequenceIDX++)
	{
		if (((USERCAL_stRAMCAL.au32InjectionSequence[u32SequenceIDX] & 0xff) == 0xff) ||
				((USERCAL_stRAMCAL.au32InjectionSequence[u32SequenceIDX] & 0xff00) == 0xff00))
		{
		    FUEL_bo720Injection = TRUE;
		}
	}

	CLO2_vInit();
	FUEL_vCalcFuelChannels();
}

void FUEL_vRun(puint32 const pu32Arg)
{
	static uint32 u32FuelRunCount = 0;
	static sint32 s32FuelPressureErrorSum;
	static uint32 u32SampleCount = 0;
	uint16 u16WDTVal;
	IOAPI_tenTriState enTriState;
	IOAPI_tenEHIOResource enEHIOResource;
	RELAY_tenBit enBit;
	uint32 u32Temp;
	sint32 s32Temp;
	static bool boFuelCutsActivePrev = FALSE;
	bool boFuelCutsActive;
	static bool boAirflowFMEEnable;
	uint32 u32FuelCutPercent = 40;

	CLO2_vFilterSensors();
	
	if (0 == u32FuelRunCount % FUEL_nInjRespCalcRate)
	{
		/* Calculate the current spread for injector response */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadInjResponseIDX,
				NULL, NULL);		
		
		/* Lookup the current value for injector response */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableInjResponseIDX,
			NULL, NULL);		
			
		/* Calculate the current spread for TAFRx */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadTAFRxIDX,
		NULL, NULL);		
		
		/* Calculate the current spread for TAFRy */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadTAFRyIDX,
		NULL, NULL);			
		
		/* Lookup the current value for TAFR */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&FUEL_tMapTAFRIDX,
		NULL, NULL);			
			
	    /* Reset the WDT */
		USER_vSVC(SYSAPI_enResetWatchdog, &u16WDTVal, NULL, NULL);
	}
	
	if ((TRUE == FUEL_boCalculatePending)
		|| (FALSE == FUEL_boFuelPrimed))

	{
		FUEL_vCyclicCalculate();

		if (EH_IO_Invalid > USERCAL_stRAMCAL.u16FuelPressureSolenoidResource)
		{
			if (USERCAL_stRAMCAL.u32GDIMAPMin >  MAP_tKiloPaFiltered)
			{
				FUEL_u16TFuelPressure = USERCAL_stRAMCAL.u16GDIPressureMin;
			}
			else if (USERCAL_stRAMCAL.u32GDIMAPMax <  MAP_tKiloPaFiltered)
			{
				FUEL_u16TFuelPressure = USERCAL_stRAMCAL.u16GDIPressureMax;
			}
			else
			{
				FUEL_u16TFuelPressure = USERCAL_stRAMCAL.u16GDIPressureMin +
					(((MAP_tKiloPaFiltered - USERCAL_stRAMCAL.u32GDIMAPMin) * (USERCAL_stRAMCAL.u16GDIPressureMax - USERCAL_stRAMCAL.u16GDIPressureMin)) /
					(USERCAL_stRAMCAL.u32GDIMAPMax - USERCAL_stRAMCAL.u32GDIMAPMin));
			}


			s32Temp = FUEL_tKiloPaFiltered - FUEL_u16TFuelPressure;

			if (0 > s32Temp)
			{
				/* accumulate the feedback error */
				s32FuelPressureErrorSum += ((s32Temp * 500) / (sint32)CAM_u32RPMRaw);
				s32FuelPressureErrorSum = 20000 < s32FuelPressureErrorSum ? 20000 : s32FuelPressureErrorSum;
				s32FuelPressureErrorSum = -20000 > s32FuelPressureErrorSum ? -20000 : s32FuelPressureErrorSum;

				/* pressure too low */
				s32Temp *= USERCAL_stRAMCAL.u16FuelPressurePGain;
				s32Temp += (s32FuelPressureErrorSum * USERCAL_stRAMCAL.u16FuelPressureIGain / 4);
				s32Temp += 10000000;
			}
			else if (2000 < s32Temp)
			{
				/* pressure more than 2000 kPa too high */
				s32Temp -= 2000;

				/* accumulate the feedback error */
				s32FuelPressureErrorSum += ((s32Temp * 500) / (sint32)CAM_u32RPMRaw);
				s32FuelPressureErrorSum = 20000 < s32FuelPressureErrorSum ? 20000 : s32FuelPressureErrorSum;
				s32FuelPressureErrorSum = -20000 > s32FuelPressureErrorSum ? -20000 : s32FuelPressureErrorSum;

				s32Temp *= USERCAL_stRAMCAL.u16FuelPressurePGain;
				s32Temp += (s32FuelPressureErrorSum * USERCAL_stRAMCAL.u16FuelPressureIGain / 4);
				s32Temp /= 2;
				s32Temp += 10000000;
			}
			else
			{
				s32Temp = 10000000;
				s32Temp += (s32FuelPressureErrorSum * USERCAL_stRAMCAL.u16FuelPressureIGain / 4);
			}

			/* add feedforward guesstimate */
			s32Temp -= (USERCAL_stRAMCAL.u16GDIValveFF * ((CAM_u32RPMRaw * MAP_tKiloPaFiltered) / 1000000));
			s32Temp += 10000 * USERCAL_stRAMCAL.u16FuelPressureControlOffset;

			s32Temp = (1000 * (sint32)USERCAL_stRAMCAL.u16GDIValveMin) < s32Temp ? s32Temp : (1000 * (sint32)USERCAL_stRAMCAL.u16GDIValveMin);
			s32Temp = (1000 * (sint32)USERCAL_stRAMCAL.u16GDIValveMax) > s32Temp ? s32Temp : (1000 * (sint32)USERCAL_stRAMCAL.u16GDIValveMax);

			FUEL_tFPAccumulate = s32Temp / CAM_u32RPMRaw;
			FUEL_tFPAccumulate -= (CAM_u32RPMRaw / 16);
			FUEL_tStartFPSolenoidDelay = (7000000 - 500 * CAM_u32RPMRaw) / CAM_u32RPMRaw;
		}
	}

	if ((TRUE == CTS_boCTSReady)
		&& (FALSE == FUEL_boFuelPrimed))
	{
		if ((0 == CAM_u32RPMRaw) && (TRUE == USERCAL_stRAMCAL.u8FuelPrimeEnable))
		{
			/* Prime the injectors */
			USER_vSVC(SYSAPI_enTEPMAsyncRequest, NULL, NULL, NULL);
			FUEL_boFuelPrimed = TRUE;
		}
		else
		{
			FUEL_boFuelPrimed = TRUE;
		}
	}
	else if ((2000 == u32FuelRunCount)
	&& (FALSE == FUEL_boFuelPrimed))
	{
		/* Finish prime */
		FUEL_boFuelPrimed = TRUE;
	}

	u32FuelRunCount++;

	if (EH_IO_Invalid != USERCAL_stRAMCAL.enFuelPumpRelay)
	{
		if (TRUE == FUEL_boFuelPumpOn)
		{
			enEHIOResource = USERCAL_stRAMCAL.enFuelPumpRelay;
			enTriState = IOAPI_enHigh;

			if (EH_IO_IIC1_SDA > enEHIOResource)
			{
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
			else if ((EH_VIO_REL1 <= enEHIOResource) && (EH_VIO_REL8 >= enEHIOResource))
			{
				enBit = enEHIOResource - EH_VIO_REL1;
				RELAYS_vOutputBit(enBit, IOAPI_enHigh == enTriState);
			}
		}
		else
		{
			enEHIOResource = USERCAL_stRAMCAL.enFuelPumpRelay;
			enTriState = IOAPI_enLow;

			if (EH_IO_IIC1_SDA > enEHIOResource)
			{
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
			else if ((EH_VIO_REL1 <= enEHIOResource) && (EH_VIO_REL8 >= enEHIOResource))
			{
				enBit = enEHIOResource - EH_VIO_REL1;
				RELAYS_vOutputBit(enBit, IOAPI_enHigh == enTriState);
			}
		}
	}

	if (FUEL_boNewSample == TRUE)
	{
		FUEL_u32ADSamples[u32SampleCount] = FUEL_u32ADCRaw;
		u32SampleCount = u32SampleCount % FUEL_nSampleCount;

		if (0 == u32SampleCount)
		{
			FUEL_u32ADCFiltered = USERMATH_u32DiscardAndAverage32(FUEL_u32ADSamples, FUEL_nSampleCount, 2);/*CR1_1*/
			FUEL_u32ADCFiltered *= 0x100;
		}

		u32Temp = ((FUEL_u32ADCFiltered / 100U) * USERCAL_stRAMCAL.s32FuelPressureSensorGain) / 10000u;

		if (0 <= USERCAL_stRAMCAL.s32FuelPressureSensorOffset)
		{
			FUEL_tKiloPaFiltered = u32Temp + USERCAL_stRAMCAL.s32FuelPressureSensorOffset;
		}
		else
		{
			if (u32Temp >= -USERCAL_stRAMCAL.s32FuelPressureSensorOffset)
			{
				FUEL_tKiloPaFiltered = u32Temp + USERCAL_stRAMCAL.s32FuelPressureSensorOffset;
			}
			else
			{
				FUEL_tKiloPaFiltered = 0;
			}
		}

		FUEL_boNewSample = FALSE;
		u32SampleCount++;
	}

	boFuelCutsActive = SENSORS_boGetAuxActive(SENSORS_enAUX_LAUNCH_LOW);
	boFuelCutsActive |= SENSORS_boGetAuxActive(SENSORS_enAUX_LAUNCH_HIGH);
	boFuelCutsActive |= (0x100 != TORQUE_u32FuelTorqueModifier);

	if (TRUE == boFuelCutsActive)
	{
		if (EST_nIgnitionReqPrimary == EST_enIgnitionTimingRequest)
		{
			/* Calculate the current spread for fuel cuts */
			USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadFuelCutsIDX,
					NULL, NULL);

			/* Lookup the current value for fuel cuts */
			USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableFuelCutsIDX,
				NULL, NULL);

			u32FuelCutPercent = (uint32)FUEL_u16FuelCutsPercent;
		}
		else
		{
			if (EST_nIgnitionReqDSGCutsStage3 == EST_enIgnitionTimingRequest)
			{
				if (2000 < CAM_u32RPMFiltered)
				{
					if (100000 < MAP_tKiloPaFiltered)
					{
						u32FuelCutPercent = (MAP_tKiloPaFiltered - 100000) / 1000;
						u32FuelCutPercent =
								USERCAL_stRAMCAL.u16TorqueReductionMaxFuelCut > u32FuelCutPercent ?
										u32FuelCutPercent : USERCAL_stRAMCAL.u16TorqueReductionMaxFuelCut;
					}
					else
					{
						u32FuelCutPercent = 0;
					}
				}
				else
				{
					u32FuelCutPercent = 0;
				}
			}
			else
			{
				/* No fuel cut pre-shift */
				u32FuelCutPercent = 0;
			}
		}

		/* Set the fuel cuts */
		USER_vSVC(SYSAPI_enSetFuelCuts, &u32FuelCutPercent, &FUEL_u32FuelChannelsMask, &FUEL_u32FuelChannelsCount);
	}
	else
	{
		u32FuelCutPercent = 0;

		if (boFuelCutsActivePrev != boFuelCutsActive)
		{
			USER_vSVC(SYSAPI_enSetFuelCuts, &u32FuelCutPercent, &FUEL_u32FuelChannelsMask, &FUEL_u32FuelChannelsCount);
		}
	}

	boFuelCutsActivePrev = boFuelCutsActive;

	/* FME fuel cuts */
#ifdef BUILD_FME
	if (0 != USERCAL_stRAMCAL.u8DBSlaveConfig)
	{
		boAirflowFMEEnable = AFM_tAirFlowAFMUg < USERCAL_stRAMCAL.u32AirflowFMELimitLow ?
				FALSE : boAirflowFMEEnable;

		boAirflowFMEEnable = AFM_tAirFlowAFMUg > USERCAL_stRAMCAL.u32AirflowFMELimitHigh ?
				TRUE : boAirflowFMEEnable;

		/* Override safety airflow limit set too high > 15g/s */
		if (15000000u < USERCAL_stRAMCAL.u32AirflowFMELimitHigh) boAirflowFMEEnable = TRUE;

		u32Temp = (u32FuelRunCount >> 4) & 0x7;

		boFuelCutsActive = FALSE;

		if (TRUE == boAirflowFMEEnable)
		{
			boFuelCutsActive |= (FME_enFaultActive == FME_enGetDiagState(FME_enPPSPair));
			boFuelCutsActive |= (FME_enFaultActive == FME_enGetDiagState(FME_enPedalTransfer));
			boFuelCutsActive |= (FME_enFaultActive == FME_enGetDiagState(FME_enTPSMSingle));
			boFuelCutsActive |= (FME_enFaultActive == FME_enGetDiagState(FME_enPPSMSingle));
			boFuelCutsActive |= (FME_enFaultActive == FME_enGetDiagState(FME_enPPSSSingle));
		}

		if (TRUE == boFuelCutsActive)
		{
			FUEL_aboFMECutFlag[u32Temp] = TRUE;
		}
		else
		{
			FUEL_aboFMECutFlag[u32Temp] = FALSE;
		}
	}
#endif //BUILD_FME

}

void FUEL_vCalculateFuellingValues()
{
	FUEL_boCalculatePending = TRUE;
}

static void FUEL_vCyclicCalculate(void)
{
	static bool boAFMTransientControl;
	static bool boFuelAFRCutoff;
	static uint32 u32OverrunCutCount;
	uint32 u32SeqIDX;
	uint8 u8SeqFuelBit;
	uint32 u32FuelCycleTimeUs;
	uint32 u32Temp = 0;
	static bool boSequentialMode;
	static bool boHighDuty;
	static uint32 u32SequentialModeCount;
	uint32 u32MaxFuel;
	uint32 u32ReturnlessPressureKpa;
	uint16 u16FuelFlowRate;

	if ((FALSE == FUEL_boFuelPrimed) &&
			(400 > CAM_u32RPMRaw) &&
			(TRUE == USERCAL_stRAMCAL.u8FuelPrimeEnable))
	{
		FUEL_tTimePredictedUs[0] = CTS_u32Primer;
		FUEL_tTimePredictedUs[1] = CTS_u32Primer;
	}
	else
	{
		uint32 u32Temp1;
		uint32 u32Temp2;
		sint32 s32Temp;

		/* Calculate the current spread for AfmTF */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadAfmTFIDX,
				NULL, NULL);		
	
		/* Lookup the current AfmTF value for AfmTF */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableAfmTFIDX,
				NULL, NULL);


		/* Max delta ramp airflow */
		if (AFM_tAirFlowAFMRawUg >= AFM_tAirFlowAFMUg)
		{
			u32Temp = USERCAL_stRAMCAL.u16AFMMaxCycleDeltaUgPos * 1000;

			if (u32Temp < (AFM_tAirFlowAFMRawUg - AFM_tAirFlowAFMUg))
			{
				AFM_tAirFlowAFMUg += u32Temp;
			}
			else
			{
				AFM_tAirFlowAFMUg = AFM_tAirFlowAFMRawUg;
			}
		}
		else
		{
			u32Temp = USERCAL_stRAMCAL.u16AFMMaxCycleDeltaUgNeg * 1000;

			if (u32Temp < (AFM_tAirFlowAFMUg - AFM_tAirFlowAFMRawUg))
			{
				if (AFM_tAirFlowAFMUg > u32Temp)
				{
					AFM_tAirFlowAFMUg -= u32Temp;
				}
				else
				{
					AFM_tAirFlowAFMUg = 0;
				}
			}
			else
			{
				AFM_tAirFlowAFMUg = AFM_tAirFlowAFMRawUg;
			}
		}


		if (0 != USERCAL_stRAMCAL.u8VariableFuelPressureEnable)
		{
			/* Calculate the current spread for Fuel Flow */
			USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadFuelFlowIDX,
					NULL, NULL);

			/* Lookup the current fuel flow rate */
			USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableFuelFlowIDX,
					NULL, NULL);
		}

		boAFMTransientControl = USERCAL_stRAMCAL.u32AFMTransientControlRPMLimit > CAM_u32RPMRaw ? TRUE : boAFMTransientControl;
		boAFMTransientControl = (USERCAL_stRAMCAL.u32AFMTransientControlRPMLimit + 100) < CAM_u32RPMRaw ? FALSE : boAFMTransientControl;										
				
		if ((USERCAL_stRAMCAL.u16RPMRunThreshold < CAM_u32RPMRaw) || (USERCAL_stRAMCAL.u8CrankingAirflowEnable == FALSE))
		{
			if ((TRUE == USERCAL_stRAMCAL.u8EnableBackupAirflowTransients) &&
			(TRUE == boAFMTransientControl))
			{
				if (AFM_tAirFlowAFMUg > AFM_tAirFlowBackupUg)
				{
					u32Temp1 = (AFM_tAirFlowBackupUg * 9) / 8;
					FUEL_tPredictedAirFlowUg = u32Temp > AFM_tAirFlowAFMUg ? AFM_tAirFlowAFMUg : u32Temp;
				}
				else
				{
					u32Temp1 = (AFM_tAirFlowBackupUg * 7) / 8;
					FUEL_tPredictedAirFlowUg = u32Temp > AFM_tAirFlowAFMUg ? u32Temp : AFM_tAirFlowAFMUg;
				}
			}
			else
			{
				if (USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim > TPS_tThetaFiltered)
				/* At small throttle angles */
				{
					if (TRUE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputClosed)
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowAFMUg;
					}
					else if ((FALSE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputClosed) &&
                             (TRUE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputOpen))
					{
						u32Temp1 = TPS_tThetaFiltered * (AFM_tAirFlowAFMUg / USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim);
						u32Temp2 = (USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim - TPS_tThetaFiltered) * (AFM_tAirFlowVEUg / USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim);

						FUEL_tPredictedAirFlowUg = u32Temp1 + u32Temp2;
					}
					else
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowVEUg;
					}
				}
				else
				{
					if (TRUE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputOpen)
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowAFMUg;
					}
					else
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowVEUg;
					}
				}
			}
		}
		else
		{
			/* Calculate the current spread for cranking airflow */
			USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadCrankingAirflowIDX,
			NULL, NULL);
		
			/* Lookup the current value for cranking airflow */
			USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableCrankingAirflowIDX,
			NULL, NULL);
			FUEL_tPredictedAirFlowUg = 1000 * FUEL_u16CrankingAirflow;
		}

		if (TRUE == FUEL_boFuelPrimed)
		{
			CLO2_vRun();
			IAC_vCallBack(NULL);
		}


		/* Add manifold capacitance transitory airflow */
		if (500000 < MAP_s32ManDeltaChargeMassPerSUg)
		{
			FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
		}
		else if ((-500000 > MAP_s32ManDeltaChargeMassPerSUg) && (FUEL_tPredictedAirFlowUg > -MAP_s32ManDeltaChargeMassPerSUg))
		{
			FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
		}
		
		FUEL_tPredictedFuelFlowUg = (10u * FUEL_tPredictedAirFlowUg) / FUEL_u16TAFR;
	
		/* Add CTS and ATS correction */
		FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10UL) * (CTS_u32FuelMultiplier + ATS_u32FuelMultiplier - 1000)) / 100UL;	

		/* Add TPS moving enrichment */
		FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10UL) * (TPS_u32MovingTPSEnrichment)) / 100UL;	

		/* Add TPS tip in enrichment */
		FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10UL) * (TPS_u32TipInEnrichment)) / 100UL;

	
		s32Temp = (sint32)FUEL_tPredictedFuelFlowUg + FILM_s32FilmLoadUgDeltaApplied;

		if (0 <= s32Temp)
		{
			FUEL_tPredictedFuelFlowUg = (uint32)s32Temp;
		}
		else
		{
			FUEL_tPredictedFuelFlowUg = 0;
		}
		
		/* Add STT trim */
		FUEL_tPredictedFuelFlowPerInjectionNg[0] = (12 / USERCAL_stRAMCAL.u8InjDivide) * (100 * FUEL_tPredictedFuelFlowUg / CAM_u32RPMRaw);//was filtered!!
		FUEL_tPredictedFuelFlowPerInjectionNg[0] *= CLO2_u32STT[0];
		FUEL_tPredictedFuelFlowPerInjectionNg[1] = (12 / USERCAL_stRAMCAL.u8InjDivide) * (100 * FUEL_tPredictedFuelFlowUg / CAM_u32RPMRaw);//was filtered!!
		FUEL_tPredictedFuelFlowPerInjectionNg[1] *= CLO2_u32STT[1];

		/* Add fuel pressure trim */
		if ((USERCAL_stRAMCAL.u8ReturnlessEnable == TRUE) && (FALSE == USERCAL_stRAMCAL.u8VariableFuelPressureEnable))
		{
			u32ReturnlessPressureKpa = 1000 * USERCAL_stRAMCAL.u16ReturnlessPressureKPa;
			u32Temp = MAP_tKiloPaFiltered + u32ReturnlessPressureKpa;
			u32Temp /= (101 + (u32ReturnlessPressureKpa / 1000));
			u32Temp = 1000 > u32Temp ? u32Temp : 999;
			u32Temp = USERMATH_u32GetSquareRoot(u32Temp);

			FUEL_tPredictedFuelFlowPerInjectionNg[0] /= 1000;
			FUEL_tPredictedFuelFlowPerInjectionNg[0] *= u32Temp;
			FUEL_tPredictedFuelFlowPerInjectionNg[1] /= 1000;
			FUEL_tPredictedFuelFlowPerInjectionNg[1] *= u32Temp;
		}

		if (FALSE == USERCAL_stRAMCAL.u8VariableFuelPressureEnable)
		{
			u16FuelFlowRate = (uint32)USERCAL_stRAMCAL.u16InjFlowRate;
		}
		else
		{
			u16FuelFlowRate = FUEL_u16FuelFlowRate;
		}

		u32Temp = (uint32)USERCAL_stRAMCAL.u8CylCount * u16FuelFlowRate;
		FUEL_tTimePredictedUs[0] = FUEL_tPredictedFuelFlowPerInjectionNg[0] / u32Temp;
		FUEL_tTimePredictedUs[1] = FUEL_tPredictedFuelFlowPerInjectionNg[1] / u32Temp;

		/* Trim short pulse */		
		if (USERCAL_stRAMCAL.u16MinLinearFuelPulse > FUEL_tTimePredictedUs[0])
		{
			FUEL_tTimePredictedUsInput = FUEL_tTimePredictedUs[0];
		    (void)BOOSTED_boIndexAndCalculateTable(FUEL_tSpreadInjShortOpeningIDX, FUEL_tTableInjShortOpeningIDX);

			/* Pragmatic workaround - predicted event < 1.6 ms */
			if (USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] > FUEL_tTimePredictedUs[0])
			{
				u32Temp = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - FUEL_tTimePredictedShortOpeningUs;
				u32Temp *= (BVM_tBattVolts - 4000);
				u32Temp /= 11000;
				FUEL_tTimePredictedShortOpeningUs = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - u32Temp;
			}

			FUEL_tTimePredictedUs[0] = FUEL_tTimePredictedShortOpeningUs;
		}	

		if (USERCAL_stRAMCAL.u16MinLinearFuelPulse > FUEL_tTimePredictedUs[1])
		{
			FUEL_tTimePredictedUsInput = FUEL_tTimePredictedUs[1];
			(void)BOOSTED_boIndexAndCalculateTable(FUEL_tSpreadInjShortOpeningIDX, FUEL_tTableInjShortOpeningIDX);

			/* Pragmatic workaround - predicted event < 1.6 ms */
			if (USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] > FUEL_tTimePredictedUs[1])
			{
				u32Temp = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - FUEL_tTimePredictedShortOpeningUs;
				u32Temp *= (BVM_tBattVolts - 4000);
				u32Temp /= 11000;
				FUEL_tTimePredictedShortOpeningUs = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - u32Temp;
			}

			FUEL_tTimePredictedUs[1] = FUEL_tTimePredictedShortOpeningUs;
		}

		boFuelAFRCutoff = (FUEL_u16TAFR < USERCAL_stRAMCAL.u16AFRReinstate) ? FALSE : boFuelAFRCutoff;
		boFuelAFRCutoff = (FUEL_u16TAFR > USERCAL_stRAMCAL.u16AFRCutoff) ? TRUE : boFuelAFRCutoff;
	}	

	FUEL_tTimePredictedUs[0] = TRUE == boFuelAFRCutoff ? 0 : FUEL_tTimePredictedUs[0];
	FUEL_tTimePredictedUs[1] = TRUE == boFuelAFRCutoff ? 0 : FUEL_tTimePredictedUs[1];

	/* Apply DSG torque reduce trim */
	if ((EST_nIgnitionReqDSGStage1 == EST_enIgnitionTimingRequest) ||
			(EST_nIgnitionReqDSGStage2 == EST_enIgnitionTimingRequest) ||
			(EST_nIgnitionReqDSGCutsStage3 == EST_enIgnitionTimingRequest))
	{
		FUEL_tTimePredictedUs[0] = TORQUE_u32FuelTorqueModifier * FUEL_tTimePredictedUs[0] / 0x100;
		FUEL_tTimePredictedUs[1] = TORQUE_u32FuelTorqueModifier * FUEL_tTimePredictedUs[1] / 0x100;
	}

	/* Increment fuel consumed for BC */
	FUEL_u32FuelConsumed += FUEL_tTimePredictedUs[0];

	/* Manage overrun fuel cut */
#ifdef BUILD_BSP_VSS_CAN
	if ((TRUE == MAP_boHighVacuum) &&
		(TRUE == TPS_boThrottleClosed) &&
		(TRUE == IAC_boOverrunCutRPMEnable) &&
		(0 != SENSORS_u16CANVSS) &&
		(0xffff != SENSORS_u16CANVSS))
#else
	if ((TRUE == MAP_boHighVacuum) &&
	    (TRUE == TPS_boThrottleClosed) &&
	    (TRUE == IAC_boOverrunCutRPMEnable))
#endif
	{
		u32OverrunCutCount = 40 > u32OverrunCutCount ? u32OverrunCutCount + 1 : u32OverrunCutCount;

		for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
		{			
			FUEL_aboSeqOverrunCutFlag[u32SeqIDX] =	u32OverrunCutCount > (10 + (u32SeqIDX << 3)) ?	TRUE : FALSE;
		}
	}
	else if (TRUE == IAC_boOverrunCutRPMEnable)
	{		
		u32OverrunCutCount = 3 < u32OverrunCutCount ? u32OverrunCutCount - 4 : 0;

		for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
		{
			FUEL_aboSeqOverrunCutFlag[u32SeqIDX] =	u32OverrunCutCount > (10 + (u32SeqIDX << 3)) ?	TRUE : FALSE;
		}
	}
	else
	{
		u32OverrunCutCount = 0;

		for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
		{
			FUEL_aboSeqOverrunCutFlag[u32SeqIDX] =	FALSE;
		}
	}

	u8SeqFuelBit = 1;

	/* Apply overrun and over-speed fuel cuts */
	for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
	{
		if ((TRUE == FUEL_aboSeqRevCutFlag[u32SeqIDX]) ||
#ifdef BUILD_FME
			(TRUE == FUEL_aboFMECutFlag[u32SeqIDX]) ||
#endif //BUILD_FME
			(TRUE == FUEL_aboSeqOverrunCutFlag[u32SeqIDX]))
		{
			FUEL_tTimeHoldUs[u32SeqIDX] = 600;
			FUEL_tTimeHold[u32SeqIDX] = 400;
		}
		else
		{
			CPU_xEnterCritical();
			if (0 == (u8SeqFuelBit & USERCAL_stRAMCAL.u8SeqFuelAssign))
			{
				FUEL_tTimeHoldUs[u32SeqIDX] = FUEL_tTimePredictedUs[0] + (uint32)FUEL_u16InjResponse;
				FUEL_tTimeHoldUs[u32SeqIDX] = (uint32)((sint32)FUEL_tTimeHoldUs[u32SeqIDX] + USERCAL_stRAMCAL.s16SequenceFuelOffset[u32SeqIDX]);
				FUEL_tTimeHold[u32SeqIDX] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[u32SeqIDX]);
			}
			else
			{
				FUEL_tTimeHoldUs[u32SeqIDX] =  FUEL_tTimePredictedUs[1] + (uint32)FUEL_u16InjResponse;
				FUEL_tTimeHoldUs[u32SeqIDX] = (uint32)((sint32)FUEL_tTimeHoldUs[u32SeqIDX] + USERCAL_stRAMCAL.s16SequenceFuelOffset[u32SeqIDX]);
				FUEL_tTimeHold[u32SeqIDX] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[u32SeqIDX]);
			}	
			CPU_xExitCritical();		
		}
			
		u8SeqFuelBit *= 2;
	}
	
	FUEL_boCalculatePending = FALSE;

	u32MaxFuel = MAX(FUEL_tTimeHoldUs[0], FUEL_tTimeHoldUs[1]);
	u32MaxFuel = MAX(u32MaxFuel, FUEL_tTimeHoldUs[2]);
	u32MaxFuel = MAX(u32MaxFuel, FUEL_tTimeHoldUs[3]);
	
	u32FuelCycleTimeUs = 120000000 / (CAM_u32RPMRaw * USERCAL_stRAMCAL.u8InjDivide);
	u32Temp = (0x10000 * u32MaxFuel) / u32FuelCycleTimeUs;


	/* Condition the high duty flag */
	boHighDuty = 40000 < u32Temp ? TRUE : boHighDuty;
	boHighDuty = 30000 > u32Temp ? FALSE : boHighDuty;


	/* Clear sequential counter */
	if (250 > CAM_u32RPMFiltered)
	{
		u32SequentialModeCount = 0;
	}

	/* Check sequential mode */
	if ((350 > CAM_u32RPMFiltered) || (true == boHighDuty) || (FALSE == FUEL_bo720Injection))
	{
		u32SequentialModeCount = 0 < u32SequentialModeCount ? u32SequentialModeCount - 1 : 0;
	}
	else
	{
		u32SequentialModeCount = FUEL_nSeqModeCountLimit > u32SequentialModeCount ? u32SequentialModeCount + 1 : FUEL_nSeqModeCountLimit;
	}


	/* Condition the sequential flag */
	boSequentialMode = FUEL_nSeqModeCountLimit <= u32SequentialModeCount ? TRUE : boSequentialMode;
	boSequentialMode = 0 == u32SequentialModeCount ? FALSE : boSequentialMode;


	if (TRUE == boSequentialMode)
	{
		if (49500 > u32Temp)
		{
			/* Assign the end of event angles */
			FUEL_tStartHoldFraction[0] = 50000 - u32Temp;
			FUEL_tStartHoldFraction[1] = 50000 - u32Temp;
			FUEL_tStartHoldFraction[2] = 50000 - u32Temp;
			FUEL_tStartHoldFraction[3] = 50000 - u32Temp;
		}
		else
		{
			/* Assign the end of event angles */
			FUEL_tStartHoldFraction[0] = 500;
			FUEL_tStartHoldFraction[1] = 500;
			FUEL_tStartHoldFraction[2] = 500;
			FUEL_tStartHoldFraction[3] = 500;
		}
	}
	else
	{
		/* Assign the start of event angles */
		//FUEL_tStartHoldFraction[0] = 1500;
		//FUEL_tStartHoldFraction[1] = 1500;
		//FUEL_tStartHoldFraction[2] = 1500;
		//FUEL_tStartHoldFraction[3] = 1500;
		if (2000 > CAM_u32RPMRaw)
		{
			FUEL_tStartHoldFraction[0] = 2000 + ((2000 - CAM_u32RPMRaw) << 3);
			FUEL_tStartHoldFraction[1] = 2000 + ((2000 - CAM_u32RPMRaw) << 3);
			FUEL_tStartHoldFraction[2] = 2000 + ((2000 - CAM_u32RPMRaw) << 3);
			FUEL_tStartHoldFraction[3] = 2000 + ((2000 - CAM_u32RPMRaw) << 3);
		}
		else
		{
			FUEL_tStartHoldFraction[0] = 2000;
			FUEL_tStartHoldFraction[1] = 2000;
			FUEL_tStartHoldFraction[2] = 2000;
			FUEL_tStartHoldFraction[3] = 2000;
		}
	}		
}


void FUEL_vTerminate(puint32 const pu32Arg)
{

}


void FUEL_vCallBack(puint32 const pu32Arg)
{

}

static void FUEL_vTEPMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	//FUEL_boFuelPrimed = TRUE;
}

static void FUEL_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	FUEL_u32ADCRaw = MIN(u32ADCResult, SENSORS_nSENSORADMAX);
	FUEL_boNewSample = TRUE;
}

static void FUEL_vCalcFuelChannels(void)
{
	uint32 u32IDX;
	uint32 u32Mask = 1;
	FUEL_u32FuelChannelsCount = 0;

	for (u32IDX = 0; u32IDX < 32; u32IDX++)
	{
		if (0 != (u32Mask & FUEL_u32FuelChannelsMask))
		{
			FUEL_u32FuelChannelsCount++;
		}

		u32Mask *= 2;
	}
}

#endif //BUILD_USER
