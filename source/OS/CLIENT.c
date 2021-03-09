/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CLIENT                                                 */
/* DESCRIPTION:        This code provides functions for adding client tasks   */
/*                     and running the client callback queue                  */
/* FILE NAME:          CLIENT.c                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <BUILD.h>
#include <CLIENT.h>
#include <DECLARATIONS.h>
#include <SYS.h>
#include <TASK.h>
#include <TYPES.h>
#include "CPUAbstract.h"
#include "TASKAPI.h"
#include "SYSAPI.h"
#include "IOAPI.h"
#include "ADCAPI.h"
#include "TEPMAPI.h"
#include "USER.h"
#include "MSG.h"
#include "PERADC.h"
#include "TEPM.h"
#include "CQUEUE.h"


uint32 u32Temp;
uint32 CLIENT_u32TaskStartCount = 0;
uint32 CLIENT_u32RunTaskCount = 0;
CLIENT_tstCBInfo astCBInfo[CLIENT_nUserCBMax];
CQUEUE_tstQueue stCBQueue;

TASK_tstTask CLIENT_rastTaskCyclic[SYS_CLIENT_MODULES_MAX];

void CLIENT_vStart(uint32 * const pu32Arg)
{
	CQUEUE_xInit(&stCBQueue, CLIENT_nUserCBMax, astCBInfo);	
	
#ifdef BUILD_KERNEL
	puint32 pu32UserEntry = (puint32)CLIENT_nUserAppImageBase;
	uint32 u32UserAppImageEntry= *pu32UserEntry;

	if ((CLIENT_nUserAppImageBase & 0xffff0000) == (0xffff0000 & u32UserAppImageEntry))	
	{
		pfUserStart = (tpfUserStart)u32UserAppImageEntry;
		iRetCode = pfUserStart(&OS_stSVCDataStruct);
	}
#endif

#ifdef BUILD_KERNEL_APP
	(void)USER_vStart(&OS_stSVCDataStruct);
#endif
	
}

void CLIENT_vRunUserCBQueue(void)
{
	ADC_tstADCResult* pstADCResult;
	TEPM_tstTEPMResult* pstTEPMEvent;
	DIAGAPI_tstDataTransferCB* pstDataTransferCB;	
	ADCAPI_tpfResultCB pfADCResultCB;
	TEPMAPI_tpfEventCB pfTEPMEventCB = NULL;
	DIAGAPI_tDataWriteCB pfDataWriteCB;

	CPU_vEnterCritical();

	if (!CQUEUE_xIsEmpty(&stCBQueue))
	{
		switch (astCBInfo[stCBQueue.u32Head].pstMBX->enMSGType)
		{
			case MSG_enADCResult:
			{
				/* Run the callback if not NULL */		
				pstADCResult = (ADC_tstADCResult*)astCBInfo[stCBQueue.u32Head].pstMBX->pstMSG;
				pfADCResultCB = (ADCAPI_tpfResultCB)astCBInfo[stCBQueue.u32Head].pfCB;

				if (NULL != pfADCResultCB)
				{
					pfADCResultCB(pstADCResult->enEHIOResource, pstADCResult->u32Result);
				}

				break;
			}		
			case MSG_enTEPMEvent:
			{
				/* Run the callback if not NULL */		
				pstTEPMEvent = (TEPM_tstTEPMResult*)astCBInfo[stCBQueue.u32Head].pstMBX->pstMSG;
				pfTEPMEventCB = (TEPMAPI_tpfEventCB)astCBInfo[stCBQueue.u32Head].pfCB;
				
				if (NULL != pfTEPMEventCB)
				{
					pfTEPMEventCB(pstTEPMEvent->enEHIOResource, pstTEPMEvent->tEventTime);
				}
				break;
			}		
			case MSG_enDiagDataWrite:
			{
				/* Run the callback if not NULL */		
				pstDataTransferCB = (DIAGAPI_tstDataTransferCB*)astCBInfo[stCBQueue.u32Head].pstMBX->pstMSG;
				pfDataWriteCB = (DIAGAPI_tDataWriteCB)astCBInfo[stCBQueue.u32Head].pfCB;
				
				if (NULL != pfTEPMEventCB)
				{
					(void)pfDataWriteCB(pstDataTransferCB->u16CID, pstDataTransferCB->pu8Data, pstDataTransferCB->u8DataCount);
					pstDataTransferCB->boTransferComplete = TRUE;
				}
				break;
			}				
			default:
			{
				break;
			}
		}
		CQUEUE_xRemoveItemUnsafe(&stCBQueue);
	}

	CPU_vExitCritical();
}

/* Caveat only call this function with interrupts enabled! Not from a thread critical section! */
CLIENT_tenErr CLIENT_enEnqueueCB(MSG_tstMBX* pstMBX, tpfClientCB pfCB)
{
	CLIENT_tenErr enErr = CLIENT_enFailQueueFull;

	CPU_xEnterCritical();
	
	if (!CQUEUE_xIsFull(&stCBQueue))
	{
		astCBInfo[stCBQueue.u32Tail].pfCB = pfCB;
		astCBInfo[stCBQueue.u32Tail].pstMBX = pstMBX;		
		CQUEUE_xAddItemUnsafe(&stCBQueue);
		enErr = CLIENT_enOK;
	}	
	
	CPU_xExitCritical();

	return enErr;
}


SYSAPI_tenSVCResult CLIENT_vAddTask(OS_tenQueueType enQueueType, SYSAPI_tpfUserTaskFunction pfUserTask, TASKAPI_tenPriority enPriority, TASKAPI_tenRateMs enRateMs)
{
	TASK_tstTask stTask;
	SYSAPI_tenSVCResult enSVCResult;
	
	stTask.tTaskID = 0x2000;
	stTask.enTaskType = enTaskFunction;
	stTask.enTaskPriority = enPriority;
	stTask.enTaskRateMS = enRateMs;
	stTask.enTaskState = enTaskReady;
	stTask.pfTaskFunction = pfUserTask;
	stTask.enPrivilege = enTaskUnprivileged;
	
	enSVCResult = OS_enAddTaskToQueue(enQueueType, &stTask);
	
	return enSVCResult;	
}
