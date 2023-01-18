/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      LSUH                                                   */
/* DESCRIPTION:        This code module initialises the required ADC, DAC     */
/*                     CTRL, spread, table, DIO and TEPM resources for        */
/*                     managing the heater control for the LSU4.X sensor/s    */
/*                                                                            */
/* FILE NAME:          LSUH.c                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _LSUH_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "LSUH.h"
#include "USERMATH.h"

/* LOCAL MACRO DEFINITIONS ****************************************************/
#define LSUH_nADConfigCount (sizeof(LSUH_rastADConfig) / sizeof(LSUH_tstADConfig))

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
#ifdef LSUH_TEPM
TEPMAPI_tstTimedEvent LSUH_aastTimedEvents[LSU_DEVICE_COUNT][TEPMAPI_nEventsMax];
#endif //LSUH_TEPM
uint32 LSUH_au32TEPMTimeout[LSU_DEVICE_COUNT];
uint32 LSUH_u32HeaterDutyMax;
uint32 LSUH_au32HeaterLowADCOnRaw[LSU_DEVICE_COUNT];
uint32 LSUH_au32HeaterLowADCOffRaw[LSU_DEVICE_COUNT];
uint32 LSUH_au32HeaterLowADCDeltaFiltered[LSU_DEVICE_COUNT];
GPM6_ttVolts LSUH_atHeaterSenseVolts[LSU_DEVICE_COUNT];/*CR1_54*/
GPM6_ttAmps LSUH_atHeaterAmps[LSU_DEVICE_COUNT];/*CR1_54*/
GPM6_ttMilliVolts LSUH_tHeaterEffMax;/*CR1_54*/
GPM6_ttMilliWatts LSUH_atHeaterPower[LSU_DEVICE_COUNT];
bool LSUH_aboADCQueuePending[LSU_DEVICE_COUNT];
CTRLAPI_ttPIDIDX LSUH_atPIDLSUH1IDX[LSU_DEVICE_COUNT];
sint32 LSUH_ai32PIDTarget[LSU_DEVICE_COUNT];
sint32 LSUH_ai32PIDFeedback[LSU_DEVICE_COUNT];
sint32 LSUH_ai32PIDOutput[LSU_DEVICE_COUNT];
uint32 LSUH_au32ACSampleCounter[LSU_DEVICE_COUNT];
bool LSUH_aboNernstRNewValue[LSU_DEVICE_COUNT];
uint32 LSUH_au32NoNewNernstValueCount[LSU_DEVICE_COUNT];
bool LSUH_aboNernstTempMode[LSU_DEVICE_COUNT];


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : LSUH_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void LSUH_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);/*CR1_53*/

/*******************************************************************************
* Interface        : LSUH_vInitiateTEPMQueue
*
* Implementation   : Function to initiate a new TEPM queue timed output
*
* Parameter
*    Par1          : enEHIOResource enum of the timer resource
*    Par2          : tEventTime the timer value at the last timer event of interest
*
* Return Value     : NIL
*******************************************************************************/
#ifdef LSUH_TEPM
static void LSUH_vInitiateTEPMQueue(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);
#endif //LSUH_TEPM

/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/
const LSUH_tstADConfig LSUH_rastADConfig[] = LSUH_nADConfig;


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void LSUH_vStart(uint32 * const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	CTRLAPI_tstPIDCB stPIDCB;
	CTRLAPI_tenCTRLType enCTRLType;	

	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;
	uint32 u32ADConfigIDX;
	bool boInitFailed = FALSE;
	
#ifdef BUILD_SPARKDOG_PF
	/* not yet tested */
	return;
#endif

	LSUH_tHeaterEffMax = LSUH_nHeaterEffInit;
	LSUH_au32ACSampleCounter[0] = 0;
	
#ifdef LSUH_TEPM
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;

	/* Request and initialise FTM for EH_IO_TMR15 and EH_IO_TMR16 */
	enEHIOResource = EH_VIO_TPM0;	
	enEHIOType = IOAPI_enTEPM;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TPM0;
		stTEPMResourceCB.enPreScalar = TEPMAPI_enDiv32;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;		
					
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);	
	}
	
	/* Request and initialise EH_IO_TMR15 */
	enEHIOResource = EH_IO_TMR15;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
		
	/* Initialise the TEPM channel EH_IO_TMR15 */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enToggle;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}		
	
	/* Request and initialise EH_IO_TMR16 */
	enEHIOResource = EH_IO_TMR16;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
	/* Initialise the TEPM channel EH_IO_TMR16 */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enToggle;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}
#endif //LSUH_TEPM
	
	/* Request and initialise AD converter resources */
	for (u32ADConfigIDX = 0; u32ADConfigIDX < LSUH_nADConfigCount; u32ADConfigIDX++)
	{
		enEHIOResource = LSUH_rastADConfig[u32ADConfigIDX].enEHIOResource;
		enEHIOType = LSUH_rastADConfig[u32ADConfigIDX].enEHIOType;
		stADCCB.enSamplesAv = LSUH_rastADConfig[u32ADConfigIDX].enSamplesAv;
		stADCCB.pfResultCB = LSUH_rastADConfig[u32ADConfigIDX].pfResultCB;
		stADCCB.enTrigger = LSUH_rastADConfig[u32ADConfigIDX].enTrigger;				
			
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
	
	/* Initialise HMEN enable control lines */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		enEHIOResource = LSUH_nHMENResource;
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

	/* Request and initialise the LSUH1 PID controller */
	stPIDCB.enPIDRate = CTRLAPI_enPIDCall;
	stPIDCB.pi32Target = &LSUH_ai32PIDTarget[0];
	stPIDCB.pi32Feedback = &LSUH_ai32PIDFeedback[0];	
	stPIDCB.pi32Output = &LSUH_ai32PIDOutput[0];	
	stPIDCB.u8PTerm = 0xFF;
	stPIDCB.u8ITerm = 0x03;
	stPIDCB.u8DTerm = 0x08;	
	stPIDCB.boNoWindUp = TRUE;	
	stPIDCB.boReset = FALSE;	
	stPIDCB.boResetIntegrator = FALSE;		
	stPIDCB.boRun = TRUE;
	stPIDCB.i32OutputMin = -2500000;	
	stPIDCB.i32OutputMax = 2500000;		
	enCTRLType = CTRLAPI_enPID;
	
	USER_vSVC(SYSAPI_enInitialiseCTRLResource, (void*)&enCTRLType, (void*)&stPIDCB, NULL);	
	
	LSUH_atPIDLSUH1IDX[0] = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(CTRLAPI_ttPIDIDX)pstSVCDataStruct->tClientHandle : -1;		
	
	*pu32Arg = ((SYSAPI_enOK == pstSVCDataStruct->enSVCResult) && !boInitFailed)	?
			*pu32Arg | SYSAPI_CALL_MASK_FAIL_RES_INIT : 
			*pu32Arg & ~SYSAPI_CALL_MASK_FAIL_RES_INIT;

			
	/* Initialise variables */
	LSUH_au32TEPMTimeout[0] = ~0;
	LSUH_au32HeaterDuty[0] = 80 * LSU_DUTY_RES;
	LSUH_ai32PIDTarget[0] = 300000u;
	LSUH_au32ACADCDeltaFiltered[0] = 0;
	LSUH_au32HeaterLowADCDeltaFiltered[0] = 0;
	LSUH_aboSensorReady[0] = FALSE;	
	LSU4X_atNernstSenseOhms[0] = 1000000u;
}

void LSUH_vRun(uint32* const pu32Arg )
{
	uint32 u32Temp;
	uint32 u32MaxDuty;
	CTRLAPI_tenCTRLType enCTRLType;	
	ADCAPI_tenTrigger enTrigger = ADCAPI_enTrigger3;	
	static uint32 u32RunCount = 0;/*CR1_56*/;	
	static uint32 u32TempControllerRunCount;
	
#ifdef BUILD_SPARKDOG_PF
	/* not yet tested */
	return;
#endif

	if (0 == (u32RunCount++ % (uint32)LSUH_nCallsIn100Ms))/*CR1_57*/
	{
		LSUH_tHeaterEffMax = (LSUH_nHeffVLimit > LSUH_tHeaterEffMax) ?
						LSUH_tHeaterEffMax + (LSUH_nHeffVRampRate / LSUH_nRateHZ) : LSUH_tHeaterEffMax;	/*CR1_57*/
	}
	

#ifdef LSUH_TEPM
	enEHIOResourceTimer = (0 == u32LSUDeviceIDX ) ? EH_IO_TMR15 : EH_IO_TMR16;
	enEHIOResourceADC = (0 == u32LSUDeviceIDX ) ? EH_IO_ADSE8 : EH_IO_ADSE9;				
	
	/* Catch a lost TEPM for heater */
	if (10000 < LSUH_au32TEPMTimeout[u32LSUDeviceIDX]++)
	{
		LSUH_vInitiateTEPMQueue(enEHIOResourceTimer, 0u);
	}
#endif //LSUH_TEPM
		
	/* ADC pending flags are expected to be syncronised... */
	if (TRUE == LSUH_aboADCQueuePending[0])
	{
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, NULL, NULL);
		LSUH_aboADCQueuePending[0] = FALSE;
	}	
	
	/* Iterate the heater PID controller if new R value */
	if (TRUE == LSUH_aboNernstRNewValue[0])
	{
		enCTRLType = CTRLAPI_enPID;					
		
		USER_vSVC(SYSAPI_enIterateCTRLResource, (void*)&enCTRLType, (void*)&LSUH_atPIDLSUH1IDX[0], NULL);
		LSUH_aboNernstRNewValue[0] = FALSE;

		LSUH_au32NoNewNernstValueCount[0] =
		    200 < LSUH_au32NoNewNernstValueCount[0] ?
				LSUH_au32NoNewNernstValueCount[0] - 200 : 0;

		LSUH_aboNernstTempMode[0] = 0 == LSUH_au32NoNewNernstValueCount[0] ? TRUE :
				LSUH_aboNernstTempMode[0];

		u32TempControllerRunCount = 0;
	}
	else
	{
		LSUH_au32NoNewNernstValueCount[0] =
		    1000 > LSUH_au32NoNewNernstValueCount[0] ?
				LSUH_au32NoNewNernstValueCount[0] + 1 : LSUH_au32NoNewNernstValueCount[0];

		LSUH_aboNernstTempMode[0] = 999 < LSUH_au32NoNewNernstValueCount[0] ? FALSE :
				LSUH_aboNernstTempMode[0];
	}

	/* Run the alternate heater resistance based controller if required */
	if ((FALSE == LSUH_aboNernstTempMode[0]) && (0 == (++u32TempControllerRunCount % 40)))
	{
		u32Temp = LSUH_atHeaterOhms[0];
		u32Temp = 14400 > u32Temp ? u32Temp : 14400;
		LSU4X_atNernstSenseOhms[0] = (14400 - u32Temp) * 42;

		enCTRLType = CTRLAPI_enPID;
		USER_vSVC(SYSAPI_enIterateCTRLResource, (void*)&enCTRLType, (void*)&LSUH_atPIDLSUH1IDX[0], NULL);
	}
	


	LSUH_atHeaterSenseVolts[0] = CONV_tADCToVolts(
			EH_IO_GPSE11, LSUH_au32HeaterLowADCDeltaFiltered[0]);
	
	LSUH_atHeaterAmps[0] = CONV_tOhmsVoltsToAmps(LSUH_nHeaterSenseOhms, LSUH_atHeaterSenseVolts[0]);
	LSUH_atHeaterOhms[0] = CONV_tVoltsAmpsToOhms(BVM_tBattVolts - LSUH_atHeaterSenseVolts[0] - 800u,
        LSUH_atHeaterAmps[0]);
	
	LSUH_ai32PIDFeedback[0] = LSU4X_atNernstSenseOhms[0];
	LSUH_atHeaterPower[0] = (10000000 - (3 * LSUH_ai32PIDOutput[0])) / 1000;
	
	/* Calculate the maximum duty cycle sustained by the current sense resistors */
	u32MaxDuty = (1000000u * (uint32)LSUH_nMaxDutyDenom) / LSUH_atHeaterAmps[0];
	u32MaxDuty /= LSUH_atHeaterAmps[0];
	
	/* Calculated the duty cycle to deliver requested power (n = Preq / VI))*/
	u32Temp = LSUH_atHeaterPower[0] * 1000u;
	u32Temp /= (BVM_tBattVolts - LSUH_atHeaterSenseVolts[0] - 800u);
	u32Temp *= 1000;
	u32Temp /= LSUH_atHeaterAmps[0];
	u32Temp = (u32MaxDuty > u32Temp) ? u32Temp : u32MaxDuty;		
	u32Temp = (LSUH_nMaxDuty > u32Temp) ? u32Temp : LSUH_nMaxDuty;
	u32Temp = (LSUH_nMinDuty < u32Temp) ? u32Temp : LSUH_nMinDuty;	
	LSUH_au32HeaterDuty[0] = u32Temp;
	
	/* Max duty cycle until sensor ready */
	LSUH_au32HeaterDuty[0] = (FALSE == LSUH_aboSensorReady[0]) ? LSUH_nMaxDuty : LSUH_au32HeaterDuty[0];
	
	/* Limit to max heater power during startup */
	u32Temp = LSUH_tHeaterEffMax * LSUH_tHeaterEffMax;
	u32Temp /= (BVM_tBattVolts - LSUH_atHeaterSenseVolts[0] - 800u);
	u32Temp *= 1000u;
	u32Temp /= (BVM_tBattVolts - LSUH_atHeaterSenseVolts[0] - 800u);
	
	LSUH_u32HeaterDutyMax = u32Temp;
	LSUH_au32HeaterDuty[0] = MIN(LSUH_au32HeaterDuty[0], LSUH_u32HeaterDutyMax);
}

void LSUH_vTerminate(uint32* const pu32Arg )
{

}

void LSUH_vCallBack(uint32* const pu32Arg )
{

}

static void LSUH_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	uint16 u16ADCDelta;
	uint32 u32Temp;/*CR1_65*/

	if (TRUE == LSUH_aboHeaterIsOn[0])
	{
		LSUH_au32HeaterLowADCOnRaw[0] = u32ADCResult;
		u32Temp = (LSUH_au32HeaterLowADCOnRaw[0] > LSUH_au32HeaterLowADCOffRaw[0]) ?
		LSUH_au32HeaterLowADCOnRaw[0] - LSUH_au32HeaterLowADCOffRaw[0] : 0;
		
		u32Temp = MIN(u32Temp, 0xffff);/*CR1_65*/
		u16ADCDelta = (uint16)u32Temp;/*CR1_65*/
		
		(void)USERMATH_u16SinglePoleLowPassFilter16(u16ADCDelta, LSUH_nHeatCurrFilt,
					&LSUH_au32HeaterLowADCDeltaFiltered[0]);
	}
	else
	{
		LSUH_au32HeaterLowADCOffRaw[0] = 4;
	}
}

void LSUH_vTurnHeaterOff(void)
{
	uint32 u32SampleCount;
	uint32 u32SampleDiscard;
	puint32 pu32SampleArray;
	uint32 u32SampleIDX;
	bool boSamplesOK = TRUE;
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenTriState enTriState;

	enEHIOResource = LSUH_nHMENResource;
	enTriState = IOAPI_enLow;
	USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource, (void*)&enTriState, (void*)NULL);

	pu32SampleArray = (puint32)(&LSU4X_aau32ACSamples[0][0]);

	/* Limit the amount of samples - use the last 8 samples */
	if (8 < LSU4X_au32ACSampleCount[0])
	{
		pu32SampleArray += (LSU4X_au32ACSampleCount[0] - 8);
		LSU4X_au32ACSampleCount[0] = 8;
	}		

	u32SampleCount = LSU4X_au32ACSampleCount[0];
	u32SampleCount = MAX(u32SampleCount, 2u);
	u32SampleDiscard = (u32SampleCount - 1) >> 1;

	LSUH_aboADCQueuePending[0] = TRUE;
	LSUH_aboHeaterIsOn[0] = FALSE;
	LSU4X_au8ACCallBackWait[0] = 0;
	LSUH_au32ACADCDelta[0] = USERMATH_u32DiscardAndAverage32(pu32SampleArray, u32SampleCount, u32SampleDiscard);

	if ((LSUH_nHeaterOhmsSamples - 1) > LSUH_au32ACSampleCounter[0])
	{
		LSUH_aau32ADADCSamples[0][LSUH_au32ACSampleCounter[0]] = LSUH_au32ACADCDelta[0];
		LSUH_au32ACSampleCounter[0]++;
	}
	else
	{
		u32SampleCount = LSUH_nHeaterOhmsSamples;
		u32SampleDiscard = u32SampleCount >> 1;
		LSUH_aau32ADADCSamples[0][LSUH_au32ACSampleCounter[0]] = LSUH_au32ACADCDelta[0];
		LSUH_au32ACADCDeltaFiltered[0] = USERMATH_u32DiscardAndAverage32(&LSUH_aau32ADADCSamples[0][0], u32SampleCount, u32SampleDiscard);

		for (u32SampleIDX = 0; u32SampleIDX < LSUH_nHeaterOhmsSamples; u32SampleIDX++)
		{
			if ((LSUH_nMinACSample > LSUH_aau32ADADCSamples[0][u32SampleIDX]) ||
					(LSUH_nMaxACSample < LSUH_aau32ADADCSamples[0][u32SampleIDX]))
			{
				boSamplesOK	= FALSE;
			}
		}

		if (TRUE == boSamplesOK)
		{
			LSU4X_atNernstSenseOhms[0] = CONV_tADCToOhms(EH_I_ADD1, LSUH_au32ACADCDeltaFiltered[0]);
			LSUH_aboNernstRNewValue[0] = true;
		}

		LSUH_au32ACSampleCounter[0] = 0;
	}
}

void LSUH_vTurnHeaterOn(void)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenTriState enTriState;

	enEHIOResource = LSUH_nHMENResource;
	enTriState = IOAPI_enHigh;
	USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource, (void*)&enTriState, (void*)NULL);

	LSUH_aboADCQueuePending[0] = TRUE;
	LSUH_aboHeaterIsOn[0] = TRUE;
}

#ifdef LSUH_TEPM
	IOAPI_tenEHIOType;
	enEHIOType = IOAPI_enDIOOutput;

void LSUH_vTEPMCallBackLow(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint32 u32SampleCount;
	uint32 u32SampleDiscard;
	puint32 pu32SampleArray;
	uint32 u32LSUDeviceIDX;
	uint32 u32SampleIDX;
	bool boSamplesOK = TRUE;
	IOAPI_tenEHIOResource enEHIOResourceAD;	
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR15:
		{
			u32LSUDeviceIDX = 0;
			enEHIOResourceAD = EH_I_ADD5;
			break;
		}
		case EH_IO_TMR16:
		{
			u32LSUDeviceIDX = 1;
			enEHIOResourceAD = EH_I_ADD7;			
			break;	
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;
		
	pu32SampleArray = (puint32)(&LSU4X_aau32ACSamples[u32LSUDeviceIDX][0]);
	
	/* Limit the amount of samples - use the last 8 samples */
	if (8 < LSU4X_au32ACSampleCount[u32LSUDeviceIDX])
	{
		pu32SampleArray += (LSU4X_au32ACSampleCount[u32LSUDeviceIDX] - 8);
		LSU4X_au32ACSampleCount[u32LSUDeviceIDX] = 8;		
	}		
		
	u32SampleCount = LSU4X_au32ACSampleCount[u32LSUDeviceIDX];
	u32SampleCount = MAX(u32SampleCount, 2u);
	u32SampleDiscard = (u32SampleCount - 1) >> 1;
	
	LSUH_aboADCQueuePending[u32LSUDeviceIDX] = TRUE;
	LSUH_aboHeaterIsOn[u32LSUDeviceIDX] = FALSE;
	LSU4X_au8ACCallBackWait[u32LSUDeviceIDX] = 0;	
	LSUH_au32ACADCDelta[u32LSUDeviceIDX] = USERMATH_u32DiscardAndAverage32(pu32SampleArray, u32SampleCount, u32SampleDiscard);
	
	if ((LSUH_nHeaterOhmsSamples - 1) > LSUH_au32ACSampleCounter[u32LSUDeviceIDX])
	{
		LSUH_aau32ADADCSamples[u32LSUDeviceIDX][LSUH_au32ACSampleCounter[u32LSUDeviceIDX]] = LSUH_au32ACADCDelta[u32LSUDeviceIDX];
		LSUH_au32ACSampleCounter[u32LSUDeviceIDX]++;		
	}
	else
	{		
		u32SampleCount = LSUH_nHeaterOhmsSamples;
		u32SampleDiscard = u32SampleCount >> 1;		
		LSUH_aau32ADADCSamples[u32LSUDeviceIDX][LSUH_au32ACSampleCounter[u32LSUDeviceIDX]] = LSUH_au32ACADCDelta[u32LSUDeviceIDX];		
		LSUH_au32ACADCDeltaFiltered[u32LSUDeviceIDX] = USERMATH_u32DiscardAndAverage32(&LSUH_aau32ADADCSamples[u32LSUDeviceIDX][0], u32SampleCount, u32SampleDiscard);
		
		for (u32SampleIDX = 0; u32SampleIDX < LSUH_nHeaterOhmsSamples; u32SampleIDX++)
		{
			if ((LSUH_nMinACSample > LSUH_aau32ADADCSamples[u32LSUDeviceIDX][u32SampleIDX]) ||
					(LSUH_nMaxACSample < LSUH_aau32ADADCSamples[u32LSUDeviceIDX][u32SampleIDX]))
			{
				boSamplesOK	= FALSE;
			}
		}
		
		if (TRUE == boSamplesOK)
		{
			LSU4X_atNernstSenseOhms[u32LSUDeviceIDX] = CONV_tADCToOhms(enEHIOResourceAD, LSUH_au32ACADCDeltaFiltered[u32LSUDeviceIDX]);	
			LSUH_aboNernstRNewValue[u32LSUDeviceIDX] = true;
		}
		
		LSUH_au32ACSampleCounter[u32LSUDeviceIDX] = 0;		
	}
}

void LSUH_vTEPMCallBackHigh(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint32 u32LSUDeviceIDX;
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR15:
		{
			u32LSUDeviceIDX = 0;
			break;
		}
		case EH_IO_TMR16:
		{
			u32LSUDeviceIDX = 1;
			break;	
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;	
	
	LSUH_aboADCQueuePending[u32LSUDeviceIDX] = TRUE;	
	LSUH_aboHeaterIsOn[u32LSUDeviceIDX] = TRUE;	
	LSUH_vInitiateTEPMQueue(enEHIOResource, tEventTime);	
}

static void LSUH_vInitiateTEPMQueue(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	TEPMAPI_ttEventCount tEventCount;
	uint16 u16LongPhase;
	uint16 u16ShortPhase;
	sint16 s16PLLTrim;
	uint32 u32LSUDeviceIDX;
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR15:
		{
			u32LSUDeviceIDX = 0;
			break;
		}
		case EH_IO_TMR16:
		{
			u32LSUDeviceIDX = 1;
			break;	
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;	
	
	/* Calculate the phase error between AC waveform and heater duty start */
	u16LongPhase = (uint16)tEventTime - LSU4X_u16PWMLastStart;
	u16ShortPhase = u16LongPhase % (LSU4X_u16PWMPeriod / 10);
	
	/* Calculate the required PLL trim */
	s16PLLTrim = (LSU4X_u16PWMPeriod / 20) < u16ShortPhase ?  
				(sint16)(u16ShortPhase - (LSU4X_u16PWMPeriod / 10)) :
				(sint16)u16ShortPhase;
	
	s16PLLTrim /= 4;	
	tEventCount = 2;
	
	LSUH_aastTimedEvents[u32LSUDeviceIDX][0].enAction = TEPMAPI_enSetLow;
	LSUH_aastTimedEvents[u32LSUDeviceIDX][0].enMethod = TEPMAPI_enHardLinkedTimeStep;
	LSUH_aastTimedEvents[u32LSUDeviceIDX][0].tEventTime = 
		((uint16)(LSU4X_u16PWMPeriod - s16PLLTrim) * LSUH_au32HeaterDuty[u32LSUDeviceIDX]) / (100ul * LSU_DUTY_RES);
	LSUH_aastTimedEvents[u32LSUDeviceIDX][0].pfEventCB = &LSUH_vTEPMCallBackLow;		

	LSUH_aastTimedEvents[u32LSUDeviceIDX][1].enAction = TEPMAPI_enSetHigh;
	LSUH_aastTimedEvents[u32LSUDeviceIDX][1].enMethod = TEPMAPI_enHardLinkedTimeStep;
	LSUH_aastTimedEvents[u32LSUDeviceIDX][1].tEventTime = 
		((uint16)(LSU4X_u16PWMPeriod - s16PLLTrim) * (100ul * LSU_DUTY_RES - LSUH_au32HeaterDuty[u32LSUDeviceIDX]) / (100ul * LSU_DUTY_RES));
	LSUH_aastTimedEvents[u32LSUDeviceIDX][1].pfEventCB = &LSUH_vTEPMCallBackHigh;
	
	USER_vSVC(SYSAPI_enAppendTEPMQueue, (void*)&enEHIOResource, 
		(void*)&LSUH_aastTimedEvents[u32LSUDeviceIDX][0], (void*)&tEventCount);
	
	LSUH_au32TEPMTimeout[u32LSUDeviceIDX] = 0;
}
#endif //LSUH_TEPM


#endif //BUILD_USER




