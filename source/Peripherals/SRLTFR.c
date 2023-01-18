/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SRLTFR                                                 */
/* DESCRIPTION:        Serial Transfer file                                   */
/* FILE NAME:          SRLTFR.c                                               */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _SRLTFR_C
 
#include <string.h>
#include "IIC.h"
#include "IOAPI.h"
#include "PERSPI.h"
#include "SYSAPI.h"
#include "types.h"
#include "CQUEUE.h"
#include "SRLTFR.h"
#include "declarations.h"

IOAPI_tenEHIOResource SRLTFR_enActiveEHIOResource;

void SRLTFR_vStart(puint32 const pu32Arg)
{
	CQUEUE_xInit(&SRLTFR_stTransferQueue, SRLTFR_nTransfersMax, SRLTFR_astTransferInfo);
	SRLTFR_enActiveEHIOResource = EH_IO_Invalid;
}

void SRLTFR_vRun(puint32 const pu32Arg)
{
	if (!CQUEUE_xIsEmpty(&SRLTFR_stTransferQueue))
	{
		switch (SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].enEHIOResource)
		{
			case EH_VIO_IIC1:
			{
				/* Caveat here see SPI */
				IIC_vInitTransfer(&SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head]);
				SRLTFR_enActiveEHIOResource = SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].enEHIOResource;
			}
			case EH_VIO_SPI1:
			case EH_VIO_SPI2:
			{
				if (0 != SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].u32ByteCount)
				{
					SRLTFR_enActiveEHIOResource = SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].enEHIOResource;
					SPI_vInitTransfer(&SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head]);
				}
			}
			default:
			{

			}	
		}
	}
}

void SRLTFR_vTerminate(puint32 const pu32Arg)
{
	
}

SYSAPI_tenSVCResult SRLTFR_enEnqueue(IOAPI_tstTransferCB* pstTransferCB)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enQueueFull;
	uint32 u32Flag;
	

	if (!CQUEUE_xIsFull(&SRLTFR_stTransferQueue))
	{
		memcpy((void*)&SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head],
						 (void*)pstTransferCB,
						  sizeof(IOAPI_tstTransferCB));	

		enSVCResult = SYSAPI_enOK;
		CQUEUE_xAddItem(&SRLTFR_stTransferQueue);

		if (TRUE == pstTransferCB->boBlockingMode)
		{
			SRLTFR_vRun(&u32Flag);
			enSVCResult = SYSAPI_enOK;

			/* Clear the queue because it was all sent in blocking mode */
			CQUEUE_xRemoveItem(&SRLTFR_stTransferQueue);
		}
	}
	
	
	return enSVCResult;
}

void SRLTFR_vNotifyCB(IOAPI_tenEHIOResource enEHIOResource)
{
	IOAPI_enTransferType enTransferType;
	IOAPI_tpfTransferCB pfCB;
	
	if (SRLTFR_enActiveEHIOResource == enEHIOResource)
	{
		enTransferType = SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].enTransferType;
		pfCB = SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].pfCB;
		
		if ((NULL != pfCB) && (IOAPI_IO_TX == enTransferType))
		{
			(*pfCB)(enEHIOResource, 
						SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].pvData,
						SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].u32ByteCount,
						SRLTFR_astTransferInfo[SRLTFR_stTransferQueue.u32Head].enTransferType);
		}
		CQUEUE_xRemoveItem(&SRLTFR_stTransferQueue);
	}
}
			
			
