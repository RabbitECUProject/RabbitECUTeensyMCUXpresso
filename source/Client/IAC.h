/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Idle Air Control Header File                           */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for idle air control           */
/*                                                                            */
/* FILE NAME:          IAC.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IAC_H
#define IAC_H

#include <string.h>
#include "CTS.h"
#include "TPS.h"
#include "IOAPI.h"
#include "SETUP.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef _IAC_C
#define EXTERN
#else
#define EXTERN extern
#endif

#define IAC_LEARN_COUNTS 400

/* GLOBAL TYPE DEFINITIONS ****************************************************/

typedef enum
{
	IAC_enResetHome,
	IAC_enNormal
} IAC_tenStepperState;

typedef enum
{
	IAC_enOpenLoop,
	IAC_enClosedLoop
} IAC_tenControlState;

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 IAC_u32ISCDuty;
//ASAM mode=readvalue name="IAC Duty" type=uint32 offset=0 min=0 max=100 m=0.024 b=0 units="%" format=4.1 help="IAC Duty Position"
EXTERN IAC_tenControlState IAC_enControlState;
//ASAM mode=readvalue name="IAC Control State" type=uint16 offset=0 min=0 max=100 units="ENUMERATION OPEN=0 CLOSED=1" format=1.0 help="IAC Control State"
EXTERN bool IAC_boOverrunCutRPMEnable;
EXTERN sint32 IAC_s32ISCESTTrim[2];
EXTERN uint8 IAC_u8SlaveTarget;
//ASAM mode=readvalue name="IAC Slave Target" type=uint8 offset=0 min=0 max=255 m=1 b=-16 units="dl" format=3.0 help="Slave IAC Position"
EXTERN uint16 IAC_u16ISCTargetRamp;
//ASAM mode=readvalue name="IAC Target RPM" type=uint16 offset=0 min=0 max=4095 m=1 b=0 units="RPM" format=4.0 help="IAC Current Target RPM"

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void IAC_vStart(uint32 * const pu32Arg);
void IAC_vRun(uint32* const pu32Arg);
void IAC_vTerminate(uint32* const pu32Arg);
void IAC_vCallBack(puint32 const pu32Arg);

#endif // MAP_H

