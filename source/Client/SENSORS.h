/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Sensors Header                                         */
/* DESCRIPTION:        This code manages the sensors AD conversions           */
/*                                                                            */
/*                                                                            */
/* FILE NAME:          SENSORS.h                                              */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SENSORS_H
#define SENSORS_H

#include <string.h>
#include "CONV.h"
#include "IOAPI.h"
#include "USER.h"
#include "SYS.h"
#include "TEPMAPI.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
typedef enum
{
	SENSORS_enADAFR,
	SENSORS_enADPPSM,
	SENSORS_enADPPSS,
	SENSORS_enAUX1,
	SENSORS_enAUX2,
	SENSORS_enAUX3,
	SENSORS_enAUX4,
	SENSORS_enADInvalid,
	SENSORS_enADCount
} SENSORS_tenADConfig;

typedef enum
{
	SENSORS_enAUX_OFF = 0,
	SENSORS_enAUX_SECONDARY_Timing_Map_LOW = 2 ,
	SENSORS_enAUX_SECONDARY_Timing_Map_HIGH = 3,
	SENSORS_enAUX_SECONDARY_TAFR_Map_LOW = 4,
	SENSORS_enAUX_SECONDARY_TAFR_Map_HIGH = 5,
	SENSORS_enAUX_ANTILAG_LOW = 6,
	SENSORS_enAUX_ANTILAG_HIGH = 7,
	SENSORS_enAUX_DRAG_MODE_LOW = 8,
	SENSORS_enAUX_DRAG_MODE_HIGH = 9,
	SENSORS_enAUX_LAUNCH_LOW = 10,
	SENSORS_enAUX_LAUNCH_HIGH = 11,
} SENSORS_tenAUXConfig;

#define SENSORS_nFastPeriod 				(2u)
#define SENSORS_nSlowPeriod					(8u)
#define SENSORS_nADScaleMax					(4095u)
#define SENSORS_nSensorsRefVolts            (5000u)
#define SENSORS_nADRefVolts                 (3300u)
#define SENSORS_nVDivRatio					(128u)		/* 1/2 voltage divider for 3.3V ECUHost */
#define SENSORS_nFastFTMDivisor             TEPMAPI_enDiv128
#define SENSORS_nFastFTMFreq                (SYS_FREQ_BUS / (1 << SENSORS_nFastFTMDivisor))
#define SENSORS_nSlowFTMDivisor             TEPMAPI_enDiv128
#define SENSORS_nSlowFTMFreq                (SYS_FREQ_BUS / (1 << SENSORS_nSlowFTMDivisor))
#define SENSORS_nSENSORADMAX                (2850)
#define SENSORS_nHertzSamplesMax            16

#ifdef BUILD_SPARKDOG_PF
#define VRA_nPullupEnablePin                EH_IO_GP2
#define VRB_nPullupEnablePin                EH_IO_UART2_TX
#define VRA_nVREnablePin                    EH_IO_UART2_RX
#define VRB_nVREnablePin                    EH_IO_GPSE7
#define VR_nHystHighPin                     EH_IO_GP7
#define VR_nHystLowPin                      EH_IO_GP1
#define VR_nPhaseTelltalePin                EH_IO_GP10
#define VVTAB_nPullupEnablePin              EH_IO_IIC1_SDA
#define VVTCD_nPullupEnablePin              EH_IO_IIC1_SCL
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
#define VRA_nPullupEnablePin                EH_IO_GP11
#define VRB_nPullupEnablePin                EH_IO_UART2_TX
#define VRA_nVREnablePin                    EH_IO_UART2_RX
#define VRB_nVREnablePin                    EH_IO_GPSE7
#define VR_nHystHighPin                     EH_IO_GP13
#define VR_nHystLowPin                      EH_IO_GP12
#define VR_nPhaseTelltalePin                EH_IO_GP10
#define VVTAB_nPullupEnablePin              EH_IO_IIC1_SDA
#define VVTCD_nPullupEnablePin              EH_IO_IIC1_SCL
#endif //BUILD_SPARKDOG_TEENSY_ADAPT


#ifdef BUILD_SPARKDOG_PF
#define CRANK_nInput                        EH_IO_TMR11
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
#define CRANK_nInput                        EH_IO_TMR10
#endif //BUILD_SPARKDOG_TEENSY_ADAPT

#ifdef BUILD_BSP_AFM_FREQ

#ifdef BUILD_SPARKDOG_PF
#define AFM_FREQ_nInput                     EH_IO_TMR10
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
#define AFM_FREQ_nInput                     EH_IO_TMR11
#endif //BUILD_SPARKDOG_TEENSY_ADAPT

#endif //BUILD_BSP_AFM_FREQ

//#define CTS_nSwitchPin                      EH_IO_GPSE1

#if 0
#define CLO2_nSwitchBank1Pin				EH_IO_GPSE2
#define CLO2_nSwitchBank2Pin				EH_IO_GPSE3
#define CLO2_nOxBank2Pin                    EH_IO_GPSE4
#define CLO2_nOxBank1Pin                    EH_IO_GPSE5
#endif

//#define CAM_nDebug							EH_IO_GPSE1

#define SENSORS_nOBDSTT1PID	                6
#define SENSORS_nOBDLTT1PID	                7
#define SENSORS_nOBDSTT2PID	                8
#define SENSORS_nOBDLTT2PID	                9
#define SENSORS_nOBDESTADVPID	            14
#define SENSORS_nAuxChannelCount            4
#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _SENSORS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define SENSORS_nCANBufferMap  \
{3,2,1,0,7,6,5,4,8+3,8+2,8+1,8+0,8+7,8+6,8+5,8+4,16+3,16+2,16+1,16+0,16+7,16+6,16+5,16+4,24+3,24+2,24+1,24+0,24+7,24+6,24+5,24+4}

#define PPSM_RAMP_MAX_NEG   25
#define PPSM_RAMP_MAX_POS   100


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN bool SENSORS_boCTSACBiasHigh;
EXTERN uint8 SENSORS_u8CANCTS;
//ASAM mode=readvalue name="CAN CTS" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="CTS CAN"
EXTERN uint8 SENSORS_u8CANATS;
//ASAM mode=readvalue name="CAN ATS" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="ATS CAN"
EXTERN uint16 SENSORS_u16CANTPS;
//ASAM mode=readvalue name="CAN TPS" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=5.0 help="TPS CAN"
EXTERN uint8 SENSORS_u8CANPPS;
//ASAM mode=readvalue name="CAN PPS" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PPS CAN"
//EXTERN uint16 SENSORS_u16CANVSS;
//ASAMREMOVED mode=readvalue name="CAN VSS" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="VSS CAN"
EXTERN uint16 SENSORS_au16CANAFR[2];
//ASAM mode=readvalue name="CAN AFR" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="VSS AFR"
EXTERN uint8 SENSORS_u8CANSTT[2];



EXTERN uint8 SENSORS_u8CANLTT[2];

EXTERN uint8 SENSORS_u8OBDAdv;

EXTERN bool SENSORS_boBrakePedalPressed;

EXTERN bool SENSORS_boCANCTSNewSample;
EXTERN bool SENSORS_boCANATSNewSample;
EXTERN bool SENSORS_boCANTPSNewSample;
EXTERN bool SENSORS_boCANPPSNewSample;
EXTERN bool SENSORS_boCANVSSNewSample;
EXTERN bool SENSORS_aboCANUEGONewSample[2];
EXTERN bool SENSORS_boOBDAdvNewSample;
EXTERN bool SENSORS_boCANNewBrakePressedSample;
EXTERN bool SENSORS_boCANNewGearPositionSample;
EXTERN bool SENSORS_boCANNewTorqueRequestSample;

EXTERN uint32 SENSORS_u32PPSMVolts;
//ASAM mode=readvalue name="PPSM Voltage" type=uint16 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="PPSM Voltage"
EXTERN uint16 SENSORS_u16TPSSafeMax;
EXTERN uint16 SENSORS_u16TPSSafeMaxModified;
EXTERN uint16 SENSORS_u16CANVSS;
//ASAM mode=readvalue name="CAN VSS" type=uint16 offset=0 min=0 max=300 m=0.1 b=0 units="km/h" format=4.1 help="CAN Vehicle Speed"
EXTERN uint8 SENSORS_u8VSSCalcGear;
//ASAM mode=readvalue name="Calculated Gear" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="VSS Calculated Gear"
EXTERN uint16 SENSORS_u16VSSCalcGearRPMSlip;
//ASAM mode=readvalue name="Calculated Gear RPM Slip" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="RPM" format=4.0 help="VSS Calculated Gear RPM Slip"
EXTERN uint32 SENSORS_u32PPSMVoltsRamp;

EXTERN uint16 SENSORS_au16ADSensorValueFiltered[SENSORS_enADCount];
//ASAM mode=readvalue name="AFR Voltage" type=uint16 offset=0 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AFR Filtered Voltage"
//ASAM mode=readvalue name="PPSM Voltage" type=uint16 offset=2 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AFR Filtered Voltage"
//ASAM mode=readvalue name="PPSS Voltage" type=uint16 offset=4 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AFR Filtered Voltage"
//ASAM mode=readvalue name="AUX1 Voltage" type=uint16 offset=6 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AUX1 Filtered Voltage"
//ASAM mode=readvalue name="AUX2 Voltage" type=uint16 offset=8 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AUX2 Filtered Voltage"
//ASAM mode=readvalue name="AUX3 Voltage" type=uint16 offset=10 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AUX3 Filtered Voltage"
//ASAM mode=readvalue name="AUX4 Voltage" type=uint16 offset=12 min=0 max=10 m=0.001 b=0 units="V" format=3.2 help="AUX4 Filtered Voltage"

EXTERN bool SENSORS_aboAUXActive[SENSORS_enADCount];
//ASAM mode=readvalue name="AUX1 State" type=uint8 offset=3 min=0 max=1 units="ENUMERATION LOW=0 HIGH=1" format=3.0 help="AUX1 State"
//ASAM mode=readvalue name="AUX2 State" type=uint8 offset=4 min=0 max=1 units="ENUMERATION LOW=0 HIGH=1" format=3.0 help="AUX2 State"
//ASAM mode=readvalue name="AUX3 State" type=uint8 offset=5 min=0 max=1 units="ENUMERATION LOW=0 HIGH=1" format=3.0 help="AUX3 State"
//ASAM mode=readvalue name="AUX4 State" type=uint8 offset=6 min=0 max=1 units="ENUMERATION LOW=0 HIGH=1" format=3.0 help="AUX4 State"


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void SENSORS_vStart(uint32 * const pu32Arg);
void SENSORS_vRun(uint32* const pu32Arg);
void SENSORS_vTerminate(uint32* const pu32Arg);
void SENSORS_vCallBack(puint32 const pu32Arg);
bool SENSORS_boSetupADSE(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tenSamplesAv enSamplesAv, ADCAPI_tpfResultCB pfResultCB, ADCAPI_tenTrigger enTrigger, puint32 pu32Arg);
void SENSORS_vInvalidateCAN16Data(uint32 u32MSGIDX, uint32 u32DataOffset);
bool SENSORS_boGetAuxActive(SENSORS_tenAUXConfig enAUXConfig);
void SENSORS_vCycleUpdate(void);

#endif // CTS_H

