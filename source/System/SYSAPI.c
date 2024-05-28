/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      SYSAPI                                                 */
/* DESCRIPTION:        This code provides functions for running system calls  */
/*                                                                            */
/* FILE NAME:          SYSAPI.c                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <SYS.h>
#ifdef KERNEL

#include "SYSAPI.h"
#include "CEM.h"
#include "CEMAPI.h"
#include "DLL.h"
#include "PROTAPI.h"
#include "IOAPI.h"
#include "DACAPI.h"
#include <TYPES.h>
#include "MSG.h"
#include "PERADC.h"
#include "PERWDT.h"
#include "DAC.h"
#include <CLIENT.h>
#include "os.h"
#include "RESM.h"
#include "TEPM.h"
#include "ADCAPI.h"
#include "DACAPI.h"
#include "MAPSAPI.h"
#include <CTRLAPI.h>
#include "SPREADSAPI.h"
#include "TABLESAPI.h"
#include "DIAGAPI.h"
#include "CQUEUE.h"
#include "SRLTFR.h"
#include "TEPM.h"
#include "CTRL.h"
#include <SPREADS.h>
#include <TABLES.h>
#include "PERUART.h"
#include "FEE.h"
#include "KERNELDIAG.h"
#include "MAPS.h"

extern uint8 UDSAL_au8Mode1DataBuffer[];
extern uint8 UDSAL_au8Mode2DataBuffer[];
extern uint8 UDSAL_au8Mode3DataBuffer[];
extern uint8 UDSAL_au8Mode4DataBuffer[];

void SYS_vAPISVC(void)
{
	static uint32 u32Temp;
	static puint16 pu16Temp;
	static CTRLAPI_ttPIDIDX PIDIDX;
	static SPREADAPI_ttSpreadIDX tSpreadIDX;	
	static TABLEAPI_ttTableIDX tTableIDX;	
	static MAPSAPI_ttMapIDX tMapIDX;		
	static bool boResult;

	
	switch (OS_stSVCDataStruct.enSVCID)
	{
		case SYSAPI_enSetClientStartTask:
		{	
			OS_stSVCDataStruct.enSVCResult =			
			CLIENT_vAddTask(OS_enCLIENTQueueStart, *(SYSAPI_tpfUserTaskFunction*)OS_stSVCDataStruct.pvArg1, *(TASKAPI_tenPriority*)OS_stSVCDataStruct.pvArg2, *(TASKAPI_tenRateMs*)OS_stSVCDataStruct.pvArg3);
			break;
		}
		
		case SYSAPI_enSetClientCyclicTask:
		{	
			OS_stSVCDataStruct.enSVCResult =			
			CLIENT_vAddTask(OS_enCLIENTQueueCyclic, *(SYSAPI_tpfUserTaskFunction*)OS_stSVCDataStruct.pvArg1, *(TASKAPI_tenPriority*)OS_stSVCDataStruct.pvArg2, *(TASKAPI_tenRateMs*)OS_stSVCDataStruct.pvArg3);
			break;
		}		
		
		case SYSAPI_enSetClientTerminateTask:
		{	
			OS_stSVCDataStruct.enSVCResult =
			CLIENT_vAddTask(OS_enCLIENTQueueTerminate, *(SYSAPI_tpfUserTaskFunction*)OS_stSVCDataStruct.pvArg1, *(TASKAPI_tenPriority*)OS_stSVCDataStruct.pvArg2, *(TASKAPI_tenRateMs*)OS_stSVCDataStruct.pvArg3);
			break;
		}		

		case SYSAPI_enRequestIOResource:
		{						
			u32Temp = (SYSAPI_ttClientHandle)RESM_u32RequestEHIOResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, OS_stSVCDataStruct.tClientHandle);
			OS_stSVCDataStruct.enSVCResult = (u32Temp == OS_stSVCDataStruct.tClientHandle) ? SYSAPI_enOK : SYSAPI_enFail;
			break;
		}		
		
		case SYSAPI_enInitialiseIOResource:
		{				
			if ((IOAPI_enDIOOutput == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2) || (IOAPI_enDIOOutput == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2))
			{
				IO_vInitDIOResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2, *(IOAPI_tenDriveStrength*)OS_stSVCDataStruct.pvArg3);
			}
			
			else if ((IOAPI_enDIOInput == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2) || (IOAPI_enDIOInput == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2))
			{
				IO_vInitDIOResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2, *(IOAPI_tenDriveStrength*)OS_stSVCDataStruct.pvArg3);
			}

			else if ((IOAPI_enGPSE == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2) || (IOAPI_enADD == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2))
			{
				IO_vInitADCResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2, (ADCAPI_tstADCCB*)OS_stSVCDataStruct.pvArg3);
			}	
			
			else if (IOAPI_enDAC == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)
			{
				IO_vInitDACResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2, (DACAPI_tstDACCB*)OS_stSVCDataStruct.pvArg3);
			}				
			
			else if ((IOAPI_enIICBus == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)
								|| (IOAPI_enUARTBus == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2) 
								|| (IOAPI_enSPIBus == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2) 
								|| (IOAPI_enCANBus == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)	
								|| (IOAPI_enUSBChannel == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)										
								|| (IOAPI_enENETChannel == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2))
			{
				OS_stSVCDataStruct.enSVCResult = IO_enInitCommsResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (IOAPI_tstPortConfigCB*)OS_stSVCDataStruct.pvArg3);			
			}
			
			else if (IOAPI_enTEPM == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)
			{
				OS_stSVCDataStruct.enSVCResult = TEPM_vInitTEPMResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (TEPMAPI_tstTEPMResourceCB*)OS_stSVCDataStruct.pvArg3);
			}
			
			else if (IOAPI_enCaptureCompare == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)
			{
				OS_stSVCDataStruct.enSVCResult = IO_enInitTEPMChannel(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (TEPMAPI_tstTEPMChannelCB*)OS_stSVCDataStruct.pvArg3, FALSE);
			}

			else if (IOAPI_enPWM == *(IOAPI_tenEHIOType*)OS_stSVCDataStruct.pvArg2)
			{
				OS_stSVCDataStruct.enSVCResult = IO_enInitTEPMChannel(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (TEPMAPI_tstTEPMChannelCB*)OS_stSVCDataStruct.pvArg3, TRUE);
			}

			else
			{
				OS_stSVCDataStruct.enSVCResult = SYSAPI_enResourceUnavailable;
			}
			
			break;			
		}
		
		case SYSAPI_enInitialiseCTRLResource:
		{
			if (CTRLAPI_enPID == *(CTRLAPI_tenCTRLType*)OS_stSVCDataStruct.pvArg1)
			{
				PIDIDX = CTRL_tRequestKernelPIDController((CTRLAPI_tstPIDCB*)OS_stSVCDataStruct.pvArg2);
				OS_stSVCDataStruct.tClientHandle = (uint32)PIDIDX;
				OS_stSVCDataStruct.enSVCResult = (-1 < PIDIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
				break;
			}

			break;
		}
		
		case SYSAPI_enIterateCTRLResource:
		{
			if (CTRLAPI_enPID == *(CTRLAPI_tenCTRLType*)OS_stSVCDataStruct.pvArg1)
			{
				PIDIDX = *(CTRLAPI_tenCTRLType*)OS_stSVCDataStruct.pvArg2;
				//CTRL_vIteratePID(PIDIDX);
				OS_stSVCDataStruct.enSVCResult = (-1 < PIDIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
				break;
			}

			break;
		}		
		
		case SYSAPI_enInitialiseSpreadResource:
		{
			tSpreadIDX = SPREAD_tRequestKernelSpread((SPREADAPI_tstSpreadCB*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.tClientHandle = (uint32)tSpreadIDX;
			OS_stSVCDataStruct.enSVCResult = (-1 < tSpreadIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
			break;
		}		
		
		case SYSAPI_enInitialiseTableResource:
		{
			tTableIDX = TABLE_tRequestKernelTable((TABLEAPI_tstTableCB*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.tClientHandle = (uint32)tTableIDX;
			OS_stSVCDataStruct.enSVCResult = (-1 < tSpreadIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
			break;
		}	

		case SYSAPI_enInitialiseMapResource:
		{
			tMapIDX = MAP_tRequestKernelMap((MAPSAPI_tstMapCB*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.tClientHandle = (uint32)tMapIDX;
			OS_stSVCDataStruct.enSVCResult = (-1 < tSpreadIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
			break;
		}
		
		case SYSAPI_enAssertDIOResource:
		{
			IO_vAssertDIOResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, *(IOAPI_tenTriState*)OS_stSVCDataStruct.pvArg2);			
			break;
		}

		case SYSAPI_enGetDIOResource:
		{
			boResult = IO_boGetDIOResource(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.pvArg1 = (void*)&boResult;
			break;
		}
		
		case SYSAPI_enTriggerADQueue:
		{
			boResult = ADC_vTriggerQueue(*(ADCAPI_tenTrigger*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enRequestIOBusTransfer:
		{
			switch ((IOAPI_tenEHIOResource)OS_stSVCDataStruct.pvArg1)
			{				
				case EH_VIO_IIC1:
				case EH_VIO_SPI1:
				case EH_VIO_SPI2:
				{				
					OS_stSVCDataStruct.enSVCResult = SRLTFR_enEnqueue((IOAPI_tstTransferCB*)OS_stSVCDataStruct.pvArg2);
					break;
				}
				default:
				{
					OS_stSVCDataStruct.enSVCResult = SYSAPI_enBadResource;
					break;
				}
			}
			break;
		}	

		case SYSAPI_enAppendTEPMQueue:
		{
			TEPM_vAppendTEPMQueue(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (TEPMAPI_tstTimedUserEvent*)OS_stSVCDataStruct.pvArg2, *(TEPMAPI_ttEventCount*)OS_stSVCDataStruct.pvArg3);
			break;
		}
		
		case SYSAPI_enConfigureUserTEPMInput:
		{
			TEPM_vConfigureUserTEPMInput(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (TEPMAPI_tstTimedUserEvent*)OS_stSVCDataStruct.pvArg2);
			break;
		}			
		
		case SYSAPI_enConfigureKernelTEPMOutput:
		{
			TEPM_vConfigureKernelTEPMOutput(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (TEPMAPI_tstTimedKernelEvent*)OS_stSVCDataStruct.pvArg2, *(TEPMAPI_ttEventCount*)OS_stSVCDataStruct.pvArg3);
			break;
		}				
		
		case SYSAPI_enWriteDACQueue:
		{
			//DAC_vWriteDACQueue(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (DACAPI_ttOutputVoltage*)OS_stSVCDataStruct.pvArg2);
			break;
		}
		
		case SYSAPI_enCalculateSpread:
		{
			boResult = SPREAD_vCalculate(*(SPREADAPI_ttSpreadIDX*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enCalculateTable:
		{
			boResult = TABLE_vCalculate(*(TABLEAPI_ttTableIDX*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}		

		case SYSAPI_enCalculateMap:
		{
			boResult = MAP_vCalculate(*(MAPSAPI_ttMapIDX*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enSetDiagCallback:
		{
			DIAG_vSetCallBack(*(DIAGAPI_tenCallBackType*)OS_stSVCDataStruct.pvArg1, OS_stSVCDataStruct.pvArg2);
			break;
		}	
		
		case SYSAPI_enGetTimerValue:
		{
			if ((EH_VIO_TPM0 <= *(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1) &&
			(EH_VIO_TPM2 >= *(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1))
			{
				TEPM_u32GetTimerVal(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, OS_stSVCDataStruct.pvArg2);
			}		
			break;			
		}
		
		case SYSAPI_enGetCRC16:
		{
			pu16Temp = (puint16)CRC16_pu16CalcCRC(*(uint16*)OS_stSVCDataStruct.pvArg1, (uint8*)OS_stSVCDataStruct.pvArg2, *(uint16*)OS_stSVCDataStruct.pvArg3);
			OS_stSVCDataStruct.pvData = (void*)pu16Temp;
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			break;
		}		
		
		case SYSAPI_enCheckPartition:
		{
			boResult = FEE_boCheckPartition();
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadPartition;
			break;
		}	

		case SYSAPI_enNVMWorkingCopy:
		{
			boResult = FEE_boNVMWorkingCopy(*(bool*)OS_stSVCDataStruct.pvArg1, *(bool*)OS_stSVCDataStruct.pvArg2, ~0);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadNVMWorkingCopy;			
			break;
		}		
		
		case SYSAPI_enSetupWorkingPage:
		{		
			boResult = FEE_boSetWorkingData((uint8*)*(uint32*)OS_stSVCDataStruct.pvArg1, *(uint16*)OS_stSVCDataStruct.pvArg2);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enQueueCANMessage:
		{
			boResult = DLL_vQueueCANMessage(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, (PROTAPI_tstCANMsg*)OS_stSVCDataStruct.pvArg2);	
			break;
		}		

		case SYSAPI_enSetupCrankTriggerEdgePattern:
		{
			CEM_tstPatternSetupCB* pstPatternSetupCB =  (CEM_tstPatternSetupCB*)OS_stSVCDataStruct.pvArg2;

			IOAPI_tenEdgePolarity enEdgePolarity = pstPatternSetupCB->enEdgePolarity;
			bool boFirstEdgeRising = pstPatternSetupCB->boFirstEdgeRising;
			uint32 u32TriggerType = pstPatternSetupCB->u32TriggerType;

			boResult = CEM_boPopulateCrankEdgeArrays((puint16)OS_stSVCDataStruct.pvArg1, boFirstEdgeRising, enEdgePolarity, u32TriggerType);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;			
			break;
		}

		case SYSAPI_enSetupSyncPointsPattern:
		{
		    CEM_vSetSyncPhaseRepeats(*(uint32*)OS_stSVCDataStruct.pvArg2);
			boResult = CEM_boPopulateSyncPointsArray((puint16)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;			
			break;
		}

		case SYSAPI_enResetWatchdog:
		{
		    //boResult = WDT_boReset(0);
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}

		case SYSAPI_enGetSpreadResult:
		{
			SPREADAPI_tstSpreadResult* pstSpreadResult = SPREAD_pstGetSpread(*(TABLEAPI_ttTableIDX*)OS_stSVCDataStruct.pvArg1);
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			OS_stSVCDataStruct.pvArg1 = (void*)pstSpreadResult;
			break;
		}

		case SYSAPI_enGetRawCommsBuffer:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			OS_stSVCDataStruct.pvArg1 = DLL_pvGetBuffered(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1);
			break;
		}

		case SYSAPI_enTEPMAsyncRequest:
		{
			TEPM_vAsyncRequest();
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			break;
		}

		case SYSAPI_enSetupSimpleCamSync:
		{
			CEM_vSetupSimpleCamSync(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1, *(bool*)OS_stSVCDataStruct.pvArg2, *(uint32*)OS_stSVCDataStruct.pvArg3);
			break;
		}

		case SYSAPI_enSetupWatchdog:
		{
			WDT_vStart(*(uint16*)OS_stSVCDataStruct.pvArg1, *(uint16*)OS_stSVCDataStruct.pvArg2);
			break;
		}

		case SYSAPI_enGetMode1Buffer:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			OS_stSVCDataStruct.pvArg1 = UDSAL_au8Mode1DataBuffer;
			break;
		}

		case SYSAPI_enGetMode2Buffer:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			OS_stSVCDataStruct.pvArg1 = UDSAL_au8Mode2DataBuffer;
			break;
		}

		case SYSAPI_enGetMode3Buffer:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			OS_stSVCDataStruct.pvArg1 = UDSAL_au8Mode3DataBuffer;
			break;
		}

		case SYSAPI_enGetMode4Buffer:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			OS_stSVCDataStruct.pvArg1 = UDSAL_au8Mode4DataBuffer;
			break;
		}

		case SYSAPI_enGetByteQueue:
		{
			switch (*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1)
			{
				case EH_VIO_UART1:
				case EH_VIO_UART2:
				{
					OS_stSVCDataStruct.pvArg1 = (void*)UART_pstGetQueue(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1);
					OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
					break;
				}
				default:
				{
					OS_stSVCDataStruct.enSVCResult = SYSAPI_enFail;
				}
			}
			break;
		}

		case SYSAPI_enGetByteQueueBuffer:
		{
			switch (*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1)
			{
				case EH_VIO_UART1:
				case EH_VIO_UART2:
				{
					OS_stSVCDataStruct.pvArg1 = (void*)UART_pu8GetQueueBuffer(*(IOAPI_tenEHIOResource*)OS_stSVCDataStruct.pvArg1);
					OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
					break;
				}
				default:
				{
					OS_stSVCDataStruct.enSVCResult = SYSAPI_enFail;
				}
			}
			break;
		}

		case SYSAPI_enSetFuelCuts:
		{
			TEPM_vSetFuelCutsMask(*(uint32*)OS_stSVCDataStruct.pvArg1, *(uint32*)OS_stSVCDataStruct.pvArg2, *(uint32*)OS_stSVCDataStruct.pvArg3);
			break;
		}

		case SYSAPI_enSetSparkCuts:
		{
			TEPM_vSetSparkCutsMask(*(uint32*)OS_stSVCDataStruct.pvArg1, *(uint32*)OS_stSVCDataStruct.pvArg2, *(uint32*)OS_stSVCDataStruct.pvArg3);
			break;
		}

		case SYSAPI_enConfigureMissingToothInterrupt:
		{
			TEPM_vConfigureMissingToothInterrupt();
			break;
		}

		case SYSAPI_enConfigureFuelPWMExport:
		{
			TEPM_vConfigureFuelPWMExport((uint32*)OS_stSVCDataStruct.pvArg1);
			break;
		}

		default:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enFail;
			break;
		}
	}
}



#endif // KERNEL
