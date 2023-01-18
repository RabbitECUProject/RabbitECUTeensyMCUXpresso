/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      LSU4.X                                                 */
/* DESCRIPTION:        This code module initialises the required ADC, DAC     */
/*                     CTRL, spread, table, DIO and TEPM resources for        */
/*                     managing the nernst cell voltage and impedence         */
/*                     measurement for the LSU4.X sensor/s                    */
/* FILE NAME:          LSU4X.c                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _LSU4X_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "LSU4X.h"

/* LOCAL MACRO DEFINITIONS ****************************************************/
#define LSU4X_nADConfigCount (sizeof(LSU4X_rastADConfig) / sizeof(LSU4X_tstADConfig))

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
DACAPI_ttOutputVoltage LSU4X_atDACVoltageQueue[LSU_DEVICE_COUNT][LSU4X_nDACQueueSize];
GPM6_ttVolts LSU4X_atNernstSenseVoltsDC[LSU_DEVICE_COUNT];
//ASAMREMOVED!! mode=readvalue name="Nernst Sense 1 Volts" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="Nernst Sense Volts Left"
GPM6_ttVolts LSU4X_atNernstSenseVoltsACHigh[LSU_DEVICE_COUNT];/*CR1_42*/
GPM6_ttVolts LSU4X_atNernstSenseVoltsACLow[LSU_DEVICE_COUNT];/*CR1_42*/
sint32 LSU4X_ai32PumpCurrentADC[LSU_DEVICE_COUNT];
sint32 LSU4X_ai32PumpCurrentADC[LSU_DEVICE_COUNT];
sint32 LSU4X_ai32PumpCurrentADCFiltered[LSU_DEVICE_COUNT];
GPM6_ttMicroAmps LSU4X_atPumpCurrentMicroAmps[LSU_DEVICE_COUNT];/*CR1_42*/
//ASAMREMOVED!! mode=readvalue name="Pump Current 1" type=sint32 offset=0 min=-20000 max=20000 m=0.001 b=0 units="uA" format=4.3 help="Pump 1 Current (uA)"
GPM6_ttMicroAmps LSU4X_atPumpCurrentMicroAmpsNorm[LSU_DEVICE_COUNT];/*CR1_42*/
CTRLAPI_ttPIDIDX LSU4X_atPIDLSU4XIDX[LSU_DEVICE_COUNT];
SPREADAPI_ttSpreadIDX LSU4X_atSpreadLSU4XIDX[LSU_DEVICE_COUNT];
TABLEAPI_ttTableIDX LSU4X_atTableLSU4XIDX[LSU_DEVICE_COUNT];
sint32 LSU4X_ai32PIDTarget[LSU_DEVICE_COUNT];
sint32 LSU4X_ai32PIDFeedback[LSU_DEVICE_COUNT];
sint32 LSU4X_ai32PIDOutput[LSU_DEVICE_COUNT];
uint32 LSU4X_au32SensorReadyCount[LSU_DEVICE_COUNT];
uint8 LSU4X_au8ACCallBackWait[LSU_DEVICE_COUNT];
IOAPI_tenTriState LSU4X_enTriState;	


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : LSU4X_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void LSU4X_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);/*CR1_40*/

/*******************************************************************************
* Interface        : LSU4X_vEnablePumpControl
*
* Implementation   : Function to determine if the pump control should be enabled
*
* Parameter
*    Par1          : NIL
*
* Return Value     : NIL
*******************************************************************************/
static void LSU4X_vEnablePumpControl(void);


/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/
const LSU4X_tstADConfig LSU4X_rastADConfig[] = LSU4X_nADConfig;/*CR1_41*/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void LSU4X_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;		
	CTRLAPI_tenCTRLType enCTRLType;
	ADCAPI_tstADCCB stADCCB;
	DACAPI_tstDACCB stDACCB;	
	CTRLAPI_tstPIDCB stPIDCB;
	SPREADAPI_tstSpreadCB stSpreadCB;	
	TABLEAPI_tstTableCB stTableCB;
	uint32 u32ADConfigIDX;
	bool boInitFailed = FALSE;
	
#ifdef BUILD_SPARKDOG_PF
	/* not yet tested */
	return;
#endif

	LSU4X_au32DCSampleCount[0] = 0;
	
	/* Request and initialise AD converter resources */	
	for (u32ADConfigIDX = 0; u32ADConfigIDX < LSU4X_nADConfigCount; u32ADConfigIDX++)
	{
		enEHIOResource = LSU4X_rastADConfig[u32ADConfigIDX].enEHIOResource;
		enEHIOType = LSU4X_rastADConfig[u32ADConfigIDX].enEHIOType;
		stADCCB.enSamplesAv = LSU4X_rastADConfig[u32ADConfigIDX].enSamplesAv;
		stADCCB.enDiffGain = LSU4X_rastADConfig[u32ADConfigIDX].enDiffGain;		
		stADCCB.pfResultCB = LSU4X_rastADConfig[u32ADConfigIDX].pfResultCB;
		stADCCB.enTrigger = LSU4X_rastADConfig[u32ADConfigIDX].enTrigger;				
			
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{											
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stADCCB);	

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) boInitFailed = TRUE;			
		}		
		else
		{	
			boInitFailed = TRUE;
			break;
		}
	}
	
	/* Request and initialise DACs */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		enEHIOType = IOAPI_enDAC;
		enEHIOResource = LSU4X_nDACResource;

		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
		
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{		
			stDACCB.enQueueDepth = (DACAPI_tenQueueDepth)(LSU4X_nDACQueueSize - 1);
			stDACCB.boWaterMarkEventEnable = FALSE;
			stDACCB.boPointerTopEventEnable = FALSE;
			stDACCB.boPointerBottomEventEnable = FALSE;
			stDACCB.enTriggerType = DACAPI_enSoftwareTrigger;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stDACCB);	
		}
	}
	
	/* Request and initialise the LSUX PID controllers */
	stPIDCB.enPIDRate = CTRLAPI_enPID1ms;
	stPIDCB.pi32Target = &LSU4X_ai32PIDTarget[0];
	stPIDCB.pi32Feedback = &LSU4X_ai32PIDFeedback[0];	
	stPIDCB.pi32Output = &LSU4X_ai32PIDOutput[0];	
	stPIDCB.u8PTerm = 0xff;
	stPIDCB.u8ITerm = 0xff;
	stPIDCB.u8DTerm = 0x20;	
	stPIDCB.boNoWindUp = TRUE;	
	stPIDCB.boReset = FALSE;	
	stPIDCB.boResetIntegrator = FALSE;		
	stPIDCB.boRun = TRUE;
	stPIDCB.i32OutputMin = -1 * (0x80 * LSU4X_nDACRange) + 100;
	stPIDCB.i32OutputMax = 0x80 * LSU4X_nDACRange - 100;
	enCTRLType = CTRLAPI_enPID;
	
	USER_vSVC(SYSAPI_enInitialiseCTRLResource, (void*)&enCTRLType, (void*)&stPIDCB, NULL);	
	LSU4X_atPIDLSU4XIDX[0] = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(CTRLAPI_ttPIDIDX)pstSVCDataStruct->tClientHandle : -1;
	

	/* Request and initialise required Kernel managed spreads */
	stSpreadCB.pvSourceData = (void*)&LSU4X_atPumpCurrentMicroAmpsNorm[0];
	stSpreadCB.pvSpreadData = (void*)&USERCAL_stRAMCAL.aUserCalPumpSpread[0];
	stSpreadCB.enDataType = TYPE_enInt32;
	stSpreadCB.s16SpreadSize = 17;
	stSpreadCB.enSpreadRate = SPREADAPI_enSpread1ms;
	stSpreadCB.pfResultCB = NULL;
	
	USER_vSVC(SYSAPI_enInitialiseSpreadResource, (void*)&stSpreadCB, NULL, NULL);	
	LSU4X_atSpreadLSU4XIDX[0] = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(SPREADAPI_ttSpreadIDX)pstSVCDataStruct->tClientHandle : -1;		
	
	boInitFailed = ((-1 == LSU4X_atSpreadLSU4XIDX[0])	|| (-1 == LSU4X_atSpreadLSU4XIDX[1])) ?
		TRUE : boInitFailed;
		
		
	/* Request and initialise required Kernel managed tables */
	stTableCB.pvTableData = (void*)&USERCAL_stRAMCAL.aUserCalPumpLambda[0];
	stTableCB.pvOutputData = (void*)&LSU4X_atLambdaRaw[0];
	stTableCB.enDataType = TYPE_enInt16;
	stTableCB.i16TableSize = 17;
	stTableCB.tSpreadIDX = LSU4X_atSpreadLSU4XIDX[0];
	stTableCB.pfResultCB = NULL;
	
	USER_vSVC(SYSAPI_enInitialiseTableResource, (void*)&stTableCB, NULL, NULL);	
	LSU4X_atTableLSU4XIDX[0] = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(TABLEAPI_ttTableIDX)pstSVCDataStruct->tClientHandle : -1;		
	
	boInitFailed = (-1 == LSU4X_atTableLSU4XIDX[0]) ?
		TRUE : boInitFailed;


	/* Request and initialise the LSU sensor 1 and 2 AC control lines */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		enEHIOResource = LSU4X_nSWHResource;
		enEHIOType = IOAPI_enDIOOutput;	
		enDriveStrength = IOAPI_enStrong;			
			
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{											
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&enDriveStrength);	

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) boInitFailed = TRUE;			
		}		
		else
		{	
			boInitFailed = TRUE;
		}
	}
	
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		enEHIOResource = LSU4X_nSWLResource;
		enEHIOType = IOAPI_enDIOOutput;	
		enDriveStrength = IOAPI_enStrong;			
			
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{											
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&enDriveStrength);	

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) boInitFailed = TRUE;			
		}		
		else
		{	
			boInitFailed = TRUE;
		}
	}		
	
	LSU4X_au32SensorReadyCount[0] = 0;
	LSU4X_ai32PIDTarget[0] = USERCAL_stRAMCAL.userCalVNernstTarget;
}

void LSU4X_vRun(uint32* const pu32Arg )
{
	static uint32 u32RunCounter = 0;/*CR1_44*/
	sint32 s32Temp;
	sint32 s32TempDelta;
	IOAPI_tenEHIOResource enEHIOResource;

#ifdef BUILD_SPARKDOG_PF
	/* not yet tested */
	return;
#endif

	ADCAPI_tenTrigger enTrigger = ADCAPI_enTrigger1;	
		
	LSU4X_atDACVoltageQueue[0][0] = (LSU4X_nDACRange / 2) + (LSU4X_ai32PIDOutput[0] / 0x100);

	
	if (0 == (u32RunCounter++ % 0x2))
	{
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, NULL, NULL);
	}
	
	if (0 == (u32RunCounter % LSU4X_nPWMCycCount))
	{
		LSUH_vTurnHeaterOn();
	}
	else if ((LSUH_au32HeaterDuty[0] / 25) <= (u32RunCounter % LSU4X_nPWMCycCount))
	{
		if (true == LSUH_aboHeaterIsOn[0])
		{
		    LSUH_vTurnHeaterOff();
		}
	}
	

	if (0 == (u32RunCounter % 0x2))
	{
		/* Set the AC control lines */	
		enEHIOResource = LSU4X_nSWHResource;
		LSU4X_enTriState = (0 == (u32RunCounter & 0x2)) ? IOAPI_enLow : IOAPI_enHigh;
		
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&LSU4X_enTriState,	(void*)NULL);	
		
		enEHIOResource = LSU4X_nSWLResource;
		LSU4X_enTriState = (0 == (u32RunCounter & 0x2)) ? IOAPI_enHigh : IOAPI_enLow;
		
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&LSU4X_enTriState,	(void*)NULL);		
	}
	
	if (LSU4X_enTriState == IOAPI_enHigh)
	{
		enEHIOResource = LSU4X_nDACResource;
		LSU4X_atDACVoltageQueue[0][0] = (LSU4X_nDACRange / 2) + (LSU4X_ai32PIDOutput[0] / 0x100) - 65;
		USER_vSVC(SYSAPI_enWriteDACQueue, (void*)&enEHIOResource, (void*)&LSU4X_atDACVoltageQueue[0][0], NULL);
	}
	else
	{
		enEHIOResource = LSU4X_nDACResource;
		LSU4X_atDACVoltageQueue[0][0] = (LSU4X_nDACRange / 2) + (LSU4X_ai32PIDOutput[0] / 0x100) + 65;
		USER_vSVC(SYSAPI_enWriteDACQueue, (void*)&enEHIOResource, (void*)&LSU4X_atDACVoltageQueue[0][0], NULL);
	}


	LSU4X_vEnablePumpControl();

	/* Buffer the nernst cell PID controller feedback */
	LSU4X_ai32PIDFeedback[0] = LSU4X_atNernstSenseVoltsDC[0];
	
	/* Calculate the pump currents */
	s32Temp = LSU4X_ai32PumpCurrentADCFiltered[0];

	/* Add offset correction */
	if (USERCAL_stRAMCAL.ai32UserCalPumpCurrentOffset[0] <= s32Temp)
	{
		s32Temp -= USERCAL_stRAMCAL.ai32UserCalPumpCurrentOffset[0];
		
		if ((sint32)USERCAL_stRAMCAL.au8UserCalPumpDeadband[0] < s32Temp)
		{
			s32Temp -= (uint32)USERCAL_stRAMCAL.au8UserCalPumpDeadband[0];
		}
		else
		{
			s32Temp = 0;
		}
	}
	else
	{
		s32Temp -= USERCAL_stRAMCAL.ai32UserCalPumpCurrentOffset[0];
		s32TempDelta = ~s32Temp;
		s32TempDelta += 1;
		
		if ((sint32)USERCAL_stRAMCAL.au8UserCalPumpDeadband[0] < s32TempDelta)
		{
			s32Temp += (uint32)USERCAL_stRAMCAL.au8UserCalPumpDeadband[0];
		}
		else
		{
			s32Temp = 0;
		}
	}			
	
	/* Multiply for factor correction */
	if (0 <= s32Temp)
	{
		LSU4X_atPumpCurrentMicroAmps[0] =
			s32Temp * (sint32)USERCAL_stRAMCAL.au32UserCalPumpCurrentPFactor[0];
	}
	else
	{
		LSU4X_atPumpCurrentMicroAmps[0] =
			s32Temp * (sint32)USERCAL_stRAMCAL.au32UserCalPumpCurrentMFactor[0];
	}
	
	/* Calculate normalised (sensor specific calibration) pump current */
	LSU4X_atPumpCurrentMicroAmpsNorm[0] =
	(LSU4X_atPumpCurrentMicroAmps[0] *
		USERCAL_stRAMCAL.au8UserCalPumpSensorTrim[0]) / 128;


	/* Calculate the current spread */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&LSU4X_atSpreadLSU4XIDX[0],
			NULL, NULL);	

	/* Lookup the current Lambda */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&LSU4X_atTableLSU4XIDX[0],
		NULL, NULL);		
}

void LSU4X_vTerminate(uint32* const pu32Arg )
{

}

void LSU4X_vCallBack(uint32* const pu32Arg )
{

}

void LSU4X_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	static uint32 u32TempLow[LSU_DEVICE_COUNT];
	static uint32 u32TempHigh[LSU_DEVICE_COUNT];
	static uint32 u32DCDL;
	uint32 u32TempDelta;
	

	switch (enEHIOResource)
	{
		case EH_VIO_IMPADC0:
		{
			LSU4X_atNernstSenseVoltsDC[0] = CONV_tADCToVolts(EH_VIO_IMPADC0, u32ADCResult);
			break;
		}
		case EH_I_ADD1:
		{	
			LSU4X_au8ACCallBackWait[0]++;		
			
			if (4 > LSU4X_au8ACCallBackWait[0])
			{
				LSU4X_au32ACSampleCount[0] = 0;
			}	
			else
			{	
				if (0xfff < u32ADCResult)
				{
					u32ADCResult = 0x80000000 - ((0xffff - u32ADCResult) << 8);
				}
				else
				{
					u32ADCResult = 0x80000000 + (u32ADCResult << 8);
				}	
				
				if (IOAPI_enHigh == LSU4X_enTriState)
				{
					u32TempLow[0] = u32ADCResult;						
				}
				else
				{				
					u32TempHigh[0] = u32ADCResult;				
				}		

				if (u32TempHigh[0] >= u32TempLow[0])
				{
					u32TempDelta = u32TempHigh[0] - u32TempLow[0];	
				}
				else
				{
					u32TempDelta = 0;
				}
				
				if (LSU4X_nACSamplesCount > LSU4X_au32ACSampleCount[0])
				{				
					LSU4X_aau32ACSamples[0][LSU4X_au32ACSampleCount[0]++] = u32TempDelta;
				}
			}
			break;
		}
		case EH_IO_GPSE10:
		{
			u32DCDL = u32ADCResult;
			break;
		}
		case EH_VIO_IMPADC1:
		{
			u32ADCResult = u32ADCResult - u32DCDL;
			LSU4X_ai32PumpCurrentADC[0] = (sint32)u32ADCResult;
			LSU4X_aas32DCSamples[0][LSU4X_au32DCSampleCount[0]++] = LSU4X_ai32PumpCurrentADC[0];
			
			if (LSU4X_nPumpCurrSamples <= LSU4X_au32DCSampleCount[0])/*CR1_45*/
			{
				LSU4X_ai32PumpCurrentADCFiltered[0] = 
					USERMATH_s32DiscardAndAverage32(&LSU4X_aas32DCSamples[0][0], LSU4X_au32DCSampleCount[0], 3u);
				LSU4X_au32DCSampleCount[0] = 0;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void LSU4X_vEnablePumpControl(void)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenTriState enTriState;		
	uint32 u32DeviceIDX;
	uint32 u32WarmUpDelayPumpMs;	
	uint32 u32WarmUpDelayMs = 1000u * (uint32)USERCAL_stRAMCAL.u8WarmUpDelay;

	u32WarmUpDelayMs = u32WarmUpDelayMs > LSU4X_nWarmMsMin ? u32WarmUpDelayMs : LSU4X_nWarmMsMin;
	u32WarmUpDelayMs = u32WarmUpDelayMs < LSU4X_nWarmMsMax ? u32WarmUpDelayMs : LSU4X_nWarmMsMax;	
	
	u32WarmUpDelayPumpMs = u32WarmUpDelayMs - LSU4X_nWarmPrePumpMs;
		
	for (u32DeviceIDX = 0; u32DeviceIDX < LSU_DEVICE_COUNT; u32DeviceIDX++)
	{
		enEHIOResource = (0 == u32DeviceIDX) ? LSU4X_nIENResource : LSU4X_nIENResource;
		
		if ((USERCAL_stRAMCAL.userCalRNernstReady > LSU4X_atNernstSenseOhms[u32DeviceIDX]) ||
				(USERCAL_stRAMCAL.userCalRHeaterThreshold < LSUH_atHeaterOhms[u32DeviceIDX]))
		{
			LSU4X_au32SensorReadyCount[u32DeviceIDX] = (u32WarmUpDelayMs > LSU4X_au32SensorReadyCount[u32DeviceIDX]) ?
			LSU4X_au32SensorReadyCount[u32DeviceIDX] + 1 : LSU4X_au32SensorReadyCount[u32DeviceIDX];
		}
		
		/* Condition sensor ready flags for display and closed loop heater control */
		LSUH_aboSensorReady[u32DeviceIDX] = (u32WarmUpDelayMs <= LSU4X_au32SensorReadyCount[u32DeviceIDX]);				
		
		/* Switch on the pump current before the sensor ready to obtain nernst impedence */
		enTriState = (u32WarmUpDelayPumpMs <= LSU4X_au32SensorReadyCount[u32DeviceIDX]) ? IOAPI_enLow : IOAPI_enHigh;		
		
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);	
	}
}

void LSU4X_vThread(void)
{
	while(1);	
}

#endif //BUILD_USER



