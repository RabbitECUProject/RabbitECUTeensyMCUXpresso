/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      WDTHA                                                  */
/* DESCRIPTION:        This code provides watchdog functions abstraction      */
/*                     layer                                                  */
/* FILE NAME:          WDTHA.c                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "WDTHA.h"

void WDTHA_vStart(uint16 u16ResetVal, uint16 u16WindowVal)
{
#ifdef BUILD_SAM3X8E
    Wdt* pstWDT = WDT;
	uint32 u32Mode = 0;

	if (0 == u16ResetVal)
	{
	    u32Mode = WDT_MR_WDDIS;		
	}
    
	wdt_init(pstWDT, u32Mode, u16ResetVal, u16WindowVal);
#endif //BUILD_SAM3X8E
}

bool WDTHA_boReset(uint16 u32InputVal)
{
#ifdef BUILD_SAM3X8E
    Wdt* pstWDT = WDT;

    wdt_restart(pstWDT);

	return TRUE;
#endif //BUILD_SAM3X8E

	return FALSE;
}

