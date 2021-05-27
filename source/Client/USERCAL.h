/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      USERCAL Header File                                    */
/* DESCRIPTION:        This code module initialises the user calibration      */
/*                     RAM mirror from the EEPROM or ROM image                */
/*                                                                            */
/* FILE NAME:          USERCAL.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef USERCAL_H
#define USERCAL_H

#include <BUILD.h>
#include <DECLARATIONS.h>
#include <stddef.h>
#include <string.h>
#include <SYS.h>
#include <TYPES.h>
#include "SYSAPI.h"
#include "UNITS.h"
#include "user.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define USERCAL_nFlexRAMBASE	0x14000000
#define USERCAL_nCRCCheckChunkSize 64

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

#define TESTCAL_off
#define C18CAL_off
#define KTM_SC_CAL_off
#define L98CAL_off
#define M15A_CAL_off
#define GOLF_MK6_CAL
#define TEST_60_M2_off
#define FIESTA_36_M1_off
#define FIESTA_36_M1_TEENSY_ADAPT_off
#define TEENSY_ADAPT_BASEoff

#ifdef TESTCAL
#include "TEST_CAL.h"	
#endif

#ifdef C18CAL
#include "C18_CAL.h"	
#endif

#ifdef L98CAL
#include "L98_CAL.h"
#endif

#ifdef KTM_SC_CAL
#include "KTM_CAL_SCA.h"
#endif

#ifdef M15A_CAL
#include "Suzuki_M15A.h"
#endif

#ifdef GOLF_MK6_CAL
#include "Golf_Mk6.h"
	#ifndef BUILD_SPARKDOG_PF
		#error "Bad build config"
	#endif
#endif

#ifdef TEST_60_M2
#include "Test_60_M2.h"
#endif

#ifdef FIESTA_36_M1
#include "FIESTA_36_M1.h"
#endif

#ifdef FIESTA_36_M1_TEENSY_ADAPT
#include "FIESTA_36_M1_TEENSY_ADAPT.h"
#endif

#ifdef TEENSY_ADAPT_BASE
#include "teensy_adapt_base.h"
	#ifndef BUILD_SPARKDOG_TEENSY_ADAPT
		#error "Bad build config"
	#endif
#endif //TEENSY_ADAPT_BASE

#pragma GCC diagnostic pop

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _USERCAL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to the user calibration
*******************************************************************************/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

typedef struct
/* The usercal struct MUST be single and packed with CRC16 last two bytes
	 for the Kernel CRC16 NVM store and restore to function */
	 /* CAVEAT!!! Make first element uint32 for %4 alignment!!! */
{
	/* 0 */
	GPM6_ttVolts userCalADRefH;
	GPM6_ttVolts userCalVRef445;	
	GPM6_ttVolts userCalVNernstTarget;
	GPM6_ttOhms	userCalRNernstReady;
	GPM6_ttOhms	userCalRHeaterThreshold;	
	uint32 au32UserCalPumpCurrentPFactor[1];	
	uint32 au32UserCalPumpCurrentMFactor[1];		
	sint32 ai32UserCalPumpCurrentOffset[1];		
	GPM6_ttMicroAmps aUserCalPumpSpread[17];	
	GPM6_ttLambda aUserCalPumpLambda[17];
	/* 1 */	
	uint8 au8UserCalPumpSensorTrim[1];
	uint32 aUserStoichFuelCal;
	uint8 au8UserCalPumpDeadband[1];	
	uint16 au16UserCalFreeAirPoints[19];
	GPM6_ttAmps aUserHeaterAlarmCurrent[1];	
	uint8 u8WarmUpDelay;
	uint8 u8DisplayMode;
	uint8 u8BackLightEnable;
	GPM6_ttVolts userCalTPSCalMin;
	GPM6_ttVolts userCalTPSCalMax;	
	/* 2 */	
	GPM6_ttVolts aUserCURVEAfmTFSpread[17];	
	GPM6_ttUg aUserCURVEAfmTFTable[17];	
	GPM6_ttVolts aUserInjResponseSpread[17];	
	uint16 aUserInjResponseTable[17];		
	uint32 aUserCoolantSensorSpread[17];	
	GPM6_ttTempC aUserCoolantSensorTable[17];			
	GPM6_ttTempC aUserCoolantEnrichmentSpread[17];	
	sint32 aUserCoolantEnrichmentTable[17];	
	uint32 aUserTAFRxSpread[17];	
	GPM6_ttMTheta aUserTAFRySpread[17];	
	/* 3 */	
	uint16 aUserMAPTAFR[17][17];
	uint32 aUserTimingxSpread[17];	
	GPM6_ttMTheta aUserTimingySpread[17];	
	uint16 aUserTimingMap[17][17];	
	uint32 aUserVExSpread[17];	
	GPM6_ttKiloPa aUserVEySpread[17];	
	uint16 aUserMAPVE[17][17];
	GPM6_ttTempC aUserCoolantStartEnrichmentSpread[17];	
	sint32 aUserCoolantStartEnrichmentTable[17];		
	GPM6_ttTempC aUserCoolantPostStartEnrichmentSpread[17];	
	/* 4 */	
	sint32 aUserCoolantPostStartEnrichmentTable[17];		
	uint32 aUserCrankingAirflowSpread[17];	
	uint16 aUserCrankingAirflowTable[17];	
	uint32 aUserDwellSpread[17];
	uint16 aUserDwellTable[17];
	GPM6_ttTempC aUserAirTempCorrectionSpread[17];
	uint16 aUserAirTempCorrectionTable[17];
	uint32 aUserTPSMovementCorrectionSpread[17];
	uint16 aUserTPSMovementCorrectionTable[17];
	uint32 aUserInjectionPhasingSpread[17];
	/* 5 */	
	uint16 aUserInjectionPhasingTable[17];
	GPM6_ttTempC aUserCTSTimingCorrectionSpread[17];
	uint16 aUserCTSTimingCorrectionTable[17];
	GPM6_ttTempC aUserATSTimingCorrectionSpread[17];
	uint16 aUserATSTimingCorrectionTable[17];
	GPM6_ttTempC aUserISCSpeedTargetSpread[17];
	uint16 aUserISCSpeedTargetTable[17];
	uint8 aUserStepperCloseTable[4];
	uint8 aUserStepperOpenTable[4];
	uint32 u32UserStepperHomeSteps;
	/* 6 */	
	uint16 aUserPrimaryTriggerTable[80];
	uint16 aUserSecondaryTriggerTable[40];
	uint16 aUserSyncPointsTable[36];
	uint8 u8UserPrimaryEdgeSetup;
	uint8 u8UserSecondaryEdgeSetup;
	uint8 u8UserPrimaryVREnable;
	uint8 u8UserSecondaryVREnable;
	uint8 u8UserFirstEdgeRisingPrimary;
	uint8 u8UserFirstEdgeRisingSecondary;
	uint32 au32InjectionSequence[4];
	/* 7 */	
	uint32 au32IgnitionSequence[4];
	IOAPI_tenEHIOResource enSimpleCamSyncSource;
	bool boCamSyncHighLate;
	uint32 u32CamSyncSampleToothCount;
	uint16 u16ManifoldVolumeCC;	
	uint8 u8CylCount;
	uint16 u16InjFlowRate;
	uint32 aUserBackupAirflowxSpread[11];	
	uint32 aUserBackupAirflowySpread[11];	
	uint32 aUserBackupAirflowMap[11][11];
	/* 8 */	
	uint8 u8ManifoldTimeConstantTrim;
	uint8 u8EnableBackupAirflowTransients;
	sint32 aUserPrimerSpread[11];
	uint32 aUserPrimerTable[11];
    uint32 aUserInjShortOpeningSpread[11];
    uint32 aUserInjShortOpeningTable[11];
	uint32 u32STTNegLimit;
	uint32 u32STTPosLimit;
	uint32 u32CLStepClosed;
	uint32 u32CLIncClosed;
	/* 9 */	
	uint32 u32CLStepOpen;
	uint32 u32CLIncOpen;
	uint16 u16TimingMainOffset;
	uint32 u32AFMTransientControlRPMLimit;
	uint8 u8WastedSparkEnable;
	uint16 u16AFRReinstate;
	uint16 u16AFRCutoff;
	uint32 u32TPSClosedLower;
	uint32 u32TPSClosedUpper;
	uint32 u32ISCESTTrimPos;
	/* 10 */	
	uint32 u32ISCESTTrimNeg;
	uint32 u32CLO2ESTTrimPos;
	uint32 u32CLO2ESTTrimNeg;
	uint16 u16CylinderCC;
	uint8 u8EnableAFMPrimaryInputOpen;
	uint32 u32SpeedDensityAFMTPSLim;
	uint16 au16SeqRPMLimit[8];
	uint16 u16SeqRPMLimitHyst;
	uint16 u16HighVacuumEnableKpa;
	uint16 u16HighVacuumDisableKpa;
	/* 11 */	
	uint16 u16OverrunCutEnableRPM;
	uint16 u16OverrunCutDisableRPM;
	uint16 u16RPMRunThreshold;
	uint8 u8SeqFuelAssign;
	uint16 u16ESTFilterClosed;
	uint16 u16ESTFilterOpen;
	uint16 u16ESTCLGain;
	uint8 u8EnableAFMPrimaryInputClosed;
	sint16 s16SequenceFuelOffset[4];
	uint16 u16SequenceFuelGain[4];
	/* 12 */	
	bool boTPSCANPrimary;
	bool boPPSCANPrimary;
	bool boCTSCANPrimary;
	bool boATSCANPrimary;
	bool boMAPCANPrimary;
	uint32 au32PrioCANID[4];
	uint8 au8SensorCANDataOffset[8];
	uint8 au8SensorCANDataByteCount[8];
	uint32 u32SyncPhaseRepeats;
	sint32 s32MapSensorGain;
	/* 13 */	
	sint32 s32MapSensorOffset;
	uint8 u8InjDivide;
	uint8 u8CLO2Leftenable;
	uint8 u8CLO2RightEnable;
	uint8 u8ReturnlessEnable;
	uint16 u16ReturnlessPressureKPa;
	uint8 u8CrankingAirflowEnable;
	uint8 u8StepperIACEnable;
	uint16 u16IACStepsOpenHot;
	uint16 u16IACStepsOpenCold;
	/* 14 */	
	uint16 u16IdleEntryOffset;
	GPM6_ttTempC s32RadFanOnTemp;
	GPM6_ttTempC s32RadFanOffTemp;
	IOAPI_tenEHIOResource enFuelPumpRelay;
	IOAPI_tenEHIOResource enESTBypass;
	uint32 aUserAirSensorSpread[17];
	GPM6_ttTempC aUserAirSensorTable[17];
	uint16 aFuelIOResource[4];
	uint16 aESTIOResource[4];
	uint16 aIACIOResource[4];
	bool boOBDISCADV;
	uint8 u8TriggerType;
	uint8 u8SyncType;
	uint8 u8TriggerPullStrength;
	uint8 u8SyncPullStrength;
	uint16 u16MinLinearFuelPulse;
	uint16 u16CTSADResource;
	uint16 u16MAPADResource;
	uint16 u16ATSADResource;
	uint16 u16AFMADResource;
	uint16 u16TPSADResource;
	uint32 aUserTipInCorrectionSpread[17];
	uint16 aUserTipInCorrectionTable[17];
	uint32 aLogicBlockVar[8];
	uint32 aLogicBlockOperand[8];
	uint16 aLogicBlockChainOutput[8];
	IOAPI_tenEHIOResource enThermoFanRelay;
	uint16 aESTIOMuxResource[4];
	bool boUEGOCANPrimary;
	uint16 aPWM2DAxesSourceXIDX[8];
	uint16 aPWM3DAxesSourceXIDX[4];
	uint16 aPWM3DAxesSourceYIDX[4];
	uint16 aPWM2DIOOutputResource[8];
	uint16 aPWM3DIOOutputResource[4];
	sint32 aUserPWM2D_1SpreadX[17];
	sint32 aUserPWM2D_2SpreadX[17];
	sint32 aUserPWM2D_3SpreadX[17];
	sint32 aUserPWM2D_4SpreadX[17];
	sint32 aUserPWM2D_5SpreadX[17];
	sint32 aUserPWM2D_6SpreadX[17];
	sint32 aUserPWM2D_7SpreadX[17];
	sint32 aUserPWM2D_8SpreadX[17];
	uint16 aUserPWM2D_1Data[17];
	uint16 aUserPWM2D_2Data[17];
	uint16 aUserPWM2D_3Data[17];
	uint16 aUserPWM2D_4Data[17];
	uint16 aUserPWM2D_5Data[17];
	uint16 aUserPWM2D_6Data[17];
	uint16 aUserPWM2D_7Data[17];
	uint16 aUserPWM2D_8Data[17];
	sint32 aUserPWM3D_1SpreadX[17];
	sint32 aUserPWM3D_2SpreadX[17];
	sint32 aUserPWM3D_3SpreadX[17];
	sint32 aUserPWM3D_4SpreadX[17];	
	sint32 aUserPWM3D_1SpreadY[17];
	sint32 aUserPWM3D_2SpreadY[17];
	sint32 aUserPWM3D_3SpreadY[17];
	sint32 aUserPWM3D_4SpreadY[17];
	uint16 aUserPWM3D_1Data[17][17];
	uint16 aUserPWM3D_2Data[17][17];
	uint16 aUserPWM3D_3Data[17][17];
	uint16 aUserPWM3D_4Data[17][17];
	uint16 u16FuelPressureSolenoidResource;
	uint8 u8VCTABPullupEnable;
	uint8 u8VCTCDPullupEnable;
	uint16 aVVTInputResource[4];
	uint8 aVVTInputType[4];
	uint16 u16FRSADResource;
	uint32 aUserFuelFlowRateSpread[17];
	uint16 aUserFuelFlowRateTable[17];
	uint8 u8VariableFuelPressureEnable;
	sint32 s32FuelPressureSensorGain;
	sint32 s32FuelPressureSensorOffset;
	uint16 u16FuelPressurePGain;
	uint16 u16FuelPressureIGain;
	uint8 u8FuelPrimeEnable;
	uint16 aUserTimingMapStage1[17][17];
	uint32 aUserTorquePedalTransferSpread[17];
	uint16 aUserTorquePedalTransferTable[17];
	sint32 aUserPressureValveFeedForwardSpread[17];
	uint16 aUserPressureValveFeedForwardTable[17];
	uint16 u16AFMMaxCycleDeltaUgPos;
	uint16 u16AFMMaxCycleDeltaUgNeg;
	uint16 aUserPWM2DPeriod[8];
	uint16 aUserPWM3DPeriod[4];
	uint8 u8FuelPumpPrimerTime;
	uint8 u8DBSlaveConfig;
	uint8 u8DBSlaveOptions;
	uint16 u16AFRADResource;
	uint16 u16PPSMADResource;
	uint16 u16PPSSADResource;
	uint16 au16AuxInputResource[4];
	uint16 au16AuxInputFunction[4];
	sint32 s32AFRADInputGain;
	sint32 s32AFRADInputOffset;
	uint16 u16AFRFuelTypeGain;
	uint32 aUserFuelCutsSpread[17];
	uint16 aUserFuelCutsTable[17];
	uint16 au16AuxInputLowThres[4];
	uint16 au16AuxInputHighThres[4];
	uint16 u16FuelPressureControlOffset;
	uint16 u16FuelPressureControlStartOffset;
	uint32 aUserTimingSecondary1xSpread[17];
	GPM6_ttMTheta aUserTimingSecondary1ySpread[17];
	uint16 u16ISCColdISCMin;
	uint16 u16ISCColdISCMax;
	uint16 u16ISCHotISCMin;
	uint16 u16ISCHotISCMax;
	uint16 u16GDIValveMin;
	uint16 u16GDIValveMax;
	uint16 u16GDIValveFF;
	uint32 aUserPedalTransferSpread[17];
	uint16 aUserPedalTransferTable[17];
	uint16 u16PressureControlThreshold;
	uint16 u16ThrottlePressureControlCal;
	uint16 u16PressureControlHyst;
	uint8 u8BoostChargeMAPEnable;
	uint16 u16BoostChargeTPSLimit;
	uint16 u16ISCEntryRamp;
	uint16 u16ISCPostStartRamp;
	uint32 u32AirflowFMELimitLow;
	uint32 u32AirflowFMELimitHigh;
	uint16 u16TorqueReductionMaxDuration;
	uint16 u16StallSaverRPMThreshold;
	uint32 aUserETCScaleSpread[17];
	uint16 aUserETCScaleTable[17];
	uint16 u16TorqueReductionMaxFuelCut;
	uint16 u16ShiftUpCountLimit;
	uint16 au16BoostTarget[6];
	uint16 u16GDIPressureMin;
	uint16 u16GDIPressureMax;
	uint32 u32GDIMAPMin;
	uint32 u32GDIMAPMax;
	uint16 u16ESTPosRateMax;
	uint16 u16ESTNegRateMax;
	uint16 u16PressureControlDPos;
	uint16 u16PressureControlDNeg;
	uint16 u16ShiftCountPressureControlLimit;
	uint16 u16ShiftPressureControl;
	uint16 aPWM3DSlowIOOutputResource[4];
	uint16 aPWM3DSlowPeriod[4];
	uint16 u16SensorHystLimitRPM;
	GPM6_ttVolts userCalPPSCalMin;
	GPM6_ttVolts userCalPPSCalMax;
	uint32 aUserPseudoMAPxSpread[17];
	uint32 aUserPseudoMAPySpread[17];
	uint32 aUserPseudoMAPMap[17][17];
	uint8 u8PseudoMAPEnable;
	uint16 u16VSSPerRPM[8];
	uint16 u16VSSCANCal;
	uint16 u16ETCOverrideKeys;
	uint16 u16ETCOverride;
	uint16 u16DiagType;
	uint32 aUserETCRPMMatchSpread[17];
	uint16 aUserETCRPMMatchTable[17];
	uint16 u16ShiftDownCountLimit;
	uint16 u16ShiftDownBlipLimit;
	uint16 u16ATXTorqueOnVSS;
	uint16 u16ATXTorqueOffVSS;
	GPM6_ttTempC aUserISCOpenLoopPosSpread[17];
	uint16 aUserISCOpenLoopPosTable[17];
	uint8 u8VehicleStoppedFuelCutEnable;
	uint16 u16CRC16;
	uint8* offsets;
} BUILD_PACKING USERCAL_tstCalibration;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
/* 	NOTE MUST ALWAYS INCLUDE CAL STRUCT ELEMENTS ONE FOR ONE AND IN ORDER */
EXTERN USERCAL_tstCalibration BUILD_PACKING USERCAL_stRAMCAL;
//
//
#pragma GCC diagnostic pop//
//
// 0
//ASAM mode=writevalue name="AD RefH Voltage" parent="USERCAL_stRAMCAL" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="AD Converter Reference High Voltage HTML=001.HTML"
//ASAM mode=writevalue name="User Cal VRef 445" parent="USERCAL_stRAMCAL" type=uint32 offset=4 min=0 max=5 m=0.001 b=0 units="TBC" format=4.3 help="TBC HTML=002.HTML"
//ASAM mode=writevalue name="Cal VNernst Target" parent="USERCAL_stRAMCAL" type=uint32 offset=8 min=0 max=5 m=0.001 b=0 units="TBC" format=4.3 help="TBC HTML=003.HTML"
//ASAM mode=writevalue name="Cal RNernst Ready" parent="USERCAL_stRAMCAL" type=uint32 offset=12 min=0 max=5 m=0.001 b=0 units="TBC" format=4.3 help="TBC HTML=004.HTML"
//ASAM mode=writevalue name="Uego Heater Rth" parent="USERCAL_stRAMCAL" type=uint32 offset=16 min=0 max=10 m=0.001 b=0 units="Ohms" format=4.3 help="Uego Heater Resistance Threshold HTML=005.HTML"
//ASAM mode=writevalue name="Uego1 Current P Cal" parent="USERCAL_stRAMCAL" type=uint32 offset=20 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current P Calibration (dimensionless) HTML=006.HTML"
//ASAM mode=writevalue name="Uego1 Current M Cal" parent="USERCAL_stRAMCAL" type=uint32 offset=24 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current M Calibration (dimensionless) HTML=007.HTML"
//ASAM mode=writevalue name="Uego1 Offset Cal" parent="USERCAL_stRAMCAL" type=sint32 offset=28 min=-5000000 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Offset Calibration (dimensionless) HTML=008.HTML"
//ASAM mode=writeaxis_pts name="Uego Pump Current Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=32 min=-5000000 max=5000000 m=0.000001 b=0 units="mA" format=7.6 help="Uego Pump Current X Axis HTML=009.HTML" xcount=17 xindexvar="Pump Current 1"
//ASAM mode=writecurve name="Uego Pump Current Table" parent="USERCAL_stRAMCAL" type=uint16 offset=100 min=0 max=5 m=0.001 b=0 units="dl" format=4.3 help="Uego Pump Current Curve HTML=010.HTML" xcount=17 xindexvar="Pump Current 1"
// 1
//ASAM mode=writevalue name="Uego1 Sensor Trim" parent="USERCAL_stRAMCAL" type=uint8 offset=134 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Uego1 Sensor Trim HTML=011.HTML"
//ASAM mode=writevalue name="Uego1 Stoich Cal" parent="USERCAL_stRAMCAL" type=uint32 offset=135 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="Uego1 Stoich Cal HTML=012.HTML"
//ASAM mode=writevalue name="User Cal Pump Deadband" parent="USERCAL_stRAMCAL" type=uint8 offset=139 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=013.HTML"
//ASAM mode=writevalue name="User Cal Free Air Points" parent="USERCAL_stRAMCAL" type=uint16 offset=140 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=014.HTML"
//ASAM mode=writevalue name="UserHeaterAlarmCurrent" parent="USERCAL_stRAMCAL" type=uint32 offset=168 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=015.HTML"
//ASAM mode=writevalue name="Warm Up Delay" parent="USERCAL_stRAMCAL" type=uint8 offset=172 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=016.HTML"
//ASAM mode=writevalue name="Display Mode" parent="USERCAL_stRAMCAL" type=uint8 offset=173 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=017.HTML"
//ASAM mode=writevalue name="Back Light Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=174 min=0 max=2000 units="ENUMERATION OFF=0 ON=1" format=3.0 help="TBC HTML=018.HTML"
//ASAM mode=writevalue name="TPS Cal Min" parent="USERCAL_stRAMCAL" type=uint32 offset=178 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="TPS voltage at min throttle position or neutral ETC position HTML=019.HTML"
//ASAM mode=writevalue name="TPS Cal Max" parent="USERCAL_stRAMCAL" type=uint32 offset=189 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="TPS voltage at maximum throttle position HTML=020.HTML"
// 2
//ASAM mode=writeaxis_pts name="AfmTF Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=193 min=0 max=300000 m=0.001 b=0 units="V" format=7.6 help="Air Flow Meter calibration curve x-axis points array HTML=021.HTML" xcount=17 xindexvar="AFM Sensor Voltage"
//ASAM mode=writecurve name="AfmTF Table" parent="USERCAL_stRAMCAL" type=uint32 offset=261 min=0 max=400 m=0.000001 b=0 units="g/s" format=4.3 help=Air Flow Meter calibration curve data points array HTML=022.HTML" xcount=17 xindexvar="AFM Sensor Voltage"
//ASAM mode=writeaxis_pts name="Inj Response Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=329 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Injector response curve x-axis points array HTML=023.HTML" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Inj Response Table" parent="USERCAL_stRAMCAL" type=uint16 offset=397 min=0 max=3 m=0.001 b=0 units="ms" format=4.3 help="Injector response curve data points array HTML=024.HTML" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writeaxis_pts name="Coolant Sensor Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=431 min=0 max=5 m=1 b=0 units="V" format=7.0 help="Coolant sensor calibration curve x-axis points array HTML=025.HTML" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writecurve name="Coolant Sensor Table" parent="USERCAL_stRAMCAL" type=sint32 offset=499 min=-40 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="Coolant sensor calibration curve data points array HTML=026.HTML" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writeaxis_pts name="CTS Enrichment Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=567 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Enrichment X Axis HTML=027.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Enrichment Table" parent="USERCAL_stRAMCAL" type=sint32 offset=635 min=-1 max=3 m=0.001 b=0 units="%" format=4.3 help="CTS Enrichment Curve HTML=028.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="TAFR Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=703 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis HTML=029.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="TAFR Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=771 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis HTML=030.HTML" xcount=17 xindexvar="MAP kPa"
// 3
//ASAM mode=writemap name="TAFR Map" parent="USERCAL_stRAMCAL" type=uint16 offset=839 min=0 max=20 m=0.01 b=0 units="dl" format=4.3 help="TAFR Map HTML=031.HTML" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="Throttle Angle"
//ASAM mode=writeaxis_pts name="Timing Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=1417 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis HTML=032.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Timing Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=1485 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis HTML=033.HTML" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writemap name="Timing Map" parent="USERCAL_stRAMCAL" type=uint16 offset=1553 min=0 max=50 m=0.1 b=0 units="Degrees" format=3.1 help="Timing Map HTML=034.HTML" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="MAP kPa"
//ASAM mode=writeaxis_pts name="VE Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=2131 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis HTML=035.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="VE Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=2199 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis HTML=036.HTML" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writemap name="VE Map" parent="USERCAL_stRAMCAL" type=uint16 offset=2267 min=50 max=150 m=0.1 b=0 units="%" format=4.1 help="VE Map HTML=037.HTML" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="MAP kPa"
//ASAM mode=writeaxis_pts name="CTS Start Enrichment Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=2845 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Start Enrichment X Axis HTML=038.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Start Enrichment Table" parent="USERCAL_stRAMCAL" type=sint32 offset=2913 min=0 max=4 m=0.001 b=0 units="%" format=4.3 help="CTS Start Enrichment Curve HTML=039.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="CTS Post Start Enrichment Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=2981 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Post Start Enrichment X Axis HTML=040.HTML" xcount=17 xindexvar="Coolant Temperature"
// 4
//ASAM mode=writecurve name="CTS Post Start Enrichment Table" parent="USERCAL_stRAMCAL" type=sint32 offset=3049 min=0 max=4 m=0.001 b=0 units="%" format=4.3 help="CTS Post Start Enrichment Curve HTML=041.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="Cranking Airflow Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3117 min=0 max=8000 m=1 b=0 units="RPM" format=4.3 help="Cranking Airflow X Axis HTML=042.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Cranking Airflow Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3185 min=0 max=65.535 m=0.001 b=0 units="g/s" format=4.3 help="Cranking Airflow Curve HTML=043.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Dwell Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3219 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Dwell X Axis HTML=044.HTML" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Dwell Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3287 min=0 max=10 m=0.001 b=0 units="ms" format=4.3 help="Dwell Curve HTML=045.HTML" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writeaxis_pts name="ATS Fuel Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3321 min=0 max=20 m=0.001 b=0 units="V" format=4.3 help="ATS Fuel Correction X Axis HTML=046.HTML" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writecurve name="ATS Fuel Correction Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3389 min=0 max=2 m=0.001 b=0 units="%" format=4.3 help="ATS Fuel Correction Curve HTML=047.HTML" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writeaxis_pts name="TPS Movement Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3423 min=-450 max=450 m=0.001 b=0 units="Degrees/s" format=4.3 help="TPS Movement Correction X Axis HTML=048.HTML" xcount=17 xindexvar="TPS Movement Rate"
//ASAM mode=writecurve name="TPS Movement Correction Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3491 min=0 max=2 m=0.001 b=0 units="%" format=6.3 help="TPS Movement Correction Curve HTML=049.HTML" xcount=17 xindexvar="TPS Movement Rate"
//ASAM mode=writeaxis_pts name="Injection Phasing Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3525 min=0 max=20000 m=0.001 b=0 units="RPM" format=4.3 help="Injection Phasing X Axis HTML=050.HTML" xcount=17 xindexvar="Engine Speed Raw"
// 5
//ASAM mode=writecurve name="Injection Phasing Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3593 min=0 max=20 m=0.001 b=0 units="ms" format=4.3 help="Injection Phasing Curve HTML=281.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="CTS Timing Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3627 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Timing Correction X Axis HTML=051.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Timing Correction Table" parent="USERCAL_stRAMCAL" type=sint16 offset=3695 min=-10 max=10 m=0.1 b=0 units="Degrees" format=4.3 help="CTS Timing Correction Curve HTML=052.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="ATS Timing Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3729 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ATS Timing Correction X Axis HTML=053.HTML" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writecurve name="ATS Timing Correction Table" parent="USERCAL_stRAMCAL" type=sint16 offset=3797 min=-10 max=10 m=0.1 b=0 units="Degrees" format=4.3 help="ATS Timing Correction Curve HTML=054.HTML" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writeaxis_pts name="ISC Target Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3831 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ISC Target X Axis HTML=055.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="ISC Target Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3899 min=0 max=2000 m=1 b=0 units="RPM" format=4.3 help="ISC Target Curve HTML=056.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writevalue name="User Stepper Close Table" parent="USERCAL_stRAMCAL" type=uint32 offset=3933 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=057.HTML"
//ASAM mode=writevalue name="User Stepper Open Table" parent="USERCAL_stRAMCAL" type=uint32 offset=3937 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=058.HTML"
//ASAM mode=writevalue name="User Stepper Home Steps" parent="USERCAL_stRAMCAL" type=uint32 offset=3941 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=059.HTML"
// 6
//ASAM mode=writeblob name="Primary Trigger Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3945 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=80 help="Primary Trigger Tooth Setup HTML=060.HTML"
//ASAM mode=writeblob name="Secondary Trigger Table" parent="USERCAL_stRAMCAL" type=uint16 offset=4105 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=40 help="Secondary Trigger Tooth Setup HTML=061.HTML"
//ASAM mode=writeblob name="Sync Points Table" parent="USERCAL_stRAMCAL" type=uint16 offset=4185 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=36 help="Syncronisation Points Setup HTML=062.HTML"
//ASAM mode=writevalue name="Primary Edge Setup" parent="USERCAL_stRAMCAL" type=uint8 offset=4257 min=0 max=255 units="ENUMERATION RISING=0 FALLING=1 ANY=2" format=5.0 help="Primary Edge Setup HTML=063.HTML"
//ASAM mode=writevalue name="Secondary Edge Setup" parent="USERCAL_stRAMCAL" type=uint8 offset=4258 min=0 max=255 units="ENUMERATION RISING=0 FALLING=1 ANY=2" format=5.0 help="Secondary Edge Setup HTML=064.HTML"
//ASAM mode=writevalue name="Primary VR Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=4259 min=0 max=255 units="ENUMERATION OFF=0 ON=1" format=5.0 help="Primary VR Circuit Enabl HTML=065.HTMLe"
//ASAM mode=writevalue name="Secondary VR Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=4260 min=0 max=255 units="ENUMERATION OFF=0 ON=1" format=5.0 help="Secondary VR Circuit Enable HTML=066.HTML"
//ASAM mode=writevalue name="First Edge Rising Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=4261 min=0 max=255 units="ENUMERATION FALSE=0 TRUE=1" format=3.0 help="First Edge Rising Primary HTML=067.HTML"
//ASAM mode=writevalue name="First Edge Rising Secondary" parent="USERCAL_stRAMCAL" type=uint8 offset=4262 min=0 max=255 units="ENUMERATION FALSE=0 TRUE=1" format=3.0 help="First Edge Rising Secondary HTML=068.HTML"
//ASAM mode=writevalue name="Injection Sequence AE" parent="USERCAL_stRAMCAL" type=uint32 offset=4263 min=0 max=65535 units="ENUMERATION BATCH_POS1st=4 BATCH_POS2nd=261 BATCH_POS3rd=518 BATCH_POS4th=775 SEQ_POS3rd=65280 SEQ_POS4th=65282 SEQ_POS1st=65284 SEQ_POS2nd=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=069.HTML"
//ASAM mode=writevalue name="Injection Sequence BF" parent="USERCAL_stRAMCAL" type=uint32 offset=4267 min=0 max=65535 units="ENUMERATION BATCH_POS1st=4 BATCH_POS2nd=261 BATCH_POS3rd=518 BATCH_POS4th=775 SEQ_POS3rd=65280 SEQ_POS4th=65282 SEQ_POS1st=65284 SEQ_POS2nd=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=070.HTML"
//ASAM mode=writevalue name="Injection Sequence CG" parent="USERCAL_stRAMCAL" type=uint32 offset=4271 min=0 max=65535 units="ENUMERATION BATCH_POS1st=4 BATCH_POS2nd=261 BATCH_POS3rd=518 BATCH_POS4th=775 SEQ_POS3rd=65280 SEQ_POS4th=65282 SEQ_POS1st=65284 SEQ_POS2nd=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=071.HTML"
//ASAM mode=writevalue name="Injection Sequence DH" parent="USERCAL_stRAMCAL" type=uint32 offset=4275 min=0 max=65535 units="ENUMERATION BATCH_POS1st=4 BATCH_POS2nd=261 BATCH_POS3rd=518 BATCH_POS4th=775 SEQ_POS3rd=65280 SEQ_POS4th=65282 SEQ_POS1st=65284 SEQ_POS2nd=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=072.HTML"

// 7
//ASAM mode=writevalue name="Ignition Sequence AE" parent="USERCAL_stRAMCAL" type=uint32 offset=4279 min=0 max=65535 units="ENUMERATION WASTED_POS1st=4 WASTED_POS2nd=261 WASTED_POS3rd=518 WASTED_POS4th=775 SEQ_POS1st=65280 SEQ_POS2nd=65282 SEQ_POS3rd=65284 SEQ_POS4th=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=073.HTML"
//ASAM mode=writevalue name="Ignition Sequence BF" parent="USERCAL_stRAMCAL" type=uint32 offset=4283 min=0 max=65535 units="ENUMERATION WASTED_POS1st=4 WASTED_POS2nd=261 WASTED_POS3rd=518 WASTED_POS4th=775 SEQ_POS1st=65280 SEQ_POS2nd=65282 SEQ_POS3rd=65284 SEQ_POS4th=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=074.HTML"
//ASAM mode=writevalue name="Ignition Sequence CG" parent="USERCAL_stRAMCAL" type=uint32 offset=4287 min=0 max=65535 units="ENUMERATION WASTED_POS1st=4 WASTED_POS2nd=261 WASTED_POS3rd=518 WASTED_POS4th=775 SEQ_POS1st=65280 SEQ_POS2nd=65282 SEQ_POS3rd=65284 SEQ_POS4th=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=075.HTML"
//ASAM mode=writevalue name="Ignition Sequence DH" parent="USERCAL_stRAMCAL" type=uint32 offset=4291 min=0 max=65535 units="ENUMERATION WASTED_POS1st=4 WASTED_POS2nd=261 WASTED_POS3rd=518 WASTED_POS4th=775 SEQ_POS1st=65280 SEQ_POS2nd=65282 SEQ_POS3rd=65284 SEQ_POS4th=65286 SEQ_UNUSED=65535" format=3.0 help="TBC HTML=076.HTML"
//ASAM mode=writevalue name="Simple Cam Sync Source" parent="USERCAL_stRAMCAL"  type=uint8 offset=4295 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=077.HTML"
//ASAM mode=writevalue name="Cam Sync High Late" parent="USERCAL_stRAMCAL" type=uint8 offset=4296 min=0 max=2000 units="ENUMERATION FALSE=0 TRUE=1" format=3.0 help="TBC HTML=078.HTML"
//ASAM mode=writevalue name="Cam Sync Sample Tooth Count" parent="USERCAL_stRAMCAL" type=uint32 offset=4297 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=079.HTML"
//ASAM mode=writevalue name="Manifold Volume" parent="USERCAL_stRAMCAL" type=uint16 offset=4301 min=0 max=65535 m=1 b=0 units="cc" format=5.0 help="Manifold Volume CC HTML=080.HTML"
//ASAM mode=writevalue name="Cyl Count" parent="USERCAL_stRAMCAL" type=uint8 offset=4303 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=081.HTML"
//ASAM mode=writevalue name="Injector Flow Rate" parent="USERCAL_stRAMCAL" type=uint16 offset=4304 min=0 max=20000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=082.HTML"
//ASAM mode=writeaxis_pts name="Airflow Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=4306 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="Airflow MAP Y Axis HTML=083.HTML" xcount=11 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Airflow Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=4350 min=0 max=90000 m=0.001 b=0 units="Degrees" format=3.1 help="Airflow MAP X Axis HTML=084.HTML" xcount=11 xindexvar="Throttle Angle"
//ASAM mode=writemap name="Airflow Map" parent="USERCAL_stRAMCAL" type=uint32 offset=4394 min=0 max=150 m=0.000001 b=0 units="g/s" format=9.6 help="Airflow Map HTML=085.HTML" xcount=11 xindexvar="Engine Speed Raw" ycount=11 yindexvar="Throttle Angle"
// 8
//ASAM mode=writevalue name="Manifold Time Constant Cal" parent="USERCAL_stRAMCAL" type=uint8 offset=4878 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Manifold Time Constant Cal HTML=086.HTML"
//ASAM mode=writevalue name="Airflow Transient Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=4879 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=1.0 help="Airflow Transients Enable HTML=087.HTML"
//ASAM mode=writeaxis_pts name="Injector Start Primer Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=4880 min=0 max=2 m=0.001 b=0 units="ms" format=4.3 help="Injector Short Opening X Axis HTML=088.HTML" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writecurve name="Injector Start Primer Table" parent="USERCAL_stRAMCAL" type=uint32 offset=4924 min=0 max=2 m=1 b=0 units="ms" format=4.3 help="Injector Short Opening Curve HTML=089.HTML" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writeaxis_pts name="Injector Short Opening Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=4968 min=0 max=2 m=0.001 b=0 units="ms" format=4.3 help="Injector Short Opening X Axis HTML=090.HTML" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writecurve name="Injector Short Opening Table" parent="USERCAL_stRAMCAL" type=uint32 offset=5012 min=0 max=2 m=0.001 b=0 units="ms" format=4.3 help="Injector Short Opening Curve HTML=091.HTML" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writevalue name="STT Neg Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5056 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=092.HTML"
//ASAM mode=writevalue name="STT Pos Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5060 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC HTML=093.HTML"
//ASAM mode=writevalue name="CLO2 Step Closed" parent="USERCAL_stRAMCAL" type=uint32 offset=5064 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC HTML=094.HTML"
//ASAM mode=writevalue name="CLO2 Inc Closed" parent="USERCAL_stRAMCAL" type=uint32 offset=5068 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC HTML=095.HTML"
// 9
//ASAM mode=writevalue name="CLO2 Step Open" parent="USERCAL_stRAMCAL" type=uint32 offset=5072 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC HTML=096.HTML"
//ASAM mode=writevalue name="CLO2 Inc Open" parent="USERCAL_stRAMCAL" type=uint32 offset=5076 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC HTML=097.HTML"
//ASAM mode=writevalue name="Timing Main Offset" parent="USERCAL_stRAMCAL" type=uint16 offset=5080 min=0 max=360 m=0.001 b=0 units="Degrees" format=1.0 help="TBC HTML=098.HTML"
//ASAM mode=writevalue name="AFM Transient Control RPM Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5082 min=0 max=8000 m=1 b=0 units="RPM" format=1.0 help="TBC HTML=099.HTML"
//ASAM mode=writevalue name="Wasted Spark Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5086 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=1.0 help="TBC HTML=100.HTML"
//ASAM mode=writevalue name="AFR Reinstate" parent="USERCAL_stRAMCAL" type=uint16 offset=5087 min=0 max=1 m=0.001 b=0 units="dl" format=1.0 help="TBC HTML=101.HTML"
//ASAM mode=writevalue name="AFR Cutoff" parent="USERCAL_stRAMCAL" type=uint16 offset=5089 min=0 max=1 m=0.001 b=0 units="Degrees" format=1.0 help="TBC HTML=102.HTML"
//ASAM mode=writevalue name="TPS Closed Lower" parent="USERCAL_stRAMCAL" type=uint32 offset=5091 min=0 max=90 m=0.001 b=0 units="Degrees" format=1.0 help="TBC HTML=103.HTML"
//ASAM mode=writevalue name="TPS Closed Upper" parent="USERCAL_stRAMCAL" type=uint32 offset=5095 min=0 max=90 m=0.001 b=0 units="Degrees" format=1.0 help="TBC HTML=104.HTML"
//ASAM mode=writevalue name="ISC EST Trim Pos" parent="USERCAL_stRAMCAL" type=uint32 offset=5099 min=0 max=15 m=0.001 b=0 units="Degrees" format=5.3 help="ISC EST Trim Positive Max HTML=105.HTML"
// 10
//ASAM mode=writevalue name="ISC EST Trim Neg" parent="USERCAL_stRAMCAL" type=uint32 offset=5103 min=0 max=15 m=0.001 b=0 units="Degrees" format=5.3 help="ISC EST Trim Negative Max HTML=106.HTML"
//ASAM mode=writevalue name="CLO2 EST Trim Pos" parent="USERCAL_stRAMCAL" type=uint32 offset=5107 min=0 max=5 m=0.001 b=0 units="Degrees" format=5.3 help="Closed Loop Fuel EST Trim Positive Max HTML=107.HTML"
//ASAM mode=writevalue name="CLO2 EST Trim Neg" parent="USERCAL_stRAMCAL" type=uint32 offset=5111 min=0 max=5 m=0.001 b=0 units="Degrees" format=5.3 help="Closed Loop Fuel EST Trim Negative Max HTML=108.HTML"
//ASAM mode=writevalue name="Cylinder CC" parent="USERCAL_stRAMCAL" type=uint16 offset=5115 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="Cylinder Capacity HTML=109.HTML"
//ASAM mode=writevalue name="AFM Primary Input Enable Open" parent="USERCAL_stRAMCAL" type=uint8 offset=5117 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=1.0 help="AFM Primary Input Enable Open HTML=110.HTML"
//ASAM mode=writevalue name="Speed Density AFM TPS Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5118 min=0 max=90 m=.001 b=0 units="degrees" format=5.3 help="TBC HTML=111.HTML"
//ASAM mode=writevalue name="Seq RPM Limit AE" parent="USERCAL_stRAMCAL" type=uint16 offset=5122 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC HTML=112.HTML"
//ASAM mode=writevalue name="Seq RPM Limit BF" parent="USERCAL_stRAMCAL" type=uint16 offset=5124 suboffset=2 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC HTML=113.HTML"
//ASAM mode=writevalue name="Seq RPM Limit CG" parent="USERCAL_stRAMCAL" type=uint16 offset=5126 suboffset=4 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC HTML=114.HTML"
//ASAM mode=writevalue name="Seq RPM Limit DH" parent="USERCAL_stRAMCAL" type=uint16 offset=5128 suboffset=6 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC HTML=115.HTML"
//ASAM mode=writevalue name="SeqRPMLimit5" parent="USERCAL_stRAMCAL" type=uint16 offset=5130 suboffset=8 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC HTML=116.HTML"
//ASAM mode=writevalue name="SeqRPMLimit6" parent="USERCAL_stRAMCAL" type=uint16 offset=5132 suboffset=10 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC HTML=117.HTML"
//ASAM mode=writevalue name="SeqRPMLimit7" parent="USERCAL_stRAMCAL" type=uint16 offset=5134 suboffset=12 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC HTML=118.HTML"
//ASAM mode=writevalue name="SeqRPMLimit8" parent="USERCAL_stRAMCAL" type=uint16 offset=5136 suboffset=14 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC HTML=119.HTML"
//ASAM mode=writevalue name="Seq RPM Limit Hyst" parent="USERCAL_stRAMCAL" type=uint16 offset=5138 min=0 max=500 m=1 b=0 units="dl" format=1.0 help="TBC HTML=120.HTML"
//ASAM mode=writevalue name="HighVacuumEnableKpa" parent="USERCAL_stRAMCAL" type=uint16 offset=5140 min=0 max=101.3 m=0.001 b=0 units="dl" format=1.0 help="TBC HTML=121.HTML"
//ASAM mode=writevalue name="HighVacuumDisableKpa" parent="USERCAL_stRAMCAL" type=uint16 offset=5142 min=0 max=101.3 m=0.001 b=0 units="dl" format=1.0 help="TBC HTML=123.HTML"
// 11
//ASAM mode=writevalue name="OverrunCutEnableRPM" parent="USERCAL_stRAMCAL" type=uint16 offset=5144 min=0 max=2000 m=1 b=0 units="dl" format=1.0 help="TBC HTML=124.HTML"
//ASAM mode=writevalue name="OverrunCutDisableRPM" parent="USERCAL_stRAMCAL" type=uint16 offset=5146 min=0 max=2000 m=1 b=0 units="dl" format=1.0 help="TBC HTML=125.HTML"
//ASAM mode=writevalue name="RPMRunThreshold" parent="USERCAL_stRAMCAL" type=uint16 offset=5148 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="TBC HTML=126.HTML"
//ASAM mode=writevalue name="CLO2 Sequence Assignment" parent="USERCAL_stRAMCAL" type=uint8 offset=5150 min=0 max=255 units="ENUMERATION CYL1stL-CYL2ndL-CYL3rdL-CYL4thL=0 CYL1stR-CYL2ndL-CYL3rdL-CYL4thL=1 CYL1stL-CYL2ndR-CYL3rdL-CYL4thL=2 CYL1stR-CYL2ndR-CYL3rdL-CYL4thL=3 CYL1stL-CYL2ndL-CYL3rdR-CYL4thL=4 CYL1stR-CYL2ndL-CYL3rdR-CYL4thL=5 CYL1stL-CYL2ndR-CYL3rdR-CYL4thL=6 CYL1stR-CYL2ndR-CYL3rdR-CYL4thL=7 CYL1stL-CYL2ndL-CYL3rdL-CYL4thR=8 CYL1stR-CYL2ndL-CYL3rdL-CYL4thR=9 CYL1stL-CYL2ndR-CYL3rdL-CYL4thR=10 CYL1stR-CYL2ndR-CYL3rdL-CYL4thR=11 CYL1stL-CYL2ndL-CYL3rdR-CYL4thR=12 CYL1stR-CYL2ndL-CYL3rdR-CYL4thR=13 CYL1stL-CYL2ndR-CYL3rdR-CYL4thR=14 CYL1stR-CYL2ndR-CYL3rdR-CYL4thR=15" format=1.0 help="CLO2 Sequence Assignment HTML=126.HTML"
//ASAM mode=writevalue name="EST Filter Closed" parent="USERCAL_stRAMCAL" type=uint16 offset=5151 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="EST Filter Closed HTML=127.HTML"
//ASAM mode=writevalue name="EST Filter Open" parent="USERCAL_stRAMCAL" type=uint16 offset=5153 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="EST Filter Open HTML=128.HTML"
//ASAM mode=writevalue name="EST CL Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5155 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="EST CL Gain HTML=129.HTML"
//ASAM mode=writevalue name="AFM Primary Input Enable Closed" parent="USERCAL_stRAMCAL" type=uint8 offset=5157 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=1.0 help="AFM Primary Input Enable Closed HTML=130.HTML"
//ASAM mode=writevalue name="Sequence 1 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5158 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 1 Fuel Offset HTML=131.HTML"
//ASAM mode=writevalue name="Sequence 2 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5160 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 2 Fuel Offset HTML=132.HTML"
//ASAM mode=writevalue name="Sequence 3 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5162 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 3 Fuel Offset HTML=133.HTML"
//ASAM mode=writevalue name="Sequence 4 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5164 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 4 Fuel Offset HTML=134.HTML"
//ASAM mode=writevalue name="Sequence 1 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5166 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 1 Fuel Gain HTML=135.HTML"
//ASAM mode=writevalue name="Sequence 2 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5168 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 2 Fuel Gain HTML=136.HTML"
//ASAM mode=writevalue name="Sequence 3 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5170 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 3 Fuel Gain HTML=137.HTML"
//ASAM mode=writevalue name="Sequence 4 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5172 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 4 Fuel Gain HTML=138.HTML"
// 12
//ASAM mode=writevalue name="TPS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5174 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="TPS CAN Enable HTML=139.HTML"
//ASAM mode=writevalue name="PPS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5175 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="PPS CAN Enable HTML=140.HTML"
//ASAM mode=writevalue name="CTS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5176 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="CTS CAN Enable HTML=141.HTML"
//ASAM mode=writevalue name="ATS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5177 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="ATS CAN Enable HTML=142.HTML"
//ASAM mode=writevalue name="MAP CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5178 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="MAP CAN Enable HTML=143.HTML"
//ASAM mode=writevalue name="Priority CAN ID 1" parent="USERCAL_stRAMCAL" type=uint32 offset=5179 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Priority CAN ID 1 HTML=144.HTML"
//ASAM mode=writevalue name="Sensor CAN Data Offset" parent="USERCAL_stRAMCAL" type=uint8 offset=5195 min=1 max=255 m=1 b=0 units="dl" format=4.0 help="Priority CAN ID 1 HTML=145.HTML"
//ASAM mode=writevalue name="Sensor CAN Data ByteCount" parent="USERCAL_stRAMCAL" type=uint8 offset=5203 min=1 max=255 m=1 b=0 units="dl" format=4.0 help="Priority CAN ID 1 HTML=146.HTML"
//ASAM mode=writevalue name="Sync Phase Repeats" parent="USERCAL_stRAMCAL" type=uint32 offset=5211 min=1 max=4095 units="ENUMERATION BATCH=1 SEQUENTIAL=2" format=4.0 help="Sync Phase Repeats HTML=147.HTML"
//ASAM mode=writevalue name="Map Sensor Gain" parent="USERCAL_stRAMCAL" type=sint32 offset=5215 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=148.HTML"
// 13
//ASAM mode=writevalue name="Map Sensor Offset" parent="USERCAL_stRAMCAL" type=sint32 offset=5219 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=149.HTML"
//ASAM mode=writevalue name="Inj Divide" parent="USERCAL_stRAMCAL" type=uint8 offset=5223 min=1 max=4 units="ENUMERATION TBI=4 BATCH=2 SEQUENTIAL=1" format=4.0 help="Sync Phase Repeats HTML=150.HTML"
//ASAM mode=writevalue name="CLO2 LeftEnable" parent="USERCAL_stRAMCAL" type=uint8 offset=5224 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=4.0 help="Sync Phase Repeats HTML=151.HTML"
//ASAM mode=writevalue name="CLO2 RightEnable" parent="USERCAL_stRAMCAL" type=uint8 offset=5225 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=4.0 help="Sync Phase Repeats HTML=152.HTML"
//ASAM mode=writevalue name="Returnless Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5226 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=4.0 help="Sync Phase Repeats HTML=153.HTML"
//ASAM mode=writevalue name="Returnless Pressure kPa" parent="USERCAL_stRAMCAL" type=uint16 offset=5227 min=1 max=500 m=1 b=0 units="kPa" format=4.0 help="Sync Phase Repeats HTML=154.HTML"
//ASAM mode=writevalue name="Cranking Airflow Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5229 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=4.0 help="Sync Phase Repeats HTML=155.HTML"
//ASAM mode=writevalue name="Stepper IAC Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5230 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=4.0 help="Sync Phase Repeats HTML=156.HTML"
//ASAM mode=writevalue name="IAC Steps Open Hot" parent="USERCAL_stRAMCAL" type=uint16 offset=5231 min=0 max=255 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=157.HTML"
//ASAM mode=writevalue name="IAC Steps Open Cold" parent="USERCAL_stRAMCAL" type=uint16 offset=5233 min=0 max=255 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=158.HTML"
// 14
//ASAM mode=writevalue name="Idle Entry RPM Offset" parent="USERCAL_stRAMCAL" type=uint16 offset=5235 min=1 max=1000 m=1 b=0 units="RPM" format=4.0 help="Idle Entry RPM Offset HTML=159.HTML"
//ASAM mode=writevalue name="Rad Fan On Temp" parent="USERCAL_stRAMCAL" type=sint32 offset=5237 min=-40 max=200 m=0.001 b=1 units="degC" format=4.0 help="Rad Fan On Temp HTML=160.HTML"
//ASAM mode=writevalue name="Rad Fan Off Temp" parent="USERCAL_stRAMCAL" type=sint32 offset=5241 min=-40 max=200 m=0.001 b=1 units="degC" format=4.0 help="Rad Fan Off Temp HTML=161.HTML"
//ASAM mode=writevalue name="Fuel Pump Relay" parent="USERCAL_stRAMCAL" type=uint8 offset=5245 min=1 max=100 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=4.0 help="Fuel Pump Relay HTML=162.HTML"
//ASAM mode=writevalue name="EST Bypass" parent="USERCAL_stRAMCAL" type=uint8 offset=5246 min=1 max=1000 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=4.0 help="EST Bypass HTML=163.HTML"
//ASAM mode=writeaxis_pts name="Air Sensor Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=5247 min=0 max=5 m=1 b=0 units="V" format=7.0 help="Air Sensor X Axis HTML=164.HTML" xcount=17 xindexvar="ATS Sensor Voltage"
//ASAM mode=writecurve name="Air Sensor Table" parent="USERCAL_stRAMCAL" type=sint32 offset=5315 min=-40 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="Air Sensor Curve HTML=165.HTML" xcount=17 xindexvar="ATS Sensor Voltage"
//ASAM mode=writevalue name="Fuel 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5383 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="Fuel Sequence 1 IO Resource HTML=166.HTML"
//ASAM mode=writevalue name="Fuel 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5385 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="Fuel Sequence 2 IO Resource HTML=167.HTML"
//ASAM mode=writevalue name="Fuel 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5387 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="Fuel Sequence 3 IO Resource HTML=168.HTML"
// 15
//ASAM mode=writevalue name="Fuel 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5389 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="Fuel Sequence 4 IO Resource HTML=169.HTML"
//ASAM mode=writevalue name="EST 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5391 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 1 IO Resource HTML=170.HTML"
//ASAM mode=writevalue name="EST 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5393 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 2 IO Resource HTML=171.HTML"
//ASAM mode=writevalue name="EST 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5395 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 3 IO Resource HTML=172.HTML"
//ASAM mode=writevalue name="EST 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5397 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 4 IO Resource HTML=173.HTML"
//ASAM mode=writevalue name="IAC 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5399 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="IAC 1 IO Resource HTML=174.HTML"
//ASAM mode=writevalue name="IAC 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5401 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="IAC 2 IO Resource HTML=175.HTML"
//ASAM mode=writevalue name="IAC 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5403 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="IAC 3 IO Resource HTML=176.HTML"
//ASAM mode=writevalue name="IAC 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5405 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="IAC 4 IO Resource HTML=177.HTML"
//ASAM mode=writevalue name="OBD ISC Advance Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5407 min=0 max=1 units="ENUMERATION OFF=0 ON=1" format=1.0 help="IAC Enable OBD Idle Advance HTML=178.HTML"

//ASAM mode=writevalue name="Trigger Type" parent="USERCAL_stRAMCAL" type=uint8 offset=5408 min=0 max=255 units="ENUMERATION AUTO=0 M15A=1 EA888=2 2NEFE=3" format=1.0 help="Trigger Type HTML=178.HTML"
//ASAM mode=writevalue name="Sync Type" parent="USERCAL_stRAMCAL" type=uint8 offset=5409 min=0 max=255 units="ENUMERATION AUTO=0 M15A=1 EA888=2" format=1.0 help="Sync Type HTML=179.HTML"
//ASAM mode=writevalue name="Trigger Pull Strength" parent="USERCAL_stRAMCAL" type=uint8 offset=5410 min=0 max=2 units="ENUMERATION WEAK=0 MEDIUM=1 STRONG=2" format=1.0 help="Trigger Pull Strength HTML=180.HTML"
//ASAM mode=writevalue name="Sync Pull Strength" parent="USERCAL_stRAMCAL" type=uint8 offset=5411 min=0 max=2 units="ENUMERATION WEAK=0 MEDIUM=1 STRONG=2" format=1.0 help="Sync Pull Strength HTML=181.HTML"
//ASAM mode=writevalue name="Min Linear Fuel Pulse" parent="USERCAL_stRAMCAL" type=uint16 offset=5412 min=0 max=2 m=0.001 b=0 units="ms" format=1.0 help="Min Linear Fuel Pulse HTML=182.HTML"

//ASAM mode=writevalue name="CTS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5414 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="CTS AD Resource HTML=183.HTML"
//ASAM mode=writevalue name="MAP AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5416 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="MAP AD Resource HTML=184.HTML"
//ASAM mode=writevalue name="ATS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5418 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="ATS AD Resource HTML=185.HTML"
//ASAM mode=writevalue name="AFM AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5420 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="AFM AD Resource HTML=186.HTML"
//ASAM mode=writevalue name="TPS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5422 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="TPS AD Resource HTML=187.HTML"

//ASAM mode=writeaxis_pts name="Tip In Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=5424 min=0 max=8000 m=1 b=0 units="RPM" format=4.3 help="Tip In Correction X Axis HTML=188.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Tip In Correction Table" parent="USERCAL_stRAMCAL" type=uint16 offset=5492 min=0 max=2 m=0.001 b=0 units="%" format=6.3 help="Tip In Correction Curve" HTML=189.HTML xcount=17 xindexvar="Engine Speed Raw"

//ASAM mode=writevalue name="Logic Block Var A" parent="USERCAL_stRAMCAL" type=uint32 offset=5526 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 1 HTML=190.HTML"
//ASAM mode=writevalue name="Logic Block Var B" parent="USERCAL_stRAMCAL" type=uint32 offset=5530 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 2 HTML=191.HTML"
//ASAM mode=writevalue name="Logic Block Var C" parent="USERCAL_stRAMCAL" type=uint32 offset=5534 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 3 HTML=192.HTML"
//ASAM mode=writevalue name="Logic Block Var D" parent="USERCAL_stRAMCAL" type=uint32 offset=5538 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 4 HTML=193.HTML"
//ASAM mode=writevalue name="Logic Block Var E" parent="USERCAL_stRAMCAL" type=uint32 offset=5542 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 5 HTML=194.HTML"
//ASAM mode=writevalue name="Logic Block Var F" parent="USERCAL_stRAMCAL" type=uint32 offset=5546 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 6 HTML=195.HTML"
//ASAM mode=writevalue name="Logic Block Var G" parent="USERCAL_stRAMCAL" type=uint32 offset=5550 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 7 HTML=196.HTML"
//ASAM mode=writevalue name="Logic Block Var H" parent="USERCAL_stRAMCAL" type=uint32 offset=5554 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 8 HTML=197.HTML"

//ASAM mode=writevalue name="Logic Block Operand A" parent="USERCAL_stRAMCAL" type=uint32 offset=5558 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 1 HTML=198.HTML"
//ASAM mode=writevalue name="Logic Block Operand B" parent="USERCAL_stRAMCAL" type=uint32 offset=5562 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 2 HTML=199.HTML"
//ASAM mode=writevalue name="Logic Block Operand C" parent="USERCAL_stRAMCAL" type=uint32 offset=5566 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 3 HTML=200.HTML"
//ASAM mode=writevalue name="Logic Block Operand D" parent="USERCAL_stRAMCAL" type=uint32 offset=5570 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 4 HTML=201.HTML"
//ASAM mode=writevalue name="Logic Block Operand E" parent="USERCAL_stRAMCAL" type=uint32 offset=5574 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 5 HTML=202.HTML"
//ASAM mode=writevalue name="Logic Block Operand F" parent="USERCAL_stRAMCAL" type=uint32 offset=5578 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 6 HTML=203.HTML"
//ASAM mode=writevalue name="Logic Block Operand G" parent="USERCAL_stRAMCAL" type=uint32 offset=5582 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 7 HTML=204.HTML"
//ASAM mode=writevalue name="Logic Block Operand H" parent="USERCAL_stRAMCAL" type=uint32 offset=5586 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 8 HTML=205.HTML"
	
//ASAM mode=writevalue name="Logic Block Chain Output A" parent="USERCAL_stRAMCAL" type=uint16 offset=5590 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 1 HTML=206.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output B" parent="USERCAL_stRAMCAL" type=uint16 offset=5592 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 2 HTML=207.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output C" parent="USERCAL_stRAMCAL" type=uint16 offset=5594 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 3 HTML=208.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output D" parent="USERCAL_stRAMCAL" type=uint16 offset=5596 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 4 HTML=209.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output E" parent="USERCAL_stRAMCAL" type=uint16 offset=5598 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 5 HTML=210.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output F" parent="USERCAL_stRAMCAL" type=uint16 offset=5600 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 6 HTML=211.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output G" parent="USERCAL_stRAMCAL" type=uint16 offset=5602 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 7 HTML=212.HTML"
//ASAM mode=writevalue name="Logic Block Chain Output H" parent="USERCAL_stRAMCAL" type=uint16 offset=5604 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 8 HTML=213.HTML"

//ASAM mode=writevalue name="Thermofan Relay" parent="USERCAL_stRAMCAL" type=uint8 offset=5606 min=1 max=100 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=4.0 help="Thermofan Relay HTML=214.HTML"

//ASAM mode=writevalue name="EST 1 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5607 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 1 IO Mux Resource HTML=215.HTML"
//ASAM mode=writevalue name="EST 2 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5609 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 2 IO Mux Resource HTML=216.HTML"
//ASAM mode=writevalue name="EST 3 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5611 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 3 IO Mux Resource HTML=217.HTML"
//ASAM mode=writevalue name="EST 4 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5613 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="EST Sequence 4 IO Mux Resource HTML=218.HTML"
//ASAM mode=writevalue name="UEGO CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5615 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="UEGO CAN Enable HTML=219.HTML"

//ASAM mode=writevalue name="PWM 2D 1 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5616 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 1 Axis Source X HTML=220.HTML"
//ASAM mode=writevalue name="PWM 2D 2 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5618 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 2 Axis Source X HTML=221.HTML"
//ASAM mode=writevalue name="PWM 2D 3 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5620 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 3 Axis Source X HTML=222.HTML"
//ASAM mode=writevalue name="PWM 2D 4 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5622 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 4 Axis Source X HTML=223.HTML"
//ASAM mode=writevalue name="PWM 2D 5 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5624 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 5 Axis Source X HTML=224.HTML"
//ASAM mode=writevalue name="PWM 2D 6 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5626 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 6 Axis Source X HTML=225.HTML"
//ASAM mode=writevalue name="PWM 2D 7 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5628 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 7 Axis Source X HTML=226.HTML"
//ASAM mode=writevalue name="PWM 2D 8 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5630 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 2D 8 Axis Source X HTML=227.HTML"
	
//ASAM mode=writevalue name="PWM 3D 1 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5632 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 1 Axis Source X HTML=228.HTML"
//ASAM mode=writevalue name="PWM 3D 2 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5634 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 2 Axis Source X HTML=229.HTML"
//ASAM mode=writevalue name="PWM 3D 3 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5636 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 3 Axis Source X HTML=230.HTML"
//ASAM mode=writevalue name="PWM 3D 4 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5638 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 4 Axis Source X HTML=231.HTML"
	
//ASAM mode=writevalue name="PWM 3D 1 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5640 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 1 Axis Source Y HTML=232.HTML"
//ASAM mode=writevalue name="PWM 3D 2 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5642 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 2 Axis Source Y HTML=233.HTML"
//ASAM mode=writevalue name="PWM 3D 3 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5644 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 3 Axis Source Y HTML=234.HTML"
//ASAM mode=writevalue name="PWM 3D 4 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5646 min=0 max=255 units="ENUMERATION RPM=0 TPS=1 MAP=2 CTS=3 ATS=4 BOOST-CONTROL=5 NONE=255" format=3.0 help="PWM 3D 4 Axis Source Y HTML=235.HTML"

//ASAM mode=writevalue name="PWM 2D 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5648 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 1 IO Resource HTML=236.HTML"
//ASAM mode=writevalue name="PWM 2D 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5650 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 2 IO Resource HTML=237.HTML"
//ASAM mode=writevalue name="PWM 2D 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5652 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 3 IO Resource HTML=238.HTML"
//ASAM mode=writevalue name="PWM 2D 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5654 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 4 IO Resource HTML=239.HTML"
//ASAM mode=writevalue name="PWM 2D 5 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5656 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 5 IO Resource HTML=240.HTML"
//ASAM mode=writevalue name="PWM 2D 6 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5658 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 6 IO Resource HTML=241.HTML"
//ASAM mode=writevalue name="PWM 2D 7 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5660 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 7 IO Resource HTML=242.HTML"
//ASAM mode=writevalue name="PWM 2D 8 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5662 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 2D 8 IO Resourc HTML=243.HTML"

//ASAM mode=writevalue name="PWM 3D 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5664 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D 1 IO Resource HTML=244.HTML"
//ASAM mode=writevalue name="PWM 3D 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5666 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D 2 IO Resource HTML=245.HTML"
//ASAM mode=writevalue name="PWM 3D 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5668 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D 3 IO Resource HTML=246.HTML"
//ASAM mode=writevalue name="PWM 3D 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5670 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D 4 IO Resource HTML=247.HTML"

//ASAM mode=writeaxis_pts name="PWM 2D Table 1_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5672 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 1 X Axis HTML=248.HTML" xcount=17 xindexvar="Generic Source IDX1"
//ASAM mode=writeaxis_pts name="PWM 2D Table 2_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5740 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 2 X Axis HTML=249.HTML" xcount=17 xindexvar="Generic Source IDX2"
//ASAM mode=writeaxis_pts name="PWM 2D Table 3_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5808 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 3 X Axis HTML=250.HTML" xcount=17 xindexvar="Generic Source IDX3"
//ASAM mode=writeaxis_pts name="PWM 2D Table 4_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5876 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 4 X Axis HTML=251.HTML" xcount=17 xindexvar="Generic Source IDX4"
//ASAM mode=writeaxis_pts name="PWM 2D Table 5_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5944 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 5 X Axis HTML=252.HTML" xcount=17 xindexvar="Generic Source IDX5"
//ASAM mode=writeaxis_pts name="PWM 2D Table 6_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6012 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 6 X Axis HTML=253.HTML" xcount=17 xindexvar="Generic Source IDX6"
//ASAM mode=writeaxis_pts name="PWM 2D Table 7_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6080 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 7 X Axis HTML=254.HTML" xcount=17 xindexvar="Generic Source IDX7"
//ASAM mode=writeaxis_pts name="PWM 2D Table 8_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6148 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 8 X Axis HTML=255.HTML" xcount=17 xindexvar="Generic Source IDX8"
		
//ASAM mode=writecurve name="PWM 2D Table 1" parent="USERCAL_stRAMCAL" type=uint16 offset=6216 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 1 HTML=256.HTML" xcount=17 xindexvar="Generic Source IDX1"
//ASAM mode=writecurve name="PWM 2D Table 2" parent="USERCAL_stRAMCAL" type=uint16 offset=6250 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 2 HTML=257.HTML" xcount=17 xindexvar="Generic Source IDX2"
//ASAM mode=writecurve name="PWM 2D Table 3" parent="USERCAL_stRAMCAL" type=uint16 offset=6284 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 3 HTML=258.HTML" xcount=17 xindexvar="Generic Source IDX3"
//ASAM mode=writecurve name="PWM 2D Table 4" parent="USERCAL_stRAMCAL" type=uint16 offset=6318 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 4 HTML=259.HTML" xcount=17 xindexvar="Generic Source IDX4"
//ASAM mode=writecurve name="PWM 2D Table 5" parent="USERCAL_stRAMCAL" type=uint16 offset=6352 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 5 HTML=260.HTML" xcount=17 xindexvar="Generic Source IDX5"
//ASAM mode=writecurve name="PWM 2D Table 6" parent="USERCAL_stRAMCAL" type=uint16 offset=6386 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 6 HTML=261.HTML" xcount=17 xindexvar="Generic Source IDX6"
//ASAM mode=writecurve name="PWM 2D Table 7" parent="USERCAL_stRAMCAL" type=uint16 offset=6420 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 7 HTML=262.HTML" xcount=17 xindexvar="Generic Source IDX7"
//ASAM mode=writecurve name="PWM 2D Table 8" parent="USERCAL_stRAMCAL" type=uint16 offset=6454 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 2D Curve 8 HTML=263.HTML" xcount=17 xindexvar="Generic Source IDX8"

//ASAM mode=writeaxis_pts name="PWM 3D Map 1_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6488 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 1 X Axis HTML=264.HTML" xcount=17 xindexvar="Generic Source IDX9"
//ASAM mode=writeaxis_pts name="PWM 3D Map 2_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6556 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 2 X Axis HTML=265.HTML" xcount=17 xindexvar="Generic Source IDX10"
//ASAM mode=writeaxis_pts name="PWM 3D Map 3_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6624 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 3 X Axis HTML=266.HTML" xcount=17 xindexvar="Generic Source IDX11"
//ASAM mode=writeaxis_pts name="PWM 3D Map 4_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6692 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 4 X Axis HTML=267.HTML" xcount=17 xindexvar="Generic Source IDX12"
//ASAM mode=writeaxis_pts name="PWM 3D Map 1_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6760 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 1 Y Axis HTML=268.HTML" xcount=17 xindexvar="Generic Source IDX13"
//ASAM mode=writeaxis_pts name="PWM 3D Map 2_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6828 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 2 Y Axis HTML=269.HTML" xcount=17 xindexvar="Generic Source IDX14"
//ASAM mode=writeaxis_pts name="PWM 3D Map 3_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6896 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 3 Y Axis HTML=270.HTML" xcount=17 xindexvar="Generic Source IDX15"
//ASAM mode=writeaxis_pts name="PWM 3D Map 4_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6964 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 4 Y Axis HTML=271.HTMl" xcount=17 xindexvar="Generic Source IDX16"
	
//ASAM mode=writemap name="PWM 3D Map 1" parent="USERCAL_stRAMCAL" type=uint16 offset=7032 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 3D Map 1 HTML=272.HTML" xcount=17 xindexvar="Generic Source IDX9" ycount=17 yindexvar="Generic Source IDX13"
//ASAM mode=writemap name="PWM 3D Map 2" parent="USERCAL_stRAMCAL" type=uint16 offset=7610 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 3D Map 2 HTML=273.HTML" xcount=17 xindexvar="Generic Source IDX10" ycount=17 yindexvar="Generic Source IDX14"
//ASAM mode=writemap name="PWM 3D Map 3" parent="USERCAL_stRAMCAL" type=uint16 offset=8188 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 3D Map 3 HTML=274.HTML" xcount=17 xindexvar="Generic Source IDX11" ycount=17 yindexvar="Generic Source IDX15"
//ASAM mode=writemap name="PWM 3D Map 4" parent="USERCAL_stRAMCAL" type=uint16 offset=8766 min=0 max=100 m=0.00152 b=0 units="%" format=4.3 help="PWM 3D Map 4 HTML=275.HTML" xcount=17 xindexvar="Generic Source IDX12" ycount=17 yindexvar="Generic Source IDX16"

//ASAM mode=writevalue name="Fuel Pressure Solenoid Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9344 min=0 max=255 m=1 b=0 units="ENUMERATION EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42" format=3.0 help="Fuel Pressure Solenoid IO Resource HTML=276.HTML"
//ASAM mode=writevalue name="VCT AB Input Pullup Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=9346 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="VCT AB Input Pullup Enable HTML=277.HTML"
//ASAM mode=writevalue name="VCT CD Input Pullup Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=9347 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="VCT AB Input Pullup Enable HTML=278.HTML"

//ASAM mode=writevalue name="VVT Input 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9348 min=0 max=255 units="ENUMERATION EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_UNUSED=125" format=3.0 help="VVT Input 1 IO Resource HTML=279.HTML"
//ASAM mode=writevalue name="VVT Input 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9350 min=0 max=255 units="ENUMERATION EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_UNUSED=125" format=3.0 help="VVT Input 2 IO Resource HTML=280.HTML"
//ASAM mode=writevalue name="VVT Input 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9352 min=0 max=255 units="ENUMERATION EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_UNUSED=125" format=3.0 help="VVT Input 3 IO Resource HTML=281.HTML"
//ASAM mode=writevalue name="VVT Input 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9354 min=0 max=255 units="ENUMERATION EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_UNUSED=125" format=3.0 help="VVT Input 4 IO Resource HTML=282.HTML"

//ASAM mode=writevalue name="VVT Input 1 Type" parent="USERCAL_stRAMCAL" type=uint8 offset=9356 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="VVT Input 1 Type HTML=283.HTML"
//ASAM mode=writevalue name="VVT Input 2 Type" parent="USERCAL_stRAMCAL" type=uint8 offset=9357 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="VVT Input 2 Type HTML=284.HTML"
//ASAM mode=writevalue name="VVT Input 3 Type" parent="USERCAL_stRAMCAL" type=uint8 offset=9358 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="VVT Input 3 Type HTML=285.HTML"
//ASAM mode=writevalue name="VVT Input 4 Type" parent="USERCAL_stRAMCAL" type=uint8 offset=9359 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="VVT Input 4 Type HTML=286.HTML"

//ASAM mode=writevalue name="Fuel Pressure AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9360 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="Fuel Pressure AD Resource HTML=287.HTML"

//ASAM mode=writeaxis_pts name="Fuel Flow Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=9362 min=0 max=20000 m=1 b=0 units="kPa" format=4.3 help="Fuel Flow curve x-axis points array HTML=288.HTML" xcount=17 xindexvar="Fuel Rail kPa"
//ASAM mode=writecurve name="Fuel Flow Table" parent="USERCAL_stRAMCAL" type=uint16 offset=9430 min=0 max=20000 m=1 b=0 units="mg/s" format=4.0 help="Fuel Flow curve data points array HTML=289.HTML" xcount=17 xindexvar="Fuel Rail kPa"

//ASAM mode=writevalue name="Fuel Pressure Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=9464 min=1 max=1 m=1 b=0 units="dl" format=4.0 help="Variable Fuel Pressure Enable HTML=290.HTML"
//ASAM mode=writevalue name="Fuel Pressure Sensor Gain" parent="USERCAL_stRAMCAL" type=sint32 offset=9465 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Sensor Gain HTML=291.HTML"
//ASAM mode=writevalue name="Fuel Pressure Sensor Offset" parent="USERCAL_stRAMCAL" type=sint32 offset=9469 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Sensor Offset HTML=292.HTML"

//ASAM mode=writevalue name="Fuel Pressure P Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=9473 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC HTML=293.HTML"
//ASAM mode=writevalue name="Fuel Pressure I Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=9475 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC HTML=294.HTML"

//ASAM mode=writevalue name="Fuel Prime Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=9477 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="TBC HTML=295.HTML"
//ASAM mode=writemap name="Secondary Timing Map 1" parent="USERCAL_stRAMCAL" type=uint16 offset=9478 min=0 max=50 m=0.1 b=0 units="Degrees" format=3.1 help="Timing Map Stage 1 HTML=296.HTML" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="MAP kPa"

//ASAM mode=writeaxis_pts name="Torque Pedal Transfer Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10056 min=0 max=20000 m=1 b=0 units="kPa" format=4.3 help="Pedal torque transfer curve x-axis points array HTML=297.HTML" xcount=17 xindexvar="Output Torque Estimate"
//ASAM mode=writecurve name="Torque Pedal Transfer Table" parent="USERCAL_stRAMCAL" type=uint16 offset=10124 min=0 max=20000 m=1 b=0 units="mg/s" format=4.0 help="Pedal Torque transfer data points array HTML=298.HTML" xcount=17 xindexvar="Output Torque Estimate"

//ASAM mode=writeaxis_pts name="Pressure Valve Feedforward Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=10158 min=0 max=20000 m=1 b=0 units="kPa" format=4.3 help="Pedal torque transfer curve x-axis points array HTML=299.HTML" xcount=17 xindexvar="Output Torque Estimate"
//ASAM mode=writecurve name="Pressure Valve Feedforward Table" parent="USERCAL_stRAMCAL" type=uint16 offset=10226 min=0 max=20000 m=1 b=0 units="mg/s" format=4.0 help="Pedal Torque transfer data points array HTML=300.HTML" xcount=17 xindexvar="Output Torque Estimate"

//ASAM mode=writevalue name="AFM Max Cycle Delta Pos" parent="USERCAL_stRAMCAL" type=uint16 offset=10260 min=0 max=65535 m=1 b=0 units="mg/cyc" format=4.0 help="Pedal Torque transfer data points array HTML=301.HTML" xcount=17 xindexvar="Output Torque Estimate"
//ASAM mode=writevalue name="AFM Max Cycle Delta Neg" parent="USERCAL_stRAMCAL" type=uint16 offset=10262 min=0 max=65535 m=1 b=0 units="mg/cyc" format=4.0 help="Pedal Torque transfer data points array HTML=302.HTML" xcount=17 xindexvar="Output Torque Estimate"

//ASAM mode=writevalue name="PWM 2D Period 1" parent="USERCAL_stRAMCAL" type=uint16 offset=10264 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 1 HTML=303.HTML"
//ASAM mode=writevalue name="PWM 2D Period 2" parent="USERCAL_stRAMCAL" type=uint16 offset=10266 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 2 HTML=304.HTML"
//ASAM mode=writevalue name="PWM 2D Period 3" parent="USERCAL_stRAMCAL" type=uint16 offset=10268 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 3 HTML=305.HTML"
//ASAM mode=writevalue name="PWM 2D Period 4" parent="USERCAL_stRAMCAL" type=uint16 offset=10270 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 4 HTML=306.HTML"
//ASAM mode=writevalue name="PWM 2D Period 5" parent="USERCAL_stRAMCAL" type=uint16 offset=10272 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 5 HTML=307.HTML"
//ASAM mode=writevalue name="PWM 2D Period 6" parent="USERCAL_stRAMCAL" type=uint16 offset=10274 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 6 HTML=308.HTML"
//ASAM mode=writevalue name="PWM 2D Period 7" parent="USERCAL_stRAMCAL" type=uint16 offset=10276 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 7 HTML=309.HTML"
//ASAM mode=writevalue name="PWM 2D Period 8" parent="USERCAL_stRAMCAL" type=uint16 offset=10278 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 8 HTML=310.HTML"

//ASAM mode=writevalue name="PWM 3D Period 1" parent="USERCAL_stRAMCAL" type=uint16 offset=10280 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 3D Period 1 HTML=311.HTML"
//ASAM mode=writevalue name="PWM 3D Period 2" parent="USERCAL_stRAMCAL" type=uint16 offset=10282 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 3D Period 2 HTML=312.HTML"
//ASAM mode=writevalue name="PWM 3D Period 3" parent="USERCAL_stRAMCAL" type=uint16 offset=10284 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 3D Period 3 HTML=313.HTML"
//ASAM mode=writevalue name="PWM 3D Period 4" parent="USERCAL_stRAMCAL" type=uint16 offset=10286 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 3D Period 4 HTML=314.HTML"

//ASAM mode=writevalue name="Fuel pump primer Time" parent="USERCAL_stRAMCAL" type=uint8 offset=10288 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 1 HTML=315.HTML"
//ASAM mode=writevalue name="Daughterboard Slave Config" parent="USERCAL_stRAMCAL" type=uint8 offset=10289 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 1 HTML=316.HTML"
//ASAM mode=writevalue name="Daughterboard Slave Options" parent="USERCAL_stRAMCAL" type=uint8 offset=10290 min=0 max=65535 m=0.00213 b=0 units="ms" format=4.1 help="PWM 2D Period 1 HTML=317.HTML"

//ASAM mode=writevalue name="AFR AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10291 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="AFR AD Resource HTML=318.HTML"
//ASAM mode=writevalue name="PPSM AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10293 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="AFR AD Resource HTML=319.HTML"
//ASAM mode=writevalue name="PPSS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10295 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="AFR AD Resource HTML=320.HTML"

//ASAM mode=writevalue name="Auxiliary Input 1 Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10297 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="Auxiliary Input 1 Resource HTML=321.HTML"
//ASAM mode=writevalue name="Auxiliary Input 2 Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10299 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="Auxiliary Input 2 Resource HTML=322.HTML"
//ASAM mode=writevalue name="Auxiliary Input 3 Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10301 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="Auxiliary Input 3 Resource HTML=323.HTML"
//ASAM mode=writevalue name="Auxiliary Input 4 Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10303 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_I_ADD1=8 EH_I_ADD2=9 EH_I_CMP1=10 EH_I_CMP2=11 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_UNUSED=125" format=3.0 help="Auxiliary Input 4 Resource HTML=324.HTML"

//ASAM mode=writevalue name="Auxiliary Input 1 Function" parent="USERCAL_stRAMCAL" type=uint16 offset=10305 min=0 max=255 units="ENUMERATION Off=0 Secondary_Timing_Map_(Low)=2 Secondary_Timing_Map_(High)=3 Secondary_TAFR_Map_(Low)=4 Secondary_TAFR_Map_(High)=5 Antilag_(Low)=6 Antilag_(High)=7 Drag_Mode_(Low)=8 Drag_Mode_(High)=9 Launch_(Low)=10 Launch_(High)=11" format=3.0 help="Auxiliary Input 1 Type HTML=325.HTML"
//ASAM mode=writevalue name="Auxiliary Input 2 Function" parent="USERCAL_stRAMCAL" type=uint16 offset=10307 min=0 max=255 units="ENUMERATION Off=0 Secondary_Timing_Map_(Low)=2 Secondary_Timing_Map_(High)=3 Secondary_TAFR_Map_(Low)=4 Secondary_TAFR_Map_(High)=5 Antilag_(Low)=6 Antilag_(High)=7 Drag_Mode_(Low)=8 Drag_Mode_(High)=9 Launch_(Low)=10 Launch_(High)=11" format=3.0 help="Auxiliary Input 2 Type HTML=326.HTML"
//ASAM mode=writevalue name="Auxiliary Input 3 Function" parent="USERCAL_stRAMCAL" type=uint16 offset=10309 min=0 max=255 units="ENUMERATION Off=0 Secondary_Timing_Map_(Low)=2 Secondary_Timing_Map_(High)=3 Secondary_TAFR_Map_(Low)=4 Secondary_TAFR_Map_(High)=5 Antilag_(Low)=6 Antilag_(High)=7 Drag_Mode_(Low)=8 Drag_Mode_(High)=9 Launch_(Low)=10 Launch_(High)=11" format=3.0 help="Auxiliary Input 3 Type HTML=327.HTML"
//ASAM mode=writevalue name="Auxiliary Input 4 Function" parent="USERCAL_stRAMCAL" type=uint16 offset=10311 min=0 max=255 units="ENUMERATION Off=0 Secondary_Timing_Map_(Low)=2 Secondary_Timing_Map_(High)=3 Secondary_TAFR_Map_(Low)=4 Secondary_TAFR_Map_(High)=5 Antilag_(Low)=6 Antilag_(High)=7 Drag_Mode_(Low)=8 Drag_Mode_(High)=9 Launch_(Low)=10 Launch_(High)=11" format=3.0 help="Auxiliary Input 4 Type HTML=328.HTML"

//ASAM mode=writevalue name="AFR Input Gain" parent="USERCAL_stRAMCAL" type=sint32 offset=10313 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=329.HTML"
//ASAM mode=writevalue name="AFR Input Offset" parent="USERCAL_stRAMCAL" type=sint32 offset=10317 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=330.HTML"

//ASAM mode=writevalue name="AFR Fuel Type Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=10321 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats HTML=331.HTML"

//ASAM mode=writeaxis_pts name="Fuel Cuts Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10323 min=0 max=20000 m=1 b=0 units="RPM" format=4.3 help="Fuel Flow Cuts x-axis points array HTML=332.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Fuel Cuts Table" parent="USERCAL_stRAMCAL" type=uint16 offset=10391 min=0 max=20000 m=1 b=0 units="%" format=4.0 help="Fuel Flow Cuts data points array HTML=333.HTML" xcount=17 xindexvar="Engine Speed Raw"

//ASAM mode=writevalue name="Aux 1 Threshold Low" parent="USERCAL_stRAMCAL" type=uint16 offset=10425 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=334.HTML"
//ASAM mode=writevalue name="Aux 2 Threshold Low" parent="USERCAL_stRAMCAL" type=uint16 offset=10427 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=335.HTML"
//ASAM mode=writevalue name="Aux 3 Threshold Low" parent="USERCAL_stRAMCAL" type=uint16 offset=10429 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=336.HTML"
//ASAM mode=writevalue name="Aux 4 Threshold Low" parent="USERCAL_stRAMCAL" type=uint16 offset=10431 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=337.HTML"
//ASAM mode=writevalue name="Aux 1 Threshold High" parent="USERCAL_stRAMCAL" type=uint16 offset=10433 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=338.HTML"
//ASAM mode=writevalue name="Aux 2 Threshold High" parent="USERCAL_stRAMCAL" type=uint16 offset=10435 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=339.HTML"
//ASAM mode=writevalue name="Aux 3 Threshold High" parent="USERCAL_stRAMCAL" type=uint16 offset=10437 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=340.HTML"
//ASAM mode=writevalue name="Aux 4 Threshold High" parent="USERCAL_stRAMCAL" type=uint16 offset=10439 min=0 max=4095 m=0.001 b=0 units="V" format=4.3 help="Sync Phase Repeats HTML=341.HTML"

//ASAM mode=writevalue name="GDI Vlv Offset" parent="USERCAL_stRAMCAL" type=uint16 offset=10441 min=0 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Offset HTML=342.HTML"
//ASAM mode=writevalue name="GDI Vlv Start Offset" parent="USERCAL_stRAMCAL" type=uint16 offset=10443 min=0 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Start Offset HTML=343.HTML"

//ASAM mode=writeaxis_pts name="Secondary Timing Map 1_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10445 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis HTML=344.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Secondary Timing Map 1_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10513 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis HTML=345.HTML" xcount=17 xindexvar="MAP kPa"


//ASAM mode=writevalue name="IAC Cold ISC Min" parent="USERCAL_stRAMCAL" type=uint16 offset=10581 min=0 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Start Offset HTML=346.HTML"
//ASAM mode=writevalue name="IAC Cold ISC Max" parent="USERCAL_stRAMCAL" type=uint16 offset=10583 min=0 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Start Offset HTML=347.HTML"
//ASAM mode=writevalue name="IAC Hot ISC Min" parent="USERCAL_stRAMCAL" type=uint16 offset=10585 min=0 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Start Offset HTML=348.HTML"
//ASAM mode=writevalue name="IAC Hot ISC Max" parent="USERCAL_stRAMCAL" type=uint16 offset=10587 min=0 max=4095 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Start Offset HTML=349.HTML"
//ASAM mode=writevalue name="GDI Valve Min" parent="USERCAL_stRAMCAL" type=uint16 offset=10589 min=2000 max=12000 m=1 b=0 units="dl" format=5.0 help="Fuel Pressure Control Start Offset HTML=350.HTML"
//ASAM mode=writevalue name="GDI Valve Max" parent="USERCAL_stRAMCAL" type=uint16 offset=10591 min=7000 max=12000 m=1 b=0 units="dl" format=5.0 help="Fuel Pressure Control Start Offset HTML=351.HTML"
//ASAM mode=writevalue name="GDI Valve FF" parent="USERCAL_stRAMCAL" type=uint16 offset=10593 min=0 max=10000 m=1 b=0 units="dl" format=4.0 help="Fuel Pressure Control Start Offset HTML=352.HTML"

//ASAM mode=writeaxis_pts name="Pedal Transfer Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10595 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="ISC Target X Axis HTML=353.HTML" xcount=17 xindexvar="PPSM Voltage"
//ASAM mode=writecurve name="Pedal Transfer Table" parent="USERCAL_stRAMCAL" type=uint16 offset=10663 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="ISC Target Curve HTML=354.HTML" xcount=17 xindexvar="PPSM Voltage"

//ASAM mode=writevalue name="Pressure Control Threshold" parent="USERCAL_stRAMCAL" type=uint16 offset=10697 min=0 max=10000 m=0.001 b=0 units="kPa" format=5.3 help="Pressure Control Threshold HTML=355.HTML"
//ASAM mode=writevalue name="Throttle Pressure Control Cal" parent="USERCAL_stRAMCAL" type=uint16 offset=10699 min=0 max=10000 m=1 b=0 units="dl" format=4.0 help="Throttle Pressure Control Cal HTML=356.HTML"
//ASAM mode=writevalue name="Pressure Control Hyst" parent="USERCAL_stRAMCAL" type=uint16 offset=10701 min=0 max=10000 m=0.001 b=0 units="kPa" format=5.3 help="Pressure Control Hyst HTML=357.HTML"

//ASAM mode=writevalue name="Boost Charge MAP Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=10703 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="Pressure Control Hyst HTML=358.HTML"
//ASAM mode=writevalue name="Boost Charge TPS Limit" parent="USERCAL_stRAMCAL" type=uint16 offset=10704 min=0 max=90 m=0.001 b=0 units="Degrees" format=4.3 help="Pressure Control Hyst HTML=359.HTML"


//ASAM mode=writevalue name="ISC Entry Ramp" parent="USERCAL_stRAMCAL" type=uint16 offset=10706 min=0 max=1 m=1 b=0 units="RPM" format=1.0 help="Pressure Control Hyst HTML=360.HTML"
//ASAM mode=writevalue name="ISC Post Start Ramp" parent="USERCAL_stRAMCAL" type=uint16 offset=10708 min=0 max=1 m=1 b=0 units="RPM" format=1.0 help="Pressure Control Hyst HTML=361.HTML"


//ASAM mode=writevalue name="Airflow FME Limit Low" parent="USERCAL_stRAMCAL" type=uint32 offset=10710 min=0 max=400 m=0.000001 b=0 units="g/s" format=4.3 help=Air Flow Meter calibration curve data points array HTML=362.HTML"
//ASAM mode=writevalue name="Airflow FME Limit High" parent="USERCAL_stRAMCAL" type=uint32 offset=10714 min=0 max=400 m=0.000001 b=0 units="g/s" format=4.3 help=Air Flow Meter calibration curve data points array HTML=363.HTML"

//ASAM mode=writevalue name="Torque Reduction Max Duration" parent="USERCAL_stRAMCAL" type=uint16 offset=10718 min=0 max=1 m=0.008 b=0 units="s" format=3.2 help="Pressure Control Hyst HTML=364.HTML"
//ASAM mode=writevalue name="Stall Saver RPM Threshold" parent="USERCAL_stRAMCAL" type=uint16 offset=10720 min=0 max=300 m=1 b=0 units="RPM" format=3.0 help="Pressure Control Hyst HTML=365.HTML"

//ASAM mode=writeaxis_pts name="ETC Scale Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10722 min=0 max=10000 m=1 b=0 units="RPM" format=4.0 help="XXXX X Axis HTML=366.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="ETC Scale Table" parent="USERCAL_stRAMCAL" type=uint16 offset=10790 min=0 max=101 m=1.6 b=0 units="V" format=4.1 help="XXX Curve HTML=367.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writevalue name="Torque Reduction Max Fuel Cut" parent="USERCAL_stRAMCAL" type=uint16 offset=10824 min=0 max=100 m=1 b=0 units="%" format=3.2 help="Pressure Control Hyst HTML=368.HTML"
//ASAM mode=writevalue name="Shift Up Time Limit" parent="USERCAL_stRAMCAL" type=uint16 offset=10826 min=0 max=1 m=0.008 b=0 units="s" format=3.2 help="Pressure Control Hyst HTML=369.HTML"

//ASAM mode=writevalue name="Boost Target 1" parent="USERCAL_stRAMCAL" type=uint16 offset=10828 min=0 max=200 m=0.01 b=0 units="kPa" format=4.2 help="Boost Gear 1 HTML=370.HTML"
//ASAM mode=writevalue name="Boost Target 2" parent="USERCAL_stRAMCAL" type=uint16 offset=10830 min=0 max=200 m=0.01 b=0 units="kPa" format=4.2 help="Boost Gear 2 HTML=371.HTML"
//ASAM mode=writevalue name="Boost Target 3" parent="USERCAL_stRAMCAL" type=uint16 offset=10832 min=0 max=200 m=0.01 b=0 units="kPa" format=4.2 help="Boost Gear 3 HTML=372.HTML"
//ASAM mode=writevalue name="Boost Target 4" parent="USERCAL_stRAMCAL" type=uint16 offset=10834 min=0 max=200 m=0.01 b=0 units="kPa" format=4.2 help="Boost Gear 4 HTML=373.HTML"
//ASAM mode=writevalue name="Boost Target 5" parent="USERCAL_stRAMCAL" type=uint16 offset=10836 min=0 max=200 m=0.01 b=0 units="kPa" format=4.2 help="Boost Gear 5 HTML=374.HTML"
//ASAM mode=writevalue name="Boost Target 6" parent="USERCAL_stRAMCAL" type=uint16 offset=10838 min=0 max=200 m=0.01 b=0 units="kPa" format=4.2 help="Boost Gear 6 HTML=375.HTML"

//ASAM mode=writevalue name="GDI Pressure Min" parent="USERCAL_stRAMCAL" type=uint16 offset=10840 min=0 max=25 m=1 b=0 units="kPa" format=4.2 help="Boost Gear 6 HTML=376.HTML"
//ASAM mode=writevalue name="GDI Pressure Max" parent="USERCAL_stRAMCAL" type=uint16 offset=10842 min=0 max=25 m=1 b=0 units="kPa" format=4.2 help="Boost Gear 6 HTML=377.HTML"
//ASAM mode=writevalue name="GDI MAP Min" parent="USERCAL_stRAMCAL" type=uint32 offset=10844 min=0 max=200 m=0.001 b=0 units="kPa" format=4.2 help="Boost Gear 6 HTML=378.HTML"
//ASAM mode=writevalue name="GDI MAP Max" parent="USERCAL_stRAMCAL" type=uint32 offset=10848 min=0 max=200 m=0.001 b=0 units="kPa" format=4.2 help="Boost Gear 6 HTML=379.HTML"

//ASAM mode=writevalue name="EST Pos Rate Max" parent="USERCAL_stRAMCAL" type=uint16 offset=10852 min=0 max=200 m=100 b=0 units="Deg/s" format=4.2 help="EST Pos Rate Max HTML=380.HTML"
//ASAM mode=writevalue name="EST Neg Rate Max" parent="USERCAL_stRAMCAL" type=uint16 offset=10854 min=0 max=200 m=100 b=0 units="Deg/s" format=4.2 help="EST Neg Rate Max HTML=381.HTML"

//ASAM mode=writevalue name="Pressure Control DPos" parent="USERCAL_stRAMCAL" type=uint16 offset=10856 min=0 max=1000 m=1 b=0 units="dl" format=4.2 help="EST Pos Rate Max HTML=382.HTML"
//ASAM mode=writevalue name="Pressure Control DNeg" parent="USERCAL_stRAMCAL" type=uint16 offset=10858 min=0 max=1000 m=1 b=0 units="dl" format=4.2 help="EST Pos Rate Max HTML=383.HTML"

//ASAM mode=writevalue name="Shift Count Pressure Control Limit" parent="USERCAL_stRAMCAL" type=uint16 offset=10860 min=0 max=1 m=0.008 b=0 units="s" format=4.3 help="EST Pos Rate Max HTML=384.HTML"
//ASAM mode=writevalue name="Shift Pressure Control" parent="USERCAL_stRAMCAL" type=uint16 offset=10862 min=0 max=100 m=1 b=0 units="%" format=4.2 help="EST Pos Rate Max HTML=385.HTML"

//ASAM mode=writevalue name="PWM 3D Slow 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10864 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D Slow 1 IO Resource HTML=386.HTML"
//ASAM mode=writevalue name="PWM 3D Slow 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10866 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D Slow 2 IO Resource HTML=387.HTML"
//ASAM mode=writevalue name="PWM 3D Slow 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10868 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D Slow 3 IO Resource HTML=388.HTML"
//ASAM mode=writevalue name="PWM 3D Slow 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=10870 min=0 max=255 units="ENUMERATION EH_IO_GPSE1=0 EH_IO_GPSE2=1 EH_IO_GPSE3=2 EH_IO_GPSE4=3 EH_IO_GPSE5=4 EH_IO_GPSE6=5 EH_IO_GPSE7=6 EH_IO_GPSE8=7 EH_IO_UART1_TX=15 EH_IO_UART1_RX=16 EH_IO_UART1_CTS=17 EH_IO_UART1_RTS=18 EH_IO_UART2_TX=19 EH_IO_UART2_RX=20 EH_IO_SPI1_MISO=21 EH_IO_SPI1_MOSI=22 EH_IO_SPI1_CLK=23 EH_IO_SPI1_CS=24 EH_IO_TMR1=25 EH_IO_TMR2=26 EH_IO_TMR3=27 EH_IO_TMR4=28 EH_IO_TMR5=29 EH_IO_TMR6=30 EH_IO_TMR7=31 EH_IO_TMR8=32 EH_IO_TMR9=33 EH_IO_TMR10=34 EH_IO_TMR11=35 EH_IO_TMR12=36 EH_IO_TMR13=37 EH_IO_TMR14=38 EH_IO_TMR15=39 EH_IO_TMR16=40 EH_IO_TMR17=41 EH_IO_TMR18=42 EH_IO_IIC1_SCL=43 EH_IO_IIC1_SDA=44 EH_IO_GP1=47 EH_IO_GP2=48 EH_IO_GP3=49 EH_IO_GP4=50 EH_IO_GP5=51 EH_IO_GP6=52 EH_IO_GP7=53 EH_IO_GP8=54 EH_IO_GP9=55 EH_IO_GP10=56 EH_IO_GP11=57 EH_IO_GP12=58 EH_IO_GP13=59 EH_IO_GP14=60 EH_IO_UNUSED=125" format=3.0 help="PWM 3D Slow 4 IO Resource HTML=389.HTML"

//ASAM mode=writevalue name="PWM 3D Slow 1 Period" parent="USERCAL_stRAMCAL" type=uint16 offset=10872 min=0 max=255 m=1 b=0 units="ms" format=2.0 help="PWM 3D Slow 1 Period HTML=390.HTML"
//ASAM mode=writevalue name="PWM 3D Slow 2 Period" parent="USERCAL_stRAMCAL" type=uint16 offset=10874 min=0 max=255 m=1 b=0 units="ms" format=2.0 help="PWM 3D Slow 2 Period HTML=391.HTML"
//ASAM mode=writevalue name="PWM 3D Slow 3 Period" parent="USERCAL_stRAMCAL" type=uint16 offset=10876 min=0 max=255 m=1 b=0 units="ms" format=2.0 help="PWM 3D Slow 3 Period HTML=392.HTML"
//ASAM mode=writevalue name="PWM 3D Slow 4 Period" parent="USERCAL_stRAMCAL" type=uint16 offset=10878 min=0 max=255 m=1 b=0 units="ms" format=2.0 help="PWM 3D Slow 4 Period HTML=393.HTML"

//ASAM mode=writevalue name="Sensor Hyst Limit RPM" parent="USERCAL_stRAMCAL" type=uint16 offset=10880 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="RPM Sensor Hyst Limit=394.HTML"

//ASAM mode=writevalue name="PPS Cal Min" parent="USERCAL_stRAMCAL" type=uint32 offset=10882 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="TPS voltage at min throttle position or neutral ETC position HTML=395.HTML"
//ASAM mode=writevalue name="PPS Cal Max" parent="USERCAL_stRAMCAL" type=uint32 offset=10886 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="TPS voltage at maximum throttle position HTML=396.HTML"

//ASAM mode=writeaxis_pts name="Pseudo Manifold Pressure Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10890 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="Pseudo Manifold Pressure MAP X Axis HTML=397.HTML" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Pseudo Manifold Pressure Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=10958 min=0 max=90 m=0.001 b=0 units="Degrees" format=3.1 help="Pseudo Manifold Pressure MAP Y Axis HTML=398.HTML" xcount=17 xindexvar="Throttle Angle"
//ASAM mode=writemap name="Pseudo Manifold Pressure Map" parent="USERCAL_stRAMCAL" type=uint32 offset=11026 min=0 max=101 m=0.001 b=0 units="kPa" format=4.1 help="Pseudo Manifold Pressure Map HTML=399.HTML" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="MAP kPa"

//ASAM mode=writevalue name="Pseudo Manifold Pressure Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=12182 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="Pseudo Manifold Pressure Enable HTML=400.HTML"

//ASAM mode=writevalue name="VSS Per 1k RPM G1" parent="USERCAL_stRAMCAL" type=uint16 offset=12183 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G1 HTML=401.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G2" parent="USERCAL_stRAMCAL" type=uint16 offset=12185 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G2 HTML=402.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G3" parent="USERCAL_stRAMCAL" type=uint16 offset=12187 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G3 HTML=403.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G4" parent="USERCAL_stRAMCAL" type=uint16 offset=12189 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G4 HTML=404.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G5" parent="USERCAL_stRAMCAL" type=uint16 offset=12191 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G5 HTML=405.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G6" parent="USERCAL_stRAMCAL" type=uint16 offset=12193 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G6 HTML=406.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G7" parent="USERCAL_stRAMCAL" type=uint16 offset=12195 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G7 HTML=407.HTML"
//ASAM mode=writevalue name="VSS Per 1k RPM G8" parent="USERCAL_stRAMCAL" type=uint16 offset=12197 min=0 max=6555 m=0.1 b=0 units="km/h" format=5.1 help="VSS Per 1000 RPM G8 HTML=408.HTML"

//ASAM mode=writevalue name="VSS CAN Calibration" parent="USERCAL_stRAMCAL" type=uint16 offset=12199 max=65535 m=1 b=0 units="dl" format=5.0 help="VSS CAN Calibration HTML=409.HTML"
//ASAM mode=writevalue name="ETC Override Keys" parent="USERCAL_stRAMCAL" type=uint16 offset=12201 min=0 max=65535 m=1 b=0 units="dl" format=5.0 help="ETC Override Keys HTML=410.HTML"
//ASAM mode=writevalue name="ETC Override" parent="USERCAL_stRAMCAL"typ e=uint16 offset=12203 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="ETC Override HTML=411.HTML"
//ASAM mode=writevalue name="Vehicle Model Diag" parent="USERCAL_stRAMCAL" type=uint16 offset=12205 min=0 max=255 units="ENUMERATION VAG_GTI_DSG_DQ250_Mk6=0 VAG_GTI_MT6_Mk6=1" format=3.0 help="ETC Override HTML=412.HTML"

//ASAM mode=writeaxis_pts name="ETC RPM Match Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=12207 min=0 max=10000 m=1 b=0 units="RPM" format=4.0 xcount=17 xindexvar="Rev Match RPM" help="TODO HTML=413.HTML"
//ASAM mode=writecurve name="ETC RPM Match Table" parent="USERCAL_stRAMCAL" type=uint16 offset=12275 min=0 max=15 m=1 b=0 units="counts" format=3.1 xcount=17 xindexvar="Rev Match RPM" help="TODO HTML=414.HTML"

//ASAM mode=writevalue name="Shift Down Time Limit" parent="USERCAL_stRAMCAL" type=uint16 offset=12309 min=0 max=5 m=0.008 b=0 units="s" format=3.2 help="Pressure Control Hyst HTML=415.HTML"
//ASAM mode=writevalue name="Shift Down Blip Limit" parent="USERCAL_stRAMCAL" type=uint16 offset=12311 min=0 max=5 m=0.008 b=0 units="s" format=3.2 help="Pressure Control Hyst HTML=416.HTML"

//ASAM mode=writevalue name="ATX Torque On VSS" parent="USERCAL_stRAMCAL" type=uint16 offset=12313 min=0 max=50 m=0.1 b=0 units="kph" format=3.2 help="Pressure Control Hyst HTML=417.HTML"
//ASAM mode=writevalue name="ATX Torque Off VSS" parent="USERCAL_stRAMCAL" type=uint16 offset=12315 min=0 max=50 m=0.1 b=0 units="kph" format=3.2 help="Pressure Control Hyst HTML=418.HTML"

//ASAM mode=writeaxis_pts name="ISC Open Loop Position Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=12317 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ISC Target X Axis HTML=419.HTML" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="ISC Open Loop Position Table" parent="USERCAL_stRAMCAL" type=uint16 offset=12385 min=0 max=100 m=0.4 b=0 units="%" format=4.3 help="ISC Target Curve HTML=420.HTML" xcount=17 xindexvar="Coolant Temperature"

//ASAM mode=writevalue name="Vehicle Stopped Fuel Cut Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=12419 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="Vehicle Stopped Fuel Cut Enable HTML=421.HTML"

//ASAM mode=writevalue name="CAL CRC" parent="USERCAL_stRAMCAL" type=uint16 offset=12420 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="CAL CRC16"
/* 	NOTE MUST ALWAYS INCLUDE CAL STRUCT ELEMENTS ONE FOR ONE AND IN ORDER */

/* Testing offsets table for ASAM parsing tool here */
#ifdef _USERCAL_C

#define OFFSETS_DATA \
{ \
	offsetof(USERCAL_tstCalibration, userCalADRefH),\
	offsetof(USERCAL_tstCalibration, userCalVRef445),\
	offsetof(USERCAL_tstCalibration, userCalVNernstTarget),\
	offsetof(USERCAL_tstCalibration, userCalRNernstReady),\
	offsetof(USERCAL_tstCalibration, userCalRHeaterThreshold),\
	offsetof(USERCAL_tstCalibration, au32UserCalPumpCurrentPFactor),\
	offsetof(USERCAL_tstCalibration, au32UserCalPumpCurrentMFactor),\
	offsetof(USERCAL_tstCalibration, ai32UserCalPumpCurrentOffset),\
	offsetof(USERCAL_tstCalibration, aUserCalPumpSpread),\
	offsetof(USERCAL_tstCalibration, aUserCalPumpLambda),\
	offsetof(USERCAL_tstCalibration, au8UserCalPumpSensorTrim),\
	offsetof(USERCAL_tstCalibration, aUserStoichFuelCal),\
	offsetof(USERCAL_tstCalibration, au8UserCalPumpDeadband),\
	offsetof(USERCAL_tstCalibration, au16UserCalFreeAirPoints),\
	offsetof(USERCAL_tstCalibration, aUserHeaterAlarmCurrent),\
	offsetof(USERCAL_tstCalibration, u8WarmUpDelay),\
	offsetof(USERCAL_tstCalibration, u8DisplayMode),\
	offsetof(USERCAL_tstCalibration, u8BackLightEnable),\
	offsetof(USERCAL_tstCalibration, userCalTPSCalMin),\
	offsetof(USERCAL_tstCalibration, userCalTPSCalMax),\
	offsetof(USERCAL_tstCalibration, aUserCURVEAfmTFSpread),\
	offsetof(USERCAL_tstCalibration, aUserCURVEAfmTFTable),\
	offsetof(USERCAL_tstCalibration, aUserInjResponseSpread),\
	offsetof(USERCAL_tstCalibration, aUserInjResponseTable),\
	offsetof(USERCAL_tstCalibration, aUserCoolantSensorSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantSensorTable),\
	offsetof(USERCAL_tstCalibration, aUserCoolantEnrichmentSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantEnrichmentTable),\
	offsetof(USERCAL_tstCalibration, aUserTAFRxSpread),\
	offsetof(USERCAL_tstCalibration, aUserTAFRySpread),\
	offsetof(USERCAL_tstCalibration, aUserMAPTAFR),\
	offsetof(USERCAL_tstCalibration, aUserTimingxSpread),\
	offsetof(USERCAL_tstCalibration, aUserTimingySpread),\
	offsetof(USERCAL_tstCalibration, aUserTimingMap),\
	offsetof(USERCAL_tstCalibration, aUserVExSpread),\
	offsetof(USERCAL_tstCalibration, aUserVEySpread),\
	offsetof(USERCAL_tstCalibration, aUserMAPVE),\
	offsetof(USERCAL_tstCalibration, aUserCoolantStartEnrichmentSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantStartEnrichmentTable),\
	offsetof(USERCAL_tstCalibration, aUserCoolantPostStartEnrichmentSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantPostStartEnrichmentTable),\
	offsetof(USERCAL_tstCalibration, aUserCrankingAirflowSpread),\
	offsetof(USERCAL_tstCalibration, aUserCrankingAirflowTable),\
	offsetof(USERCAL_tstCalibration, aUserDwellSpread),\
	offsetof(USERCAL_tstCalibration, aUserDwellTable),\
	offsetof(USERCAL_tstCalibration, aUserAirTempCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserAirTempCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserTPSMovementCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserTPSMovementCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserInjectionPhasingSpread),\
	offsetof(USERCAL_tstCalibration, aUserInjectionPhasingTable),\
	offsetof(USERCAL_tstCalibration, aUserCTSTimingCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserCTSTimingCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserATSTimingCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserATSTimingCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserISCSpeedTargetSpread),\
	offsetof(USERCAL_tstCalibration, aUserISCSpeedTargetTable),\
	offsetof(USERCAL_tstCalibration, aUserStepperCloseTable),\
	offsetof(USERCAL_tstCalibration, aUserStepperOpenTable),\
	offsetof(USERCAL_tstCalibration, u32UserStepperHomeSteps),\
	offsetof(USERCAL_tstCalibration, aUserPrimaryTriggerTable),\
	offsetof(USERCAL_tstCalibration, aUserSecondaryTriggerTable),\
	offsetof(USERCAL_tstCalibration, aUserSyncPointsTable),\
	offsetof(USERCAL_tstCalibration, u8UserPrimaryEdgeSetup),\
	offsetof(USERCAL_tstCalibration, u8UserSecondaryEdgeSetup),\
	offsetof(USERCAL_tstCalibration, u8UserPrimaryVREnable),\
	offsetof(USERCAL_tstCalibration, u8UserSecondaryVREnable),\
	offsetof(USERCAL_tstCalibration, u8UserFirstEdgeRisingPrimary),\
	offsetof(USERCAL_tstCalibration, u8UserFirstEdgeRisingSecondary),\
	offsetof(USERCAL_tstCalibration, au32InjectionSequence),\
	offsetof(USERCAL_tstCalibration, au32IgnitionSequence),\
	offsetof(USERCAL_tstCalibration, enSimpleCamSyncSource),\
	offsetof(USERCAL_tstCalibration, boCamSyncHighLate),\
	offsetof(USERCAL_tstCalibration, u32CamSyncSampleToothCount),\
	offsetof(USERCAL_tstCalibration, u16ManifoldVolumeCC),\
	offsetof(USERCAL_tstCalibration, u8CylCount),\
	offsetof(USERCAL_tstCalibration, u16InjFlowRate),\
	offsetof(USERCAL_tstCalibration, aUserBackupAirflowxSpread),\
	offsetof(USERCAL_tstCalibration, aUserBackupAirflowySpread),\
	offsetof(USERCAL_tstCalibration, aUserBackupAirflowMap),\
	offsetof(USERCAL_tstCalibration, u8ManifoldTimeConstantTrim),\
	offsetof(USERCAL_tstCalibration, u8EnableBackupAirflowTransients),\
	offsetof(USERCAL_tstCalibration, aUserPrimerSpread),\
	offsetof(USERCAL_tstCalibration, aUserPrimerTable),\
	offsetof(USERCAL_tstCalibration, aUserInjShortOpeningSpread),\
	offsetof(USERCAL_tstCalibration, aUserInjShortOpeningTable),\
	offsetof(USERCAL_tstCalibration, u32STTNegLimit),\
	offsetof(USERCAL_tstCalibration, u32STTPosLimit),\
	offsetof(USERCAL_tstCalibration, u32CLStepClosed),\
	offsetof(USERCAL_tstCalibration, u32CLIncClosed),\
	offsetof(USERCAL_tstCalibration, u32CLStepOpen),\
	offsetof(USERCAL_tstCalibration, u32CLIncOpen),\
	offsetof(USERCAL_tstCalibration, u16TimingMainOffset),\
	offsetof(USERCAL_tstCalibration, u32AFMTransientControlRPMLimit),\
	offsetof(USERCAL_tstCalibration, u8WastedSparkEnable),\
	offsetof(USERCAL_tstCalibration, u16AFRReinstate),\
	offsetof(USERCAL_tstCalibration, u16AFRCutoff),\
	offsetof(USERCAL_tstCalibration, u32TPSClosedLower),\
	offsetof(USERCAL_tstCalibration, u32TPSClosedUpper),\
	offsetof(USERCAL_tstCalibration, u32ISCESTTrimPos),\
	offsetof(USERCAL_tstCalibration, u32ISCESTTrimNeg),\
	offsetof(USERCAL_tstCalibration, u32CLO2ESTTrimPos),\
	offsetof(USERCAL_tstCalibration, u32CLO2ESTTrimNeg),\
	offsetof(USERCAL_tstCalibration, u16CylinderCC),\
	offsetof(USERCAL_tstCalibration, u8EnableAFMPrimaryInputOpen),\
	offsetof(USERCAL_tstCalibration, u32SpeedDensityAFMTPSLim),\
	offsetof(USERCAL_tstCalibration, au16SeqRPMLimit),\
	offsetof(USERCAL_tstCalibration, u16SeqRPMLimitHyst),\
	offsetof(USERCAL_tstCalibration, u16HighVacuumEnableKpa),\
	offsetof(USERCAL_tstCalibration, u16HighVacuumDisableKpa),\
	offsetof(USERCAL_tstCalibration, u16OverrunCutEnableRPM),\
	offsetof(USERCAL_tstCalibration, u16OverrunCutDisableRPM),\
	offsetof(USERCAL_tstCalibration, u16RPMRunThreshold),\
	offsetof(USERCAL_tstCalibration, u8SeqFuelAssign),\
	offsetof(USERCAL_tstCalibration, u16ESTFilterClosed),\
	offsetof(USERCAL_tstCalibration, u16ESTFilterOpen),\
	offsetof(USERCAL_tstCalibration, u16ESTCLGain),\
	offsetof(USERCAL_tstCalibration, u8EnableAFMPrimaryInputClosed),\
	offsetof(USERCAL_tstCalibration, s16SequenceFuelOffset),\
	offsetof(USERCAL_tstCalibration, u16SequenceFuelGain),\
	offsetof(USERCAL_tstCalibration, boTPSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boPPSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boCTSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boATSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boMAPCANPrimary),\
	offsetof(USERCAL_tstCalibration, au32PrioCANID),\
	offsetof(USERCAL_tstCalibration, au8SensorCANDataOffset),\
	offsetof(USERCAL_tstCalibration, au8SensorCANDataByteCount),\
	offsetof(USERCAL_tstCalibration, u32SyncPhaseRepeats),\
	offsetof(USERCAL_tstCalibration, s32MapSensorGain),\
	offsetof(USERCAL_tstCalibration, s32MapSensorOffset),\
	offsetof(USERCAL_tstCalibration, u8InjDivide),\
	offsetof(USERCAL_tstCalibration, u8CLO2Leftenable),\
	offsetof(USERCAL_tstCalibration, u8CLO2RightEnable),\
	offsetof(USERCAL_tstCalibration, u8ReturnlessEnable),\
	offsetof(USERCAL_tstCalibration, u16ReturnlessPressureKPa),\
	offsetof(USERCAL_tstCalibration, u8CrankingAirflowEnable),\
	offsetof(USERCAL_tstCalibration, u8StepperIACEnable),\
	offsetof(USERCAL_tstCalibration, u16IACStepsOpenHot),\
	offsetof(USERCAL_tstCalibration, u16IACStepsOpenCold),\
	offsetof(USERCAL_tstCalibration, u16IdleEntryOffset),\
	offsetof(USERCAL_tstCalibration, s32RadFanOnTemp),\
	offsetof(USERCAL_tstCalibration, s32RadFanOffTemp),\
	offsetof(USERCAL_tstCalibration, enFuelPumpRelay),\
	offsetof(USERCAL_tstCalibration, enESTBypass),\
	offsetof(USERCAL_tstCalibration, aUserAirSensorSpread),\
	offsetof(USERCAL_tstCalibration, aUserAirSensorTable),\
	offsetof(USERCAL_tstCalibration, aFuelIOResource),\
	offsetof(USERCAL_tstCalibration, aESTIOResource),\
	offsetof(USERCAL_tstCalibration, aIACIOResource),\
	offsetof(USERCAL_tstCalibration, boOBDISCADV),\
	offsetof(USERCAL_tstCalibration, u8TriggerType),\
	offsetof(USERCAL_tstCalibration, u8SyncType),\
	offsetof(USERCAL_tstCalibration, u8TriggerPullStrength),\
	offsetof(USERCAL_tstCalibration, u8SyncPullStrength),\
	offsetof(USERCAL_tstCalibration, u16MinLinearFuelPulse),\
	offsetof(USERCAL_tstCalibration, u16CTSADResource),\
	offsetof(USERCAL_tstCalibration, u16MAPADResource),\
	offsetof(USERCAL_tstCalibration, u16ATSADResource),\
	offsetof(USERCAL_tstCalibration, u16AFMADResource),\
	offsetof(USERCAL_tstCalibration, u16TPSADResource),\
	offsetof(USERCAL_tstCalibration, aUserTipInCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserTipInCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aLogicBlockVar),\
	offsetof(USERCAL_tstCalibration, aLogicBlockOperand),\
	offsetof(USERCAL_tstCalibration, aLogicBlockChainOutput),\
	offsetof(USERCAL_tstCalibration, enThermoFanRelay),\
	offsetof(USERCAL_tstCalibration, aESTIOMuxResource),\
	offsetof(USERCAL_tstCalibration, boUEGOCANPrimary),\
	offsetof(USERCAL_tstCalibration, aPWM2DAxesSourceXIDX),\
	offsetof(USERCAL_tstCalibration, aPWM3DAxesSourceXIDX),\
	offsetof(USERCAL_tstCalibration, aPWM3DAxesSourceYIDX),\
	offsetof(USERCAL_tstCalibration, aPWM2DIOOutputResource),\
	offsetof(USERCAL_tstCalibration, aPWM3DIOOutputResource),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_1SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_2SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_3SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_4SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_5SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_6SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_7SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_8SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_1Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_2Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_3Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_4Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_5Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_6Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_7Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_8Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_1SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_2SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_3SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_4SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_1SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_2SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_3SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_4SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_1Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_2Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_3Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_4Data),\
	offsetof(USERCAL_tstCalibration, u16FuelPressureSolenoidResource),\
	offsetof(USERCAL_tstCalibration, u8VCTABPullupEnable),\
	offsetof(USERCAL_tstCalibration, u8VCTCDPullupEnable),\
	offsetof(USERCAL_tstCalibration, aVVTInputResource),\
	offsetof(USERCAL_tstCalibration, aVVTInputType),\
	offsetof(USERCAL_tstCalibration, u16FRSADResource),\
	offsetof(USERCAL_tstCalibration, aUserFuelFlowRateSpread),\
	offsetof(USERCAL_tstCalibration, aUserFuelFlowRateTable),\
	offsetof(USERCAL_tstCalibration, u8VariableFuelPressureEnable),\
	offsetof(USERCAL_tstCalibration, s32FuelPressureSensorGain),\
	offsetof(USERCAL_tstCalibration, s32FuelPressureSensorOffset),\
	offsetof(USERCAL_tstCalibration, u16FuelPressurePGain),\
	offsetof(USERCAL_tstCalibration, u16FuelPressureIGain),\
	offsetof(USERCAL_tstCalibration, u8FuelPrimeEnable),\
	offsetof(USERCAL_tstCalibration, aUserTimingMapStage1),\
	offsetof(USERCAL_tstCalibration, aUserTorquePedalTransferSpread),\
	offsetof(USERCAL_tstCalibration, aUserTorquePedalTransferTable),\
	offsetof(USERCAL_tstCalibration, aUserPressureValveFeedForwardSpread),\
	offsetof(USERCAL_tstCalibration, aUserPressureValveFeedForwardTable),\
	offsetof(USERCAL_tstCalibration, u16AFMMaxCycleDeltaUgPos),\
    offsetof(USERCAL_tstCalibration, u16AFMMaxCycleDeltaUgNeg),\
    offsetof(USERCAL_tstCalibration, aUserPWM2DPeriod),\
    offsetof(USERCAL_tstCalibration, aUserPWM3DPeriod),\
    offsetof(USERCAL_tstCalibration, u8FuelPumpPrimerTime),\
    offsetof(USERCAL_tstCalibration, u8DBSlaveConfig),\
    offsetof(USERCAL_tstCalibration, u8DBSlaveOptions),\
    offsetof(USERCAL_tstCalibration, u16AFRADResource),\
	offsetof(USERCAL_tstCalibration, u16PPSMADResource),\
	offsetof(USERCAL_tstCalibration, u16PPSSADResource),\
	offsetof(USERCAL_tstCalibration, s32AFRADInputGain),\
	offsetof(USERCAL_tstCalibration, s32AFRADInputOffset),\
	offsetof(USERCAL_tstCalibration, u16AFRFuelTypeGain),\
	offsetof(USERCAL_tstCalibration, au16AuxInputResource),\
	offsetof(USERCAL_tstCalibration, au16AuxInputFunction),\
	offsetof(USERCAL_tstCalibration, aUserFuelCutsSpread),\
	offsetof(USERCAL_tstCalibration, aUserFuelCutsTable),\
	offsetof(USERCAL_tstCalibration, au16AuxInputLowThres),\
	offsetof(USERCAL_tstCalibration, au16AuxInputHighThres),\
	offsetof(USERCAL_tstCalibration, u16FuelPressureControlOffset),\
	offsetof(USERCAL_tstCalibration, u16FuelPressureControlStartOffset),\
	offsetof(USERCAL_tstCalibration, u16ISCColdISCMin),\
	offsetof(USERCAL_tstCalibration, u16ISCColdISCMax),\
	offsetof(USERCAL_tstCalibration, u16ISCHotISCMin),\
	offsetof(USERCAL_tstCalibration, u16ISCHotISCMax),\
	offsetof(USERCAL_tstCalibration, u16GDIValveMin),\
	offsetof(USERCAL_tstCalibration, u16GDIValveMax),\
	offsetof(USERCAL_tstCalibration, u16GDIValveFF),\
	offsetof(USERCAL_tstCalibration, aUserPedalTransferSpread),\
    offsetof(USERCAL_tstCalibration, aUserPedalTransferTable),\
    offsetof(USERCAL_tstCalibration, u16PressureControlThreshold),\
	offsetof(USERCAL_tstCalibration, u16ThrottlePressureControlCal),\
	offsetof(USERCAL_tstCalibration, u16PressureControlHyst),\
	offsetof(USERCAL_tstCalibration, u8BoostChargeMAPEnable),\
	offsetof(USERCAL_tstCalibration, u16BoostChargeTPSLimit),\
	offsetof(USERCAL_tstCalibration, u16ISCEntryRamp),\
	offsetof(USERCAL_tstCalibration, u16ISCPostStartRamp),\
	offsetof(USERCAL_tstCalibration, u32AirflowFMELimitLow),\
	offsetof(USERCAL_tstCalibration, u32AirflowFMELimitHigh),\
	offsetof(USERCAL_tstCalibration, u16TorqueReductionMaxDuration),\
	offsetof(USERCAL_tstCalibration, u16StallSaverRPMThreshold),\
	offsetof(USERCAL_tstCalibration, aUserETCScaleSpread[17]),\
	offsetof(USERCAL_tstCalibration, aUserETCScaleTable[17]),\
	offsetof(USERCAL_tstCalibration, u16TorqueReductionMaxFuelCut),\
	offsetof(USERCAL_tstCalibration, u16ShiftUpCountLimit),\
	offsetof(USERCAL_tstCalibration, au16BoostTarget),\
	offsetof(USERCAL_tstCalibration, u16GDIPressureMin),\
	offsetof(USERCAL_tstCalibration, u16GDIPressureMax),\
	offsetof(USERCAL_tstCalibration, u32GDIMAPMin),\
	offsetof(USERCAL_tstCalibration, u32GDIMAPMax),\
	offsetof(USERCAL_tstCalibration, u16ESTPosRateMax),\
	offsetof(USERCAL_tstCalibration, u16ESTNegRateMax),\
	offsetof(USERCAL_tstCalibration, u16PressureControlDPos),\
	offsetof(USERCAL_tstCalibration, u16PressureControlDNeg),\
	offsetof(USERCAL_tstCalibration, u16ShiftCountPressureControlLimit),\
	offsetof(USERCAL_tstCalibration, u16ShiftPressureControl),\
	offsetof(USERCAL_tstCalibration, aPWM3DSlowIOOutputResource),\
	offsetof(USERCAL_tstCalibration, aPWM3DSlowPeriod),\
	offsetof(USERCAL_tstCalibration, u16SensorHystLimitRPM),\
	offsetof(USERCAL_tstCalibration, userCalPPSCalMin),\
	offsetof(USERCAL_tstCalibration, userCalPPSCalMax),\
	offsetof(USERCAL_tstCalibration, aUserPseudoMAPxSpread),\
    offsetof(USERCAL_tstCalibration, aUserPseudoMAPySpread),\
    offsetof(USERCAL_tstCalibration, aUserPseudoMAPMap),\
	offsetof(USERCAL_tstCalibration, u8PseudoMAPEnable),\
	offsetof(USERCAL_tstCalibration, u16VSSPerRPM),\
	offsetof(USERCAL_tstCalibration, u16VSSCANCal),\
	offsetof(USERCAL_tstCalibration, u16ETCOverrideKeys),\
	offsetof(USERCAL_tstCalibration, u16ETCOverride),\
	offsetof(USERCAL_tstCalibration, u16DiagType),\
	offsetof(USERCAL_tstCalibration, aUserETCRPMMatchSpread),\
    offsetof(USERCAL_tstCalibration, aUserETCRPMMatchTable),\
	offsetof(USERCAL_tstCalibration, u16ShiftDownCountLimit),\
	offsetof(USERCAL_tstCalibration, u16ShiftDownBlipLimit),\
	offsetof(USERCAL_tstCalibration, u16ATXTorqueOnVSS),\
    offsetof(USERCAL_tstCalibration, u16ATXTorqueOffVSS),\
    offsetof(USERCAL_tstCalibration, aUserISCOpenLoopPosSpread),\
	offsetof(USERCAL_tstCalibration, aUserISCOpenLoopPosTable),\
    offsetof(USERCAL_tstCalibration, u8VehicleStoppedFuelCutEnable),\
	offsetof(USERCAL_tstCalibration, u16CRC16)}

const uint32 __attribute__((used)) au32Offsets[]=OFFSETS_DATA;
//ASAM mode=readvalue name="au32Offsets" type=uint32 offset=12422 min=0 max=65535 m=1 b=0 units="dl" format=8.0 help="Internal"
#endif

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void USERCAL_vStart(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vRun(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vTerminate(puint32 const pu32Arg);/*CR1_1*/


#endif // USERCAL_H

