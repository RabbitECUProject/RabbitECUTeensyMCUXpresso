/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Electronic Spark Timing Header File                    */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for electronic spark timing    */
/*                                                                            */
/* FILE NAME:          EST.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef EST_H
#define EST_H

#include <string.h>
#include "IOAPI.h"
#include "TEPMAPI.h"
#include "USER.h"
#include "cpuabstract.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _EST_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

typedef enum
{
	EST_nIgnitionPrimary,
	EST_nIgnitionStage1,
	EST_nIgnitionStage2,
	EST_nIgnitionStage3,
} EST_tenIgnitionTimingStage;

typedef enum
{
	EST_nIgnitionReqPrimary,
	EST_nIgnitionReqDSGStage1,
	EST_nIgnitionReqDSGStage2,
	EST_nIgnitionReqDSGCutsStage3,
	EST_nIgnitionReqStage3,
} EST_tenIgnitionTimingRequest;

#ifdef BUILD_SPARKDOG_PF
#define EST_nMotor1EnablePin          EH_IO_GP4
#define EST_nMotor2EnablePin          EH_IO_GP4
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
#define EST_nMotor1EnablePin          EH_IO_GP3
#define EST_nMotor2EnablePin          EH_IO_GP2
#endif

#ifdef BUILD_SPARKDOG_MKS20
#define EST_nMotor1EnablePin          EH_IO_GP7
#define EST_nMotor2EnablePin          EH_IO_GP6
#define EST_nExportPhasePin           EH_IO_GPSE9
#endif //BUILD_SPARKDOG_MKS20

#define EST_nSlowFTMDivisor           TEPMAPI_enDiv128
#define EST_nDwellOffMinUs            1500u
#define EST_nDegreesPerCycle          360u
#define EST_xUsToSlowTicks(x)          (((SENSORS_nSlowFTMFreq / 100u) * (x))	/ 10000u)

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/

EXTERN TEPMAPI_ttEventTime EST_tStartFractionD;
EXTERN TEPMAPI_ttEventTime EST_tStartFractionC;
EXTERN TEPMAPI_ttEventTime EST_tStartFractionB;
EXTERN TEPMAPI_ttEventTime EST_tStartFractionA;	
//ASAM mode=readvalue name="EST Start Fraction" type=uint16 offset=0 min=0 max=1 m=0.00001526 b=0 units="dl" format=6.0 help="EST Dwell Start Fraction"
EXTERN TEPMAPI_ttEventTime EST_tEndFractionD;
EXTERN TEPMAPI_ttEventTime EST_tEndFractionC;
EXTERN TEPMAPI_ttEventTime EST_tEndFractionB;
EXTERN TEPMAPI_ttEventTime EST_tEndFractionA;
EXTERN TEPMAPI_ttEventTime EST_tDwellUs;	
//ASAM mode=readvalue name="EST Dwell Fraction" type=uint16 offset=0 min=0 max=1 m=0.00001526 b=0 units="dl" format=6.0 help="EST Dwell Fraction" 
EXTERN GPM6_ttMTheta EST_tIgnitionAdvanceMtheta;
EXTERN sint32 EST_tIgnitionAdvanceMthetaReport;
//ASAM mode=readvalue name="EST Ignition Advance" type=sint32 offset=0 min=-50 max=50 m=0.001 b=0 units="degrees" format=5.3 help="EST Ignition Advance"

EXTERN EST_tenIgnitionTimingRequest EST_enIgnitionTimingRequest;
//ASAM mode=readvalue name="EST Timing Request Mode" type=uint16 offset=0 min=0 max=10 units="ENUMERATION PRIMARY=0 STAGE1=1 STAGE2=2 STAGE3=3 STAGE4=4" format=1.0 help="EST Dwell Start Fraction"

EXTERN uint16 EST_u16KnockSensorThresholdApply;
//ASAM mode=readvalue name="EST Applied Knock Threshold" type=uint16 offset=0 min=0 max=127 m=1 b=0 units="dl" format=3.0 help="EST applied Knock Threshold"

EXTERN sint16 EST_s16KnockTimingTrim;
//ASAM mode=readvalue name="EST Applied Knock Trim" type=sint16 offset=0 min=-30 max=0 m=0.1 b=0 units="degrees" format=3.1 help="EST applied Knock Trim"


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void EST_vStart(uint32 * const pu32Arg);
void EST_vRun(uint32* const pu32Arg);
void EST_vTerminate(uint32* const pu32Arg);
void EST_vCallBack(puint32 const pu32Arg);


#endif // EST_H

