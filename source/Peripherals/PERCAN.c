/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CAN                                                    */
/* DESCRIPTION:        This code provides functions for interacting with      */
/*                     the CAN hardware module/s                              */
/* FILE NAME:          CAN.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "PERCAN.h"

void CAN_vStart(uint32* const u32Stat)
{
	
}

SYSAPI_tenSVCResult CAN_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	return CANHA_u32InitBus(enEHIOResource, pstPortConfigCB);
}

void CAN_vRun(uint32* const u32Stat)
{

}

bool CAN_boReadMB(tstCANModule* pstCAN, CANHA_tstCANMB* pstCANMB)
{
    return CANHA_boReadMB(pstCAN, pstCANMB);
}

void CAN_vInterrupt(IOAPI_tenEHIOResource enEHIOResource, void* pvData)
{
    CANHA_vInterrupt(enEHIOResource);
}

void CAN_vInitTransfer(IOAPI_tstTransferCB* pstTransferCB)
{
	CANHA_vInitTransfer(pstTransferCB);
}

void CAN_vTerminate(uint32* const u32Stat)
{

}




			
			
