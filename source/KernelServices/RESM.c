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
#define _RESM_C

#include <SYS.h>
#include <BUILD.h>
#include <stddef.h>

#ifdef BUILD_KERNEL_APP

#include "IOAPI.h"
#include <TYPES.h>
#include "TEPM.h"
#include "RESM.h"

RESM_tstResource RESM_astResource[IO_Total_Count + RES_Count];

void RESM_vStart(uint32* const u32Stat)
{
	uint32 u32IDX;
	
	for (u32IDX = 0; u32IDX < (IO_Total_Count + RES_Count); u32IDX++)
	{
		RESM_astResource[u32IDX].tClientHandle = (SYSAPI_ttClientHandle)NULL;
	}	
}

void RESM_vRun(uint32* const u32Stat)
{
	
}

void RESM_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_ttClientHandle RESM_u32RequestEHIOResource(IOAPI_tenEHIOResource enEHIO, SYSAPI_ttClientHandle tClientHandle)
{		
	if ((0 == RESM_astResource[enEHIO].tClientHandle)
		||(tClientHandle == RESM_astResource[enEHIO].tClientHandle))
	{
		RESM_astResource[enEHIO].tClientHandle = tClientHandle;
	}
	else
	{
		tClientHandle = 0;
	}
	
	return tClientHandle;
}

SYSAPI_ttClientHandle RESM_u32GetMasterEHIOResource(IOAPI_tenEHIOResource enEHIOResource)
{
	return RESM_astResource[enEHIOResource].tClientHandle;	
}

uint32 RESM_u32GetMasterEHIOResourceList(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOResource* penEHIOResource)
{
	uint32 u32MasterCount = 0;
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR1:
		case EH_IO_TMR2:
		case EH_IO_TMR3:
		case EH_IO_TMR4:
		case EH_IO_TMR5:
		case EH_IO_TMR6:
		case EH_IO_TMR7:
		case EH_IO_TMR8:
		case EH_IO_TMR9:
		case EH_IO_TMR10:
		case EH_IO_TMR11:
		case EH_IO_TMR12:
		case EH_IO_TMR13:
		case EH_IO_TMR14:
		case EH_IO_TMR15:
		case EH_IO_TMR16:	
		{
#ifdef BUILD_KERNEL_OR_KERNEL_APP
			TEPM_boGetMasterEHIOResourceList(enEHIOResource, penEHIOResource, &u32MasterCount);
#endif			
			break;		
		}
		default:
		{
			break;
		}
	}
	
	return u32MasterCount;
}


#endif // KERNEL

