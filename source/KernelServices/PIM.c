/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      PIM Port Integration Module purpose is to provide      */
/*                     bit I/O service to the application layer               */
/* DESCRIPTION:                                                               */
/* FILE NAME:          PIM.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "OS.h"
#include "PIM.h"
#include "PIMHA.h"


void PIM_vStart(uint32* const u32Stat)
{
	PIMHA_vStart(u32Stat);
	OS_xModuleStartOK(*u32Stat);
}

void PIM_vRun(uint32* const u32Stat)
{

}

void PIM_vTerminate(uint32* const u32Stat)
{

}

void PIM_vInitPortBit(PIMAPI_tenPort enPort, IOAPI_tenEHIOType enIOType, uint32 u32PortBit)
{
	PIMHA_vInitPortBit(enPort, enIOType, u32PortBit);
}

void PIM_vAssertPortBit(PIMAPI_tenPort enPort, uint32 u32PortBit, IOAPI_tenTriState enTriState)
{
	if (PIMAPI_enVIRT_PORT_F > enPort)
	{
		PIMHA_vAssertPortBit(enPort, u32PortBit, enTriState);
	}
}


bool PIM_boGetPortBitState(PIMAPI_tenPort enPort, uint32 u32PortBit)
{
	bool boState = FALSE;

	if (PIMAPI_enVIRT_PORT_F > enPort)
	{
		boState = PIMHA_boGetPortBitState(enPort, u32PortBit);
	}

	return boState;

}

void PIM_vSetPortMux(PIMAPI_tenPort enPort, IOAPI_tenEHIOType enIOType, uint32 u32PortBit, uint32 u32MuxSel)
{
    PIMHA_vSetPortMux(enPort, enIOType, u32PortBit, u32MuxSel);	
}




			
			
