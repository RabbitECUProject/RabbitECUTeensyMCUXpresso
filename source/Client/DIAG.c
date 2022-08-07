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

CQUEUE_tstQueue* DIAG_pstUARTQueue;
uint8* DIAG_pu8UARTBuffer;

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
* Implementation   : Serach for the index of a CID within a CID info table
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
	
	BOOSTED_vResetSVCResult();

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
	static uint8 u8UARTSeq;
	uint32 u32Temp;
	uint8 u8Temp;
	static uint8 u8UARTChecksum;
	static uint8 au8UARTInBuffer[4];

	USERDIAG_u32GlobalTimeTick++;

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
			stCANMsg.u32DWH = 0x01000000 + (((CAM_u32RPMFiltered * 4) % 0x100) << 8) + (CAM_u32RPMFiltered * 4) / 0x100;
			stCANMsg.u32DWL = 0;
			stCANMsg.u32DWH |= ((TORQUE_u32OutputTorqueModified & 0xff) << 16);

			DIAG_u8PedalPositionReport = TORQUE_u32TorquePedalEstimateScaled / 100;

			stCANMsg.u32DWL |= (TORQUE_u32OutputTorqueEstimate & 0xff);
			stCANMsg.u32DWL |= ((TORQUE_u32OutputTorqueEstimate & 0xff) << 24);
			stCANMsg.u32DWL |= ((TORQUE_u32IdleStabilisationTorque & 0xff) << 8);

			u32Temp = DIAG_u8PedalPositionReport;
			stCANMsg.u32DWL |= (u32Temp << 16);

			if (!TORQUE_u32PedalClosed)
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
		stCANMsg.u32DWH |= ((32 << 8) + 6);

		if (0 <= CTS_tTempCFiltered)
		{
			u32Temp = CTS_tTempCFiltered / 500;
			stCANMsg.u32DWH |= (u32Temp << 16);
		}

		if (TRUE == SENSORS_boBrakePedalPressed)
		{
			stCANMsg.u32DWH |= 0x0300;
		}

		stCANMsg.u32DWL = (76 << 16) + ((TORQUE_u32TorqueModelEstimateScaled & 0xff) << 8) +
				TORQUE_u32TorqueModelEstimateScaled / 0x100;

		USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/

		stCANMsg.u32ID = 1152;
		stCANMsg.u32DWH = USERDIAG_rau8Codes1152[u8Counter20] << 24;
		stCANMsg.u32DWH |= (44 << 16);
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
	else if (19 == (USERDIAG_u32GlobalTimeTick % 64))
	{
		u8Temp = 0x3f & USERCAL_stRAMCAL.u8DBSlaveConfig;
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp + u8UARTSeq);
		u8UARTChecksum += (u8Temp + u8UARTSeq);
		u8UARTSeq -= 0x40;
	}
	else if (27 == (USERDIAG_u32GlobalTimeTick % 64))
	{
		u8Temp = 0x3f & TORQUE_u16ETCScaleRamped;
		UARTHA_vSendChar(EH_VIO_UART1, u8Temp + u8UARTSeq);
		u8UARTChecksum += (u8Temp + u8UARTSeq);
		u8UARTSeq -= 0x40;
	}
	else if (35 == (USERDIAG_u32GlobalTimeTick % 64))
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

		UARTHA_vSendChar(EH_VIO_UART1, u8Temp + u8UARTSeq);
		u8UARTChecksum += (u8Temp + u8UARTSeq);
		u8UARTSeq -= 0x40;
	}
	else if (43 == (USERDIAG_u32GlobalTimeTick % 64))
	{
		UARTHA_vSendChar(EH_VIO_UART1, u8UARTSeq + (0x3f & u8UARTChecksum));
		u8UARTChecksum = 0;
		u8UARTSeq = 0xc0;
	}




	if (FALSE == CQUEUE_xIsEmpty(DIAG_pstUARTQueue))
	{
		uint8 u8Temp = DIAG_pu8UARTBuffer[DIAG_pstUARTQueue->u32Tail];
		CQUEUE_xRemoveItem(DIAG_pstUARTQueue);

		au8UARTInBuffer[3 - (u8Temp >> 6)] = u8Temp;

		if (0xc0 == (u8Temp & 0xc0))
		{
			u8Temp = au8UARTInBuffer[1];
			u8Temp += au8UARTInBuffer[2];
			u8Temp += au8UARTInBuffer[3];

			if (au8UARTInBuffer[1] == (u8Temp &  0x3f))
			{


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


