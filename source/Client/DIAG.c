/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Diagnostics                                            */
/* DESCRIPTION:        This code module initialises the required comms        */
/*                     resources for and provides functions for diagnostics   */
/* FILE NAME:          DIAG.c                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _USERDIAG_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#ifdef BUILD_USER

#include "diag.h"
#include "PERUART.h"
#include "gendata.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
const USERDIAG_tstIdentifierIB USERDIAG_rastIdentifierIB[] = USER_DIAG_nCIDInfo;
DIAGAPI_tenDiagSecurityLevel USERDIAG_enSecLevel;
puint8 USERDIAG_pu8Mode1Data;
puint8 USERDIAG_pu8Mode2Data;
puint8 USERDIAG_pu8Mode3Data;
puint8 USERDIAG_pu8Mode4Data;
puint8 USERDIAG_pu8Mode6Data;
puint8 USERDIAG_pu8Mode7Data;
uint16 USERDIAG_rau16Mode1DataOffsets[MODE1_PIDS_COUNT] = USERDIAG_nMode1PIDOffsets;
const uint8 USERDIAG_rau8Codes648[] = USERDIAG_nCodes648;
const uint8 USERDIAG_rau8Codes1152[] = USERDIAG_nCodes1152;

CQUEUE_tstQueue* volatile DIAG_pstUARTQueue;
uint8* DIAG_pu8UARTBuffer;
extern bool UDSAL_boCodePrepReq; // OK this is naughty - TBC
extern bool UDSAL_boCodeRunReq; // OK this is also naughty

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : DIAG_vNTOH
*
* Implementation   : Network to Host data endianness conversion
*
* Parameter
*    Par1          : Pointer to the data
*    Par2          : Datacount
*    Par3          : Datasize in bytes
*
* Return Value     : Nil
*******************************************************************************/
static void	DIAG_vNTOH(puint8 pu8Data, uint8 u8DataCount, uint8 u8DataSize);
static SPREADAPI_ttSpreadIDX USERDIAG_stGetSpreadIDX(puint8 pu8Data);

/* TODO
static void USERDIAG_vSnapShot(uint16);
*/

/*******************************************************************************
* Interface        : USERDIAG_i16GetCIDParamAddress
*
* Implementation   : Search for the index of a CID within a CID info table
*
* Parameter
*    Par1          : The CID being searched for
*
* Return Value     : Index of the found data, -1 for failed to find
*******************************************************************************/
static sint16 USERDIAG_i16GetCIDParamAddress(uint16);


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void USERDIAG_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOType enEHIOType;	
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tstPortConfigCB stPortConfigCB;	
	DIAGAPI_tenCallBackType enCallBackType;
	uint32 u32DiagSpreadsIDX;
	
	USERDIAG_u32GlobalTimeTick = 0;
	DIAG_u8EngagedGearReport = 255;
	DIAG_boCodeUpdateRequest = false;
	
	BOOSTED_vResetSVCResult();

	DIAG_vVersionMajor = BUILD_VER_MAJOR;
	DIAG_vVersionMinor = BUILD_VER_MINOR;

	for (u32DiagSpreadsIDX = 0; u32DiagSpreadsIDX < DIAG_nSPREADSRECORDSMAX; u32DiagSpreadsIDX++)
	{
		DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].pData = NULL;
		DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].tSpreadIDX = ~0;
	}

	for (u32DiagSpreadsIDX = 0; u32DiagSpreadsIDX < DIAG_nSPREADSMAX; u32DiagSpreadsIDX++)
	{
		DIAG_apu8SpreadTableAddresses[DIAG_nSPREADSMAX] = NULL;
	}
	
#ifdef USERDIAG_nEnableUART
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{		
		/* Setup the USERDIAG UART resources */
		enEHIOResource = EH_VIO_UART1;
		
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{	
			stPortConfigCB.enPortMode = IOAPI_enPortComms;
			stPortConfigCB.u32BaudRateHz = USERDIAG_nUARTFrequency;
			stPortConfigCB.enVIOResource = EH_VIO_UART1;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enRXPin = EH_IO_UART1_RX;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enTXPin = EH_IO_UART1_TX;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enRTSPin = EH_IO_Invalid;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enCTSPin = EH_IO_Invalid;		
			stPortConfigCB.enLLProtocol = PROTAPI_enLLNone;
			stPortConfigCB.enNLProtocol = PROTAPI_enNLNone;
			stPortConfigCB.enTLProtocol = PROTAPI_enTLNone;
			stPortConfigCB.pfRXTransferCB = NULL;
			stPortConfigCB.pfTXTransferCB = NULL;	
			enEHIOType = IOAPI_enUARTBus;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stPortConfigCB);	
			
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{	
				enCallBackType = DIAGAPI_enDataWriteCB;
				USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);			
			}
		}

		/* Let's not TX in interrupt mode */
		UARTHA_vDisableTXInterrupt(enEHIOResource);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			USER_vSVC(SYSAPI_enGetByteQueue, (void*)&enEHIOResource, NULL, NULL);

			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{
				DIAG_pstUARTQueue = (CQUEUE_tstQueue*)pstSVCDataStruct->pvArg1;
			}
		}

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			USER_vSVC(SYSAPI_enGetByteQueueBuffer, (void*)&enEHIOResource, NULL, NULL);

			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{
				DIAG_pu8UARTBuffer = (uint8*)((CQUEUE_tstQueue*)pstSVCDataStruct->pvArg1);
			}
		}
	}	
#endif
	
#ifdef USERDIAG_nEnableCAN	
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{		
		/* Setup the USERDIAG CAN resources */
		enEHIOResource = EH_VIO_CAN1;	
		
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{	
			stPortConfigCB.enPortMode = IOAPI_enPortComms;
			stPortConfigCB.u32BaudRateHz = USERDIAG_nCAN1Frequency;
			stPortConfigCB.enVIOResource = EH_VIO_CAN1;
			stPortConfigCB.stPinConfig.uPinInfo.stCANPinInfo.enRXPin = EH_IO_CAN1R;
			stPortConfigCB.stPinConfig.uPinInfo.stCANPinInfo.enTXPin = EH_IO_CAN1T;	
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32GlobalCANDiagAddress = 0x7df;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANDiagAddress = 0x7e0;			
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[0] = 80;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[1] = 1440;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[2] = 1088;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[3] = 428;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[0] = 0x7ff;				
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[1] = 0x7ff;	
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[2] = 0x7ff;	
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[3] = 0x7ff;				
			stPortConfigCB.enLLProtocol = PROTAPI_enLLCAN11;
			stPortConfigCB.enNLProtocol = PROTAPI_enISO15765;			
			stPortConfigCB.enTLProtocol = PROTAPI_enTLUDS;
			stPortConfigCB.pfRXTransferCB = NULL;
			stPortConfigCB.pfTXTransferCB = NULL;	
			enEHIOType = IOAPI_enCANBus;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stPortConfigCB);	
			
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{	
				enCallBackType = DIAGAPI_enDataWriteCB;
				USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);			
			}
		}
	}	
#endif


#ifdef USERDIAG_nEnableUSB
if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
{
	/* Setup the USERDIAG USB resources */
	enEHIOResource = EH_VIO_USB;

	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource, NULL, NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stPortConfigCB.enPortMode = IOAPI_enPortComms;
		stPortConfigCB.enVIOResource = EH_VIO_USB;
		stPortConfigCB.enLLProtocol = PROTAPI_enUSB;
		stPortConfigCB.enNLProtocol = PROTAPI_enISO15765;
		stPortConfigCB.enTLProtocol = PROTAPI_enTLUDS;
		stPortConfigCB.pfRXTransferCB = NULL;
		stPortConfigCB.pfTXTransferCB = NULL;
		enEHIOType = IOAPI_enUSBChannel;

		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stPortConfigCB);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			enCallBackType = DIAGAPI_enDataWriteCB;
			USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);
		}
	}
	
	USERDIAG_pu8Mode1Data = NULL;
	USERDIAG_pu8Mode2Data = NULL;
	USERDIAG_pu8Mode3Data = NULL;
	USERDIAG_pu8Mode4Data = NULL;
	USERDIAG_pu8Mode6Data = NULL;
	USERDIAG_pu8Mode7Data = NULL;

	USER_vSVC(SYSAPI_enGetMode1Buffer, NULL, NULL, NULL);
	USERDIAG_pu8Mode1Data = pstSVCDataStruct->pvArg1;

	if (NULL != USERDIAG_pu8Mode1Data)
	{
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1]] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1] + 1] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1] + 2] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1] + 3] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2]] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2] + 1] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2] + 2] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2] + 3] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3]] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3] + 1] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3] + 2] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3] + 3] = 0xff;
	}
}
#endif

#ifdef USERDIAG_nEnableENET
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{		
		/* Setup the USERDIAG ENET resources */
		enEHIOResource = EH_VIO_ENET_CH1;	
		
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{	
			stPortConfigCB.enPortMode = IOAPI_enPortComms;
			stPortConfigCB.enVIOResource = EH_VIO_ENET_CH1;
			stPortConfigCB.enLLProtocol = PROTAPI_enLL802_3;
			stPortConfigCB.enNLProtocol = PROTAPI_enISO15765_IP;			
			stPortConfigCB.enTLProtocol = PROTAPI_enTLUDS_UDP;
			stPortConfigCB.stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQSourcePort = USERDIAG_nRPCUDSREQSourcePort;
			stPortConfigCB.stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQDestPort = USERDIAG_nRPCUDSREQDestPort;		
			stPortConfigCB.pfRXTransferCB = NULL;
			stPortConfigCB.pfTXTransferCB = NULL;	
			enEHIOType = IOAPI_enENETChannel;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stPortConfigCB);	
			
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{		
				enCallBackType = DIAGAPI_enDataWriteCB;
				USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);
			}			
		}
	}
#endif
}

void USERDIAG_vRun(puint32 const pu32Arg)
{
	static uint32 u32SpreadIDX;
	SPREADAPI_ttSpreadIDX tSpreadIDX;
	SPREADAPI_tstSpreadResult* pstSpreadResult;
	PROTAPI_tstCANMsg stCANMsg;
	IOAPI_tenEHIOResource enEHIOResource;
	static uint8 u8Counter10;
	static uint8 u8Counter20;
	static uint8 u8Counter1000;
	uint32 u32Temp;
	uint16 u16Temp;
	uint8 u8Temp;
	static uint8 u8UARTTXChecksum;
	static uint8 au8UARTInBuffer[8];
	static uint8 u8UARTInBufferIDX = 0;
	bool boRXUart;
	uint32 u32FastRPM;
	puint8 pu8RAMData = (puint8)&USERCAL_stRAMCAL;
	uint16 u16UserCalDataCount = sizeof(USERCAL_stRAMCAL);
	uint32 u32StackInitVal;
	uint32* pu32Image;
	SCB_Type* pstSCB;
	tpfUpdate pfUpdate;

#if defined(DEBUG_DIAG)
	static uint32 debugRXCounts = 0;
#endif //DEBUG_DIAG

	USERDIAG_u32GlobalTimeTick++;

	if ((TRUE == UDSAL_boCodePrepReq) && (0 == CAM_u32RPMFiltered))
	{
		DIAG_boCodeUpdateRequest = true;
		USER_vSVC(SYSAPI_enSetupWorkingPage, (void*)&pu8RAMData,
					(void*)&u16UserCalDataCount,	(void*)NULL);
	}

	if ((TRUE == UDSAL_boCodeRunReq) && (0 == CAM_u32RPMFiltered))
	{
		if (1111 == USERCAL_stRAMCAL.u16ETCOverrideKeys)
		{
			pu32Image = (uint32*)USERDIAG_nCODE_MIN_ADDR;
			pstSCB = SCB;
			u32StackInitVal = *pu32Image++;
			pfUpdate = (tpfUpdate)(*pu32Image);

			if (((tpfUpdate*)USERDIAG_nCODE_MIN_ADDR < pfUpdate) &&
				((tpfUpdate*)USERDIAG_nCODE_MAX_ADDR > pfUpdate))
			{
				CPU_vEnterCritical();
				__set_MSP(u32StackInitVal);
				pstSCB->VTOR = (uint32)USERDIAG_nCODE_MIN_ADDR;
				pfUpdate = (tpfUpdate)(*pu32Image);
				pfUpdate();
			}
		}
	}

	if (DIAG_nSPREADSMAX > u32SpreadIDX)
	{
		if (NULL != DIAG_apu8SpreadTableAddresses[u32SpreadIDX])
		{
			tSpreadIDX = USERDIAG_stGetSpreadIDX(DIAG_apu8SpreadTableAddresses[u32SpreadIDX]);

			if (-1 != tSpreadIDX)
			{
				pstSpreadResult = BOOSTED_pstGetSpread(tSpreadIDX);
				DIAG_astSpreadResult[u32SpreadIDX].uSpreadData.stSpreadResult.u16SpreadIndex = pstSpreadResult->uSpreadData.stSpreadResult.u16SpreadIndex;
				DIAG_astSpreadResult[u32SpreadIDX].uSpreadData.stSpreadResult.u16SpreadOffset = pstSpreadResult->uSpreadData.stSpreadResult.u16SpreadOffset;
			}
			else
			{
				/* Just copy the raw uint32 data because this is not a spread */
				DIAG_astSpreadResult[u32SpreadIDX].uSpreadData.u32Data = *(uint32*)DIAG_apu8SpreadTableAddresses[u32SpreadIDX];
			}
		}

		u32SpreadIDX++;
	}
	else
	{
		u32SpreadIDX = 0;
	}

	if (NULL != USERDIAG_pu8Mode1Data)
	{
		switch (USERDIAG_u32GlobalTimeTick % 10)
		{
			case 0:
			{


				break;
			}


			case 1:
			{
				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_RPM]] = (4 * CAM_u32RPMRaw) / 0x100;
				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_RPM] + 1] = (4 * CAM_u32RPMRaw) & 0xff;

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_IAT]] = (uint8)(((sint32)40000 + ATS_tTempCFiltered) / 1000);
				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_ECT]] = (uint8)(((sint32)40000 + CTS_tTempCFiltered) / 1000);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_TPS]] = (uint8)(TPS_tThetaFiltered / 352);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_MAP]] = (uint8)(MAP_tKiloPaFiltered / 1000);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_IgnAdvance]] = (uint8)((EST_tIgnitionAdvanceMtheta / 500) + 128);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_CalcEngineLoad]] = (uint8)(((MAP_tKiloPaFiltered / 1000) * CAM_u32RPMRaw) / 10000);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_STT_Bank1]] = (uint8)(3 + (CLO2_u32STT[0] / 8));

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_STT_Bank2]] = (uint8)(3 + (CLO2_u32STT[1] / 8));

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_LTT_Bank1]] = 0x80;

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_LTT_Bank2]] = 0x80;

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_FuelPressure]] = (uint8)(USERCAL_stRAMCAL.u16ReturnlessPressureKPa / 33);

				break;
			}
		}
	}

	if ((2 == (USERDIAG_u32GlobalTimeTick % 10)) && (TRUE == DIAG_u8PowerModeActive))
	{
		/* 10 millisecond group */
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 640;

		if (400 > CAM_u32RPMFiltered)
		{
			stCANMsg.u32DWH = 0x01000000;
			stCANMsg.u32DWL = 0;

			u32Temp = TPS_tThetaFiltered / 300;
			stCANMsg.u32DWL |= u32Temp;
			stCANMsg.u32DWL |= (u32Temp << 24);
			u32Temp = TPS_tThetaFiltered / 251;
			stCANMsg.u32DWL |= (u32Temp << 8);

			if (6000 < TPS_tThetaFiltered)
			{
				u32Temp = (TPS_tThetaFiltered - 6000) / 100;
				stCANMsg.u32DWL |= (u32Temp << 16);
				stCANMsg.u32DWH &= 0x00ffffff;
			}

			DIAG_u8PedalPositionReport = (TPS_tThetaFiltered - 6000) / 100;
		}
		else
		{
			u32FastRPM = *CAM_pu32FastRPMRaw;
			stCANMsg.u32DWH = 0x01000000 + (((u32FastRPM) % 0x100) << 8) + (u32FastRPM) / 0x100;
			stCANMsg.u32DWL = 0;
			stCANMsg.u32DWH |= ((TORQUE_u32OutputTorqueModified & 0xff) << 16);

			if (TPS_boThrottleClosed > 0)
			{
				DIAG_u8PedalPositionReport = 0;
			}
			else
			{
				DIAG_u8PedalPositionReport = TORQUE_u32TorquePedalEstimateScaled / 100;
			}

			stCANMsg.u32DWL |= (TORQUE_u32OutputTorqueEstimate & 0xff);
			stCANMsg.u32DWL |= ((TORQUE_u32OutputTorqueEstimate & 0xff) << 24);
			stCANMsg.u32DWL |= ((TORQUE_u32IdleStabilisationTorque & 0xff) << 8);

			u32Temp = DIAG_u8PedalPositionReport;
			stCANMsg.u32DWL |= (u32Temp << 16);

			if (!TPS_boThrottleClosed)
			{
				stCANMsg.u32DWH &= 0x00ffffff;
			}
		}

		enEHIOResource = EH_VIO_CAN1;
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 896;
		stCANMsg.u32DWL = 0x0000fe00;
		stCANMsg.u32DWH = 0x20830000;
		stCANMsg.u32DWH |= (DIAG_u8PedalPositionReport << 8);

		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 1160;

		if (28800 < MAP_tKiloPaFiltered)
		{
			if (174500 > MAP_tKiloPaFiltered)
			{
				u32Temp = ((MAP_tKiloPaFiltered - 28800) / 573) << 8;
				u32Temp = u32Temp | ((u32Temp) << 8);
			}
			else
			{
				u32Temp = 0xfefe00;
			}
		}
		else
		{
			u32Temp = 0;
		}

		u32Temp |= ((u8Counter10 << 28) + 126);
		stCANMsg.u32DWH = u32Temp;

		stCANMsg.u32DWL = 0xfeffff00;
		USER_DIAG_APPEND_XOR();
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		u8Counter10 = (u8Counter10 + 1) % 0x10;
	}

	if ((6 == (USERDIAG_u32GlobalTimeTick % 20)) && (TRUE == DIAG_u8PowerModeActive))
	{
		/* 20 millisecond group */
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 866;
		stCANMsg.u32DWH = (u8Counter20 << 24) + 166;
		stCANMsg.u32DWL = 0xfe000000;
		enEHIOResource = EH_VIO_CAN1;

		if (8 <= u8Counter20)
		{
			stCANMsg.u32DWL += (u8Counter20 + 72);
		}
		else
		{
			stCANMsg.u32DWL += (u8Counter20 + 88);
		}

		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u32ID = 644;
		stCANMsg.u8DLC = 6;
		stCANMsg.u32DWL = 0;
		stCANMsg.u32DWH = (u8Counter20 << 24) + (u8Counter20 << 16);
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 648;

		stCANMsg.u32DWH = USERDIAG_rau8Codes648[u8Counter20] << 24;
		stCANMsg.u32DWH |= 6;

		if (0 <= CTS_tTempCFiltered)
		{
			u32Temp = CTS_tTempCFiltered / 500;
			stCANMsg.u32DWH |= (u32Temp << 16);
		}

		u16Temp = 2048 > (SENSORS_u16CANVSS / 8) ? SENSORS_u16CANVSS / 8 : 255;
		stCANMsg.u32DWH |= u16Temp;

		if (TRUE == SENSORS_boBrakePedalPressed)
		{
			stCANMsg.u32DWH |= 0x0300;
		}

		stCANMsg.u32DWL = ((IAC_u16ISCTargetRamp / 10) << 16) + (TORQUE_u32TorqueEstimateScale & 0xff) +
				((TORQUE_u32TorqueEstimateScale / 0x100) << 8);

		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u32ID = 1152;
		stCANMsg.u32DWH = USERDIAG_rau8Codes1152[u8Counter20] << 24;

		stCANMsg.u32DWH += (FUEL_u32FuelConsumed & 0xff) << 8;
		stCANMsg.u32DWH += (FUEL_u32FuelConsumed & 0xff00) >> 8;

		stCANMsg.u32DWL = (24 << 24) + (4 << 8);
		USER_DIAG_APPEND_XOR();
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u32ID = 1162;
		stCANMsg.u32DWL = u8Counter20 << 4;
		stCANMsg.u32DWH = (u8Counter20 << 28) + (2 << 24) + 2;
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u32ID = 1416;
		stCANMsg.u32DWH = (232 << 24) + (60 << 16) + (127 << 8) + 135;
		stCANMsg.u32DWL = (52 << 24) + 153;
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		u8Counter20 = (u8Counter20 + 1) % 0x10;
	}

	if ((8 == (USERDIAG_u32GlobalTimeTick % 1000)) && (TRUE == DIAG_u8PowerModeActive))
	{
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 1408;
		stCANMsg.u32DWH = u8Counter1000 << 24;
		stCANMsg.u32DWL = 0;
		enEHIOResource = EH_VIO_CAN1;
		u8Counter1000 = (u8Counter1000 + 1) % 0x10;
		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/
	}



	/* UART data */
	if (1 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		UARTHA_vSendChar(EH_VIO_UART1, 0xaa);
		u8UARTTXChecksum = 0xaa;
	}
	else if (2 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp =  0x7f & USERCAL_stRAMCAL.u8DBSlaveConfig;
		u8Temp |= 0x40;
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (3 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)TORQUE_u16ETCScaleRamped;
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (4 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		switch (USERCAL_stRAMCAL.u16DiagType)
		{
			case 0:
			{
				if (0x1234 == USERCAL_stRAMCAL.u16ETCOverrideKeys)
				{
					u8Temp = (uint8)USERCAL_stRAMCAL.u16ETCOverride;
				}
				else if ((0 != TORQUE_u16GearShiftCount) && (TRUE == TORQUE_boDownShift) && (TRUE == TORQUE_boVehicleMovingDS))
				{
					u8Temp = TORQUE_u16RevMatchPosition & 0x1f;
				}
				else
				{
					if ((FALSE == MAP_boBoostETCCutEnable) || (2000 > CAM_u32RPMFiltered))
					{
						/* 0x10 <= IAC_u8SlaveTarget < 0x20 */
						u8Temp = 0x3f & IAC_u8SlaveTarget;
					}
					else
					{
						/* 0x20 <= MAP_u8PressureThrottleTrim < 0x3f */
						u8Temp = MAP_u8PressureThrottleTrim;
					}
				}

				break;
			}
			case 1:
			{
				if (0 != CAM_u32RPMFiltered)
				{
					u8Temp = 0x3f & IAC_u8SlaveTarget;
				}
				else
				{
					/* ISC neutral point */
					u8Temp = 0x18;
				}

				break;
			}
			default:
			{
				u8Temp = 0;
				break;
			}
		}

		DIAG_u16ISCCmd = u8Temp;
		u8Temp |= 0x40;
		UARTHA_vSendChar(EH_VIO_UART1, 0x7f & u8Temp);
		u8UARTTXChecksum += (0x7f & u8Temp);
	}
	else if (5 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[0];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (6 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f &  (uint8)GENDATA_u16PWMOutput[1];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (7 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[2];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (8 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[3];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (9 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[4];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (10 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[5];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (11 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[6];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (12 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0x7f & (uint8)GENDATA_u16PWMOutput[7];
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (13 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		u8Temp = 0;
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp);
		u8UARTTXChecksum += u8Temp;
	}
	else if (14 == (USERDIAG_u32GlobalTimeTick % 16))
	{
		UARTHA_vSendChar(EH_VIO_UART1, 0x7f & u8UARTTXChecksum);
	}


	if (FALSE == CQUEUE_xIsEmpty(DIAG_pstUARTQueue))
	{
		boRXUart = FALSE;

		while (FALSE == boRXUart)
		{
			CPU_xEnterCritical();
			u8Temp = DIAG_pu8UARTBuffer[DIAG_pstUARTQueue->u32Head];
			CQUEUE_xRemoveItemUnsafe(DIAG_pstUARTQueue);
			CPU_xExitCritical();

			if (0 != (u8Temp & 0x80))
			{
				u8UARTInBufferIDX = 0;
				au8UARTInBuffer[1] = 0xff;
				au8UARTInBuffer[2] = 0xff;
				au8UARTInBuffer[3] = 0xff;
				au8UARTInBuffer[4] = 0xff;
				au8UARTInBuffer[5] = 0xff;
				au8UARTInBuffer[6] = 0xff;
				au8UARTInBuffer[7] = 0xff;
			}

			au8UARTInBuffer[u8UARTInBufferIDX % 8] = u8Temp;
			boRXUart = CQUEUE_xIsEmpty(DIAG_pstUARTQueue);
			u8UARTInBufferIDX++;
		}

		if (4 <= u8UARTInBufferIDX)
		{
			if ((0x7f & (au8UARTInBuffer[0] + au8UARTInBuffer[1] + au8UARTInBuffer[2])) == au8UARTInBuffer[3])
			{
				switch (au8UARTInBuffer[0] & 0xf0)
				{
					case 0x80:
					{
						SENSORS_au16ADCImport[0] = ((au8UARTInBuffer[0] & 0x0f) + ((uint16)au8UARTInBuffer[1] << 4)) << 1;

						if (SENSORS_au16ADCImport[0] < 200)
						{
							SENSORS_aboADCNew[0] = FALSE;
						}
						else
						{
							SENSORS_aboADCNew[0] = TRUE;
						}

						SENSORS_au16ADCImport[1] = au8UARTInBuffer[2];

						break;
					}
					default:
					{
						break;
					}
				}
			}
			else
			{
#if defined(DEBUG_DIAG)
				debugRXCounts++;
#endif //DEBUG_DIAG
			}
		}
	}
}

/* TODO
static void USERDIAG_vSnapShot(uint16 u16FreezeDTC)
{
	memcpy(USERDIAG_pu8Mode2Data, USERDIAG_pu8Mode1Data, USERDIAG_nMode1BufferSize);
	USERDIAG_pu8Mode2Data[USERDIAG_rau16Mode1DataOffsets[MODE1_FreezeDTC]] = u16FreezeDTC / 0x100;
	USERDIAG_pu8Mode2Data[USERDIAG_rau16Mode1DataOffsets[MODE1_FreezeDTC] + 1] = u16FreezeDTC / 0x100;
}
*/

static SPREADAPI_ttSpreadIDX USERDIAG_stGetSpreadIDX(puint8 pu8Data)
{
	SPREADAPI_ttSpreadIDX tSpreadIDX = -1;
	uint32 u32AddressPairIDX;

	for (u32AddressPairIDX = 0; u32AddressPairIDX < DIAG_nSPREADSRECORDSMAX; u32AddressPairIDX++)
	{
		if (pu8Data == DIAG_astIDXAddressPairs[u32AddressPairIDX].pData)
		{
			tSpreadIDX = DIAG_astIDXAddressPairs[u32AddressPairIDX].tSpreadIDX;
			break;
		}
	}

	return tSpreadIDX;
}

void USERDIAG_vAddIDXAddressPair(USERDIAG_tstSpreadIDXAddressPair* pstSpreadIDXAddressPair)
{
    uint32 u32DiagSpreadsIDX;

	for (u32DiagSpreadsIDX = 0; u32DiagSpreadsIDX < DIAG_nSPREADSRECORDSMAX; u32DiagSpreadsIDX++)
	{
		if (NULL == DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].pData)
		{
			DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].pData = pstSpreadIDXAddressPair->pData;
			DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].tSpreadIDX = pstSpreadIDXAddressPair->tSpreadIDX;
			break;
		}
	}
}


void USERDIAG_vTerminate(puint32 const pu32Arg)
{

}

void USERDIAG_vCallBack(uint32* const pu32Arg)
{

}

DIAGAPI_tenCBResult USERDIAG_boReadDataCallBack(uint16 u16CID, puint8 pu8Data, uint8 u8DataCount)
{
	return DIAGAPI_enCBOK;/*CR1_25*/
}

DIAGAPI_tenCBResult USERDIAG_boWriteDataCallBack(uint16 u16CID, puint8 pu8Data, uint8 u8DataCount)
{
	sint16 i16CIDIDX;
	DIAGAPI_tenCBResult enCBResult = DIAGAPI_enRWGenFail;
	
	i16CIDIDX = USERDIAG_i16GetCIDParamAddress(u16CID);
	
	if (-1 != i16CIDIDX)
	{
		if (USERDIAG_enSecLevel >= USERDIAG_rastIdentifierIB[i16CIDIDX].enSecLevel)
		{
			DIAG_vNTOH(pu8Data, u8DataCount, USERDIAG_rastIdentifierIB[i16CIDIDX].u8DataSize);
			memcpy((void*)USERDIAG_rastIdentifierIB[i16CIDIDX].pu8Data, 
					(void*)pu8Data, USERDIAG_rastIdentifierIB[i16CIDIDX].u8DataCount);
			enCBResult = DIAGAPI_enCBOK;
		}
		else
		{
			enCBResult = DIAGAPI_enRWSecurityError;
		}
	}
	else
	{
		enCBResult = DIAGAPI_enRWParamNotFound;
	}
	
	return enCBResult;
}

DIAGAPI_tenCBResult USERDIAG_boRoutineControl(DIAGAPI_tenRoutineAction enRoutineAction, puint8 pu8Data, uint8 u8DataCount)
{
	return DIAGAPI_enCBOK;/*CR1_25*/
}

static sint16 USERDIAG_i16GetCIDParamAddress(uint16 u16CID)
{
	uint16 u16IDX;
	sint16 i16CIDIDX = -1;
	
	for (u16IDX = 0; 
		u16IDX < (sizeof(USERDIAG_rastIdentifierIB) / sizeof(USERDIAG_tstIdentifierIB));
		u16IDX++)
	{
		if (u16CID == USERDIAG_rastIdentifierIB[u16IDX].u16CID)
		{
			i16CIDIDX = u16IDX;
			break;			
		}	
	}
	
	return i16CIDIDX;
}

static void	DIAG_vNTOH(puint8 pu8Data, uint8 u8DataCount, uint8 u8DataSize)
{
	uint32 u32Temp;
	uint16 u16Temp;
	
	if ((4 > u8DataSize) && (0 == (u8DataCount % u8DataSize)))/*CR1_28*//*CR1_29*/
	{
		while (0 < u8DataCount)
		{
			switch ((u8DataCount & 0x03) % u8DataSize)/*CR1_27*/
			{
				case 0:
				{
					memcpy((void*)&u32Temp, (void*)pu8Data, 4);
					*pu8Data++ = (uint8)((u32Temp & 0xff000000) >> 24);
					*pu8Data++ = (uint8)((u32Temp & 0xff0000) >> 16);				
					*pu8Data++ = (uint8)((u32Temp & 0xff00) >> 8);
					*pu8Data++ = (uint8)((u32Temp & 0xff) >> 0);	
					u8DataCount -= 4;
					break;
				}
				case 2:
				{
					memcpy((void*)&u16Temp, (void*)pu8Data, 2);
					*pu8Data++ = (uint8)((u16Temp & 0xff00) >> 8);
					*pu8Data++ = (uint8)((u16Temp & 0xff) >> 0);				
					u8DataCount -= 2;
					break;
				}	
				case 1:
				{
					u8DataCount = 0;
				}
			}	
		}	
	}
}

#endif //BUILD_USER


