/******************************************************************************/
/*    Copyright (c) 2019 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Generic Data Code File                                 */
/* DESCRIPTION:        This code module contains generic data transforms      */
/*                                                                            */
/* FILE NAME:          gendata.c                                              */
/* REVISION HISTORY:   28-06-2019 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#define _GENDATA_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER
#include "GENDATA.h"
#include "AFM.h"
#include "MAP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/

SPREADAPI_ttSpreadIDX GENDATA_tSpreadPWM2DxIDX[GENDATA_nPWM2DTableCount];
SPREADAPI_ttSpreadIDX GENDATA_tSpreadPWM3DxIDX[GENDATA_nPWM3DTableCount];
SPREADAPI_ttSpreadIDX GENDATA_tSpreadPWM3DyIDX[GENDATA_nPWM3DTableCount];

TABLEAPI_ttTableIDX GENDATA_tTablePWMIDX[GENDATA_nPWM2DTableCount];
MAPSAPI_ttMapIDX GENDATA_tMapPWMIDX[GENDATA_nPWM3DTableCount];

TEPMAPI_tstTimedKernelEvent GENDATA_astTimedKernelEvents[1];


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void GENDATA_vStart(puint32 const pu32Arg)
{
	uint32 u32OutputIDX;
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;
	TEPMAPI_ttEventCount tEventCount = 1;
	
	/* Request and initialise required Kernel managed spread for PWM table 1 x */
	GENDATA_tSpreadPWM2DxIDX[0] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[0], (void*)&USERCAL_stRAMCAL.aUserPWM2D_1SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM table 2 x */
	GENDATA_tSpreadPWM2DxIDX[1] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[1], (void*)&USERCAL_stRAMCAL.aUserPWM2D_2SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 3 x */
	GENDATA_tSpreadPWM2DxIDX[2] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[2], (void*)&USERCAL_stRAMCAL.aUserPWM2D_3SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 4 x */
	GENDATA_tSpreadPWM2DxIDX[3] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[3], (void*)&USERCAL_stRAMCAL.aUserPWM2D_4SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 5 x */
	GENDATA_tSpreadPWM2DxIDX[4] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[4], (void*)&USERCAL_stRAMCAL.aUserPWM2D_5SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 6 x */
	GENDATA_tSpreadPWM2DxIDX[5] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[5], (void*)&USERCAL_stRAMCAL.aUserPWM2D_6SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 7 x */
	GENDATA_tSpreadPWM2DxIDX[6] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[6], (void*)&USERCAL_stRAMCAL.aUserPWM2D_7SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 8 x */
	GENDATA_tSpreadPWM2DxIDX[7] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[7], (void*)&USERCAL_stRAMCAL.aUserPWM2D_8SpreadX, TYPE_enInt32, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);


	/* 2D PWM maps data */
	/* Request and initialise required Kernel managed map for 2D PWM 1 */
	GENDATA_tTablePWMIDX[0] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_1Data, (void*)&GENDATA_u16PWMOutput[0], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[0], NULL);

	/* Request and initialise required Kernel managed map for 2D PWM 2 */
	GENDATA_tTablePWMIDX[1] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_2Data, (void*)&GENDATA_u16PWMOutput[1], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[1], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 3 */
	GENDATA_tTablePWMIDX[2] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_3Data, (void*)&GENDATA_u16PWMOutput[2], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[2], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 4 */
	GENDATA_tTablePWMIDX[3] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_4Data, (void*)&GENDATA_u16PWMOutput[3], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[3], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 5 */
	GENDATA_tTablePWMIDX[4] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_5Data, (void*)&GENDATA_u16PWMOutput[4], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[4], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 6 */
	GENDATA_tTablePWMIDX[5] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_6Data, (void*)&GENDATA_u16PWMOutput[5], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[5], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 7 */
	GENDATA_tTablePWMIDX[6] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_7Data, (void*)&GENDATA_u16PWMOutput[6], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[6], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 8 */
	GENDATA_tTablePWMIDX[7] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_8Data, (void*)&GENDATA_u16PWMOutput[7], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[7], NULL);
	

	/* 3D PWM maps X axes */
	/* Request and initialise required Kernel managed spread for PWM map 1 x */
	GENDATA_tSpreadPWM3DxIDX[0] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[8], (void*)&USERCAL_stRAMCAL.aUserPWM3D_1SpreadX, TYPE_enInt32, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 2 x */
	GENDATA_tSpreadPWM3DxIDX[1] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[9], (void*)&USERCAL_stRAMCAL.aUserPWM3D_2SpreadX, TYPE_enInt32, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 3 x */
	GENDATA_tSpreadPWM3DxIDX[2] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[10], (void*)&USERCAL_stRAMCAL.aUserPWM3D_3SpreadX, TYPE_enInt32, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 4 x */
	GENDATA_tSpreadPWM3DxIDX[3] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[11], (void*)&USERCAL_stRAMCAL.aUserPWM3D_4SpreadX, TYPE_enInt32, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);



	/* 3D PWM maps Y axes */
	/* Request and initialise required Kernel managed spread for PWM map 1 x */
	GENDATA_tSpreadPWM3DyIDX[0] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[12], (void*)&USERCAL_stRAMCAL.aUserPWM3D_1SpreadY, TYPE_enInt32, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 2 x */
	GENDATA_tSpreadPWM3DyIDX[1] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[13], (void*)&USERCAL_stRAMCAL.aUserPWM3D_2SpreadY, TYPE_enInt32, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 3 x */
	GENDATA_tSpreadPWM3DyIDX[2] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[14], (void*)&USERCAL_stRAMCAL.aUserPWM3D_3SpreadY, TYPE_enInt32, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 4 x */
	GENDATA_tSpreadPWM3DyIDX[3] = SETUP_tSetupSpread((void*)&GENDATA_s32SOURCE[15], (void*)&USERCAL_stRAMCAL.aUserPWM3D_4SpreadY, TYPE_enInt32, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);



	/* 3D PWM maps data */
	/* Request and initialise required Kernel managed map for 3D PWM 1 */
	GENDATA_tMapPWMIDX[0] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_1Data, (void*)&GENDATA_u16PWMOutput[8], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[0], GENDATA_tSpreadPWM3DyIDX[0], NULL);	
	
	/* Request and initialise required Kernel managed map for 3D PWM 2 */
	GENDATA_tMapPWMIDX[1] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_2Data, (void*)&GENDATA_u16PWMOutput[9], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[1], GENDATA_tSpreadPWM3DyIDX[1], NULL);
	
	/* Request and initialise required Kernel managed map for 3D PWM 3 */
	GENDATA_tMapPWMIDX[2] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_3Data, (void*)&GENDATA_u16PWMOutput[10], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[2], GENDATA_tSpreadPWM3DyIDX[2], NULL);
	
	/* Request and initialise required Kernel managed map for 3D PWM 4 */
	GENDATA_tMapPWMIDX[3] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_4Data, (void*)&GENDATA_u16PWMOutput[11], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[3], GENDATA_tSpreadPWM3DyIDX[3], NULL);


	for (u32OutputIDX = 0; u32OutputIDX < 8; u32OutputIDX++)
	{
		/* Request and initialise 2D PWM ***************************/
		if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM2DIOOutputResource[u32OutputIDX])
		{
			enEHIOResource = USERCAL_stRAMCAL.aPWM2DIOOutputResource[u32OutputIDX];
			enEHIOType = IOAPI_enPWM;
			USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,
				(void*)NULL, (void*)NULL);

			/* Initialise the TEPM channel PWM */
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{
				stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
				stTEPMChannelCB.boInterruptEnable = TRUE;

				USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
			}

			/* Switch out off at timer ms */
			GENDATA_astTimedKernelEvents[0].enAction = TEPMAPI_enSetLow;
			GENDATA_astTimedKernelEvents[0].enMethod = TEPMAPI_enHardLinkedTimeStep;
			GENDATA_astTimedKernelEvents[0].ptEventTime = &GENDATA_u32PWMOutput[u32OutputIDX];
			GENDATA_astTimedKernelEvents[0].tAccumulate = USERCAL_stRAMCAL.aUserPWM2DPeriod[u32OutputIDX];

			USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource,
				(void*)&GENDATA_astTimedKernelEvents[0], (void*)&tEventCount);
		}
	}

	for (u32OutputIDX = 0; u32OutputIDX < 4; u32OutputIDX++)
	{
		/* Request and initialise 2D PWM ***************************/
		if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM3DIOOutputResource[u32OutputIDX])
		{
			enEHIOResource = USERCAL_stRAMCAL.aPWM3DIOOutputResource[u32OutputIDX];
			enEHIOType = IOAPI_enPWM;
			USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,
				(void*)NULL, (void*)NULL);

			/* Initialise the TEPM channel PWM */
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{
				stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
				stTEPMChannelCB.boInterruptEnable = TRUE;

				USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
			}

			/* Switch out off at timer ms */
			GENDATA_astTimedKernelEvents[0].enAction = TEPMAPI_enSetLow;
			GENDATA_astTimedKernelEvents[0].enMethod = TEPMAPI_enHardLinkedTimeStep;
			GENDATA_astTimedKernelEvents[0].ptEventTime = &GENDATA_u32PWMOutput[u32OutputIDX + 8];
			GENDATA_astTimedKernelEvents[0].tAccumulate = USERCAL_stRAMCAL.aUserPWM3DPeriod[u32OutputIDX + 8];

			USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource,
				(void*)&GENDATA_astTimedKernelEvents[0], (void*)&tEventCount);
		}
	}

	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;

	if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[0])
	{
		enEHIOResource = USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[0];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}

	if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[1])
	{
		enEHIOResource = USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[1];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}

	if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[2])
	{
		enEHIOResource = USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[2];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}

	if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[3])
	{
		enEHIOResource = USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[3];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}
}

void GENDATA_vRun(puint32 const pu32Arg)
{
	static uint32 u32SampleCount = 0;
	static uint32 au32PWMSlowCount[4];
	static bool aboPWMOn[4];
	uint16 u16GenIDXX;
	uint16 u16GenIDXY;
	uint32 u32Temp;
	IOAPI_tenTriState enTriState;
	IOAPI_tenEHIOResource enEHIOResource;

	/* Buffer GENDATA_enBoostValve */
	GENDATA_s32VARS[GENDATA_enBoostValve] = MAP_u16PressureValveDuty;

	/* Buffer GENDATA_enThrottleAngle */
	GENDATA_s32VARS[GENDATA_enThrottleAngle] = TPS_tThetaFiltered;

	/* Buffer GENDATA_enCTS */
	GENDATA_s32VARS[GENDATA_enCTS] = CTS_tTempCFiltered;

	/* Buffer GENDATA_enEngineSpeed */
	GENDATA_s32VARS[GENDATA_enEngineSpeed] = CAM_u32RPMFiltered;


	u16GenIDXX = USERCAL_stRAMCAL.aPWM2DAxesSourceXIDX[u32SampleCount];
	GENDATA_s32SOURCE[u32SampleCount] = GENDATA_s32VARS[u16GenIDXX];

	if (0xff != u16GenIDXX)
	{
		/* Calculate the current generic spread */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&GENDATA_tSpreadPWM2DxIDX[u32SampleCount],
				NULL, NULL);

		/* Lookup the current generic table */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&GENDATA_tTablePWMIDX[u32SampleCount],
				NULL, NULL);

		GENDATA_u32PWMOutput[u32SampleCount] = GENDATA_u16PWMOutput[u32SampleCount];
	}

	u16GenIDXX = USERCAL_stRAMCAL.aPWM2DAxesSourceXIDX[u32SampleCount];
	GENDATA_s32SOURCE[u32SampleCount] = GENDATA_s32VARS[u16GenIDXX];

	if (0xff != u16GenIDXX)
	{
		/* Calculate the current generic spread */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&GENDATA_tSpreadPWM2DxIDX[u32SampleCount],
				NULL, NULL);

		/* Lookup the current generic table */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&GENDATA_tTablePWMIDX[u32SampleCount],
				NULL, NULL);

		GENDATA_u32PWMOutput[u32SampleCount] = GENDATA_u16PWMOutput[u32SampleCount];
	}


	u16GenIDXX = USERCAL_stRAMCAL.aPWM3DAxesSourceXIDX[u32SampleCount % 4];
	u16GenIDXY = USERCAL_stRAMCAL.aPWM3DAxesSourceYIDX[u32SampleCount % 4];

	GENDATA_s32SOURCE[8 + (u32SampleCount % 4)] = GENDATA_s32VARS[u16GenIDXX];
	GENDATA_s32SOURCE[12 + (u32SampleCount % 4)] = GENDATA_s32VARS[u16GenIDXY];

	if ((0xff != u16GenIDXX) && (0xff != u16GenIDXX))
	{
		/* Calculate the current generic spread X */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&GENDATA_tSpreadPWM3DxIDX[u32SampleCount % 4],
				NULL, NULL);

		/* Calculate the current generic spread Y */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&GENDATA_tSpreadPWM3DyIDX[u32SampleCount % 4],
				NULL, NULL);

		/* Lookup the current generic map */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&GENDATA_tMapPWMIDX[u32SampleCount % 4],
				NULL, NULL);


		GENDATA_u32PWMOutput[8 + (u32SampleCount % 4)] = GENDATA_u16PWMOutput[8 + (u32SampleCount % 4)];
	}

	u32SampleCount = 7 > u32SampleCount ? u32SampleCount + 1 : 0;

	for (u16GenIDXX = 0; u16GenIDXX < 4; u16GenIDXX++)
	{
		if (EH_IO_Invalid > USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[u16GenIDXX])
		{
			if ((USERCAL_stRAMCAL.aPWM3DSlowPeriod[u16GenIDXX] - 1) > au32PWMSlowCount[u16GenIDXX])
			{
				au32PWMSlowCount[u16GenIDXX]++;
				u32Temp = (GENDATA_u32PWMOutput[8 + u16GenIDXX] * USERCAL_stRAMCAL.aPWM3DSlowPeriod[u16GenIDXX]) / 0x10000;

				if ((u32Temp <= au32PWMSlowCount[u16GenIDXX]) && (TRUE == aboPWMOn[u16GenIDXX]))
				{
					enEHIOResource = USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[u16GenIDXX];
					enTriState = IOAPI_enLow;
					USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
					(void*)&enTriState,	(void*)NULL);
					aboPWMOn[u16GenIDXX] = FALSE;
				}
			}
			else
			{
				aboPWMOn[u16GenIDXX] = TRUE;
				au32PWMSlowCount[u16GenIDXX] = 0;
				enEHIOResource = USERCAL_stRAMCAL.aPWM3DSlowIOOutputResource[u16GenIDXX];
				enTriState = IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
		}
	}
}

void GENDATA_vTerminate(puint32 const pu32Arg)
{

}



#endif //BUILD_USER
