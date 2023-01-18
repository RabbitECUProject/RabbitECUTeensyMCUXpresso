/******************************************************************************/
/*    Copyright (c) 2020 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Torque Control Module Header File                      */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for torque control             */
/*                                                                            */
/* FILE NAME:          TORQUE.c                                               */
/* REVISION HISTORY:   01-04-2020 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TORQUE_H
#define TORQUE_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"
#include "USERCAL.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/


#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _MAP_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define TORQUE_nPPSRange 25400u

#define TORQUE_MIN_FCUTS_MAP 30000u
#define TORQUE_MAX_FCUTS_MAP 180000u
#define TORQUE_MIN_FCUTS_RPM 500u

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 TORQUE_u32ATXTorqueLimit;
EXTERN uint32 TORQUE_u32OutputTorqueModified;
EXTERN uint32 TORQUE_u32OutputTorqueEstimate;
//ASAM mode=readvalue name="Output Torque Estimate" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=3.1 help="Torque Estimate"

EXTERN uint32 TORQUE_u32QuickCutPercent;
EXTERN uint32 TORQUE_u32QuickCutDuration;
EXTERN uint32 TORQUE_u32ESTTorqueModifier;
EXTERN uint32 TORQUE_u32DBWTorqueModifier;
EXTERN uint32 TORQUE_u32FuelTorqueModifier;
EXTERN uint32 TORQUE_u32IdleStabilisationTorque;
EXTERN uint32 TORQUE_u32TorquePedalEstimateScaled;
EXTERN bool TORQUE_u32PedalWOT;
EXTERN uint32 TORQUE_u32TorqueEstimateScale;
//ASAM mode=readvalue name="Torque Estimate Scale" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=5.0 help="Torque Scale"
EXTERN uint16 TORQUE_u16ETCScale;
EXTERN uint16 TORQUE_u16ETCScaleRamped;
EXTERN uint16 TORQUE_u16GearShiftCount;
//ASAM mode=readvalue name="Shift Control Count" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=5.0 help="Shift Control Count"
EXTERN bool TORQUE_boDownShift;
EXTERN uint16 TORQUE_u16GearShiftPressureControlCount;
//ASAM mode=readvalue name="Shift Control Pressure Count" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=5.0 help="Shift Control Pressure Count"
EXTERN uint8 TORQUE_u8ATXSelectedGear;
//ASAM mode=readvalue name="ATX Selected Gear" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="ATX Selected Gear"
EXTERN uint32 TORQUE_u32RevMatchRPM;
//ASAM mode=readvalue name="Rev Match RPM" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="RPM" format=4.0 help="Rev Match Downshift RPM"
EXTERN uint16 TORQUE_u16RevMatchPosition;
//ASAM mode=readvalue name="Rev Match ETC Position" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=4.0 help="Rev Match ETC Position"
EXTERN bool TORQUE_boVehicleMovingUS;
//ASAM mode=readvalue name="ATX Vehicle Moving Upshifts" type=uint8 offset=0 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="ATX Vehicle Moving Upshifts"
EXTERN bool TORQUE_boVehicleMovingDS;
//ASAM mode=readvalue name="ATX Vehicle Moving Downshifts" type=uint8 offset=0 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="ATX Vehicle Moving Downshifts"
EXTERN bool TORQUE_boManualShiftMode;
//ASAM mode=readvalue name="ATX Manual Shift Mode" type=uint8 offset=0 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="ATX Manual Shift Mode"
EXTERN bool TORQUE_boPostShift;
EXTERN bool TORQUE_boESTTorqueModify;


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void TORQUE_vStart(uint32 * const pu32Arg);
void TORQUE_vRun(uint32* const pu32Arg);
void TORQUE_vTerminate(uint32* const pu32Arg);
void TORQUE_vCallBack(puint32 const pu32Arg);

#endif // TORQUE_H
