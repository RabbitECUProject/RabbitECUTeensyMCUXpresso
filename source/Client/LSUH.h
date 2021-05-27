/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      LSUH Header File                                       */
/* DESCRIPTION:        This code module initialises the required ADC, DAC     */
/*                     CTRL, spread, table, DIO and TEPM resources for        */
/*                     managing the heater control for the LSU4.X sensor/s    */
/*                                                                            */
/* FILE NAME:          LSUH.h                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef LSUH_H
#define LSUH_H

#include "ADCAPI.h"
#include "CONV.h"
#include "CTRLAPI.h"
#include "declarations.h"
#include "IOAPI.h"
#include "LSU4X.h"
#include "TEPMAPI.h"
#include "UNITS.h"
#include "USER.h"
#include "USERMATH.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define LSUH_nADConfig																																												\
{																																																							\
	{EH_IO_UART1_CTS, IOAPI_enADSE, ADCAPI_en4Samples, ADCAPI_enDiffGain1, &LSUH_vADCCallBack, ADCAPI_enTrigger3},	\
}

#define LSU_DUTY_RES                10u
#define LSUH_nRateHZ                1000u
#define LSUH_nHeaterEffInit			7000u				/* 7 volts effective heater at startup */
#define LSUH_nHeaterSenseOhms		180u
#define LSUH_nHeaterOhmsSamples     3u
#define LSUH_nMinACSample           10000u
#define LSUH_nMaxACSample           1600000u
#define LSUH_nCurrSenseOhms			0.18
#define	LSUH_nCurrSensePower		0.5
#define LSUH_nMaxDutyDenom			((1000 * LSUH_nCurrSensePower) / LSUH_nCurrSenseOhms)
#define LSUH_nMinDuty               200u
#define LSUH_nMaxDuty               900u
#define LSUH_nCallsIn100Ms			(0.1 * LSUH_nRateHZ)
#define	LSUH_nHeffVLimit            13000u
#define LSUH_nHeffVRampRate			40000u
#define LSUH_nTEPMTimeOut           (10 * LSUH_nRateHZ)
#define LSUH_nHeatCurrFilt			16u

#define LSUH_nHMENResource          EH_IO_GP1
#define LSUH_nHMFResource           EH_IO_UART1_CTS

#undef EXTERN
#ifdef _LSUH_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN uint32 LSUH_au32HeaterDuty[LSU_DEVICE_COUNT];
//ASAMREMOVED!! mode=readvalue name="Heater Duty 1" type=uint32 offset=0 min=0 max=100 m=0.1 b=0 units="%" format=4.1 help="Heater Left Duty Cycle"

/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to hold an AD resource configuration
*******************************************************************************/
typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tenSamplesAv enSamplesAv;
	ADCAPI_tenDiffGain enDiffGain;	
	ADCAPI_tpfResultCB pfResultCB;	
	ADCAPI_tenTrigger enTrigger;
} LSUH_tstADConfig;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN bool LSUH_aboHeaterIsOn[LSU_DEVICE_COUNT];
EXTERN bool LSUH_aboSensorReady[LSU_DEVICE_COUNT];
EXTERN uint32 LSUH_au32ACADCDeltaFiltered[LSU_DEVICE_COUNT];
EXTERN uint32 LSUH_aau32ADADCSamples[LSU_DEVICE_COUNT][LSUH_nHeaterOhmsSamples];
EXTERN uint32 LSUH_au32ACADCDelta[LSU_DEVICE_COUNT];
EXTERN GPM6_ttOhms LSUH_atHeaterOhms[LSU_DEVICE_COUNT];
//ASAMREMOVED!! mode=readvalue name="Heater Ohms 1" type=uint32 offset=0 min=0 max=10 m=0.001 b=0 units="Ohms" format=5.3 help="Heater 1 Ohms"


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void LSUH_vStart(uint32 * const pu32Arg);
void LSUH_vRun(uint32* const pu32Arg);
void LSUH_vTerminate(uint32* const pu32Arg);
void LSUH_vCallBack(uint32* const pu32Arg);
void LSUH_vTurnHeaterOff(void);
void LSUH_vTurnHeaterOn(void);

#endif // LSUH_H

