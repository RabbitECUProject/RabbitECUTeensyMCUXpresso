/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "CPUAbstract.h"
#include "CQUEUE.h"
#include "peruart.h"
#include "DLL.h"
#include "regset.h"
#include "sys.h"
#include "IRQ.h"
#include "declarations.h"
#include "os.h"
#include "DLL.h"
#include "types.h"
#include "SIM.h"

CQUEUE_tstQueue UART_astCBQueue[EH_LAST_UART - EH_FIRST_UART + 1];
uint8 UART_aau8ByteQueue[EH_LAST_UART - EH_FIRST_UART + 1][UART_nByteQueueSize];


void UART_vStart(uint32* const u32Stat)
{
	OS_xModuleStartOK(*u32Stat);

	CQUEUE_xInit(&UART_astCBQueue[0], UART_nByteQueueSize, &UART_aau8ByteQueue[0][0]);
	CQUEUE_xInit(&UART_astCBQueue[1], UART_nByteQueueSize, &UART_aau8ByteQueue[1][0]);
}

void UART_vRun(uint32* const u32Stat)
{

}

void UART_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_tenSVCResult UART_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{	
	SYSAPI_tenSVCResult enSVCResult;
	
	enSVCResult = UARTHA_enInitBus(enEHIOResource, pstPortConfigCB);
	
	return enSVCResult;
}

void UART_vSendChar(IOAPI_tenEHIOResource enEHIOResource, uint8 ucData)
{
	UARTHA_vSendChar(enEHIOResource, ucData);
}

uint8 UART_u8GetChar(IOAPI_tenEHIOResource enEHIOResource)
{
	return UARTHA_u8GetChar(enEHIOResource);	
}

void UART_vDisableTXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UARTHA_vDisableTXInterrupt(enEHIOResource);
}

void UART_vEnableTXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UARTHA_vEnableTXInterrupt(enEHIOResource);
}

void UART_vDisableRXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UARTHA_vDisableRXInterrupt(enEHIOResource);
}

void UART_vEnableRXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UART_vEnableRXInterrupt(enEHIOResource);
}

void UART_vInterrupt(IOAPI_tenEHIOResource enEHIOResource, void* pvData)
{
    uint32 u32UARTIDX = enEHIOResource - EH_VIO_UART1;
    uint8 u8Data;
    DLL_tstRXDLLData* pstRXDLLData = (DLL_tstRXDLLData*)pvData;

	while (!CQUEUE_xIsFull(&UART_astCBQueue[u32UARTIDX]))
	{
		if (0 < pstRXDLLData->u8DataCount)
		{
			u8Data = pstRXDLLData->u8Data[pstRXDLLData->u8RXByteIDX];
			UART_aau8ByteQueue[u32UARTIDX][UART_astCBQueue[u32UARTIDX].u32Tail] = u8Data;
			CQUEUE_xAddItem(&UART_astCBQueue[u32UARTIDX]);
			pstRXDLLData->u8DataCount--;
		}

		if (0 == pstRXDLLData->u8DataCount) break;
	}
}

uint8 UART_u8GetQueuedChar(IOAPI_tenEHIOResource enEHIOResource)
{
	uint8 u8Data;
    uint32 u32UARTIDX = enEHIOResource - EH_VIO_UART1;

    u8Data = UART_aau8ByteQueue[u32UARTIDX][UART_astCBQueue[u32UARTIDX].u32Head];
    CQUEUE_xRemoveItem(&UART_astCBQueue[u32UARTIDX]);

    return u8Data;
}

uint8* UART_pu8GetQueueBuffer(IOAPI_tenEHIOResource enEHIOResource)
{
	uint8* pu8Data;
    uint32 u32UARTIDX = enEHIOResource - EH_VIO_UART1;

    pu8Data = &UART_aau8ByteQueue[u32UARTIDX][0];

    return pu8Data;
}

CQUEUE_tstQueue* UART_pstGetQueue(IOAPI_tenEHIOResource enEHIOResource)
{
	CQUEUE_tstQueue* pstQueue;
    uint32 u32UARTIDX = enEHIOResource - EH_VIO_UART1;

    pstQueue = &UART_astCBQueue[u32UARTIDX];

    return pstQueue;
}


			
			
