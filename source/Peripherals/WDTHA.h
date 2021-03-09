/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      WDTHA header file                                      */
/* DESCRIPTION:        This code provides watchdog functions abstraction      */
/*                     layer                                                  */
/* FILE NAME:          WDTHA.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef WDTHA_H
#define WDTHA_H

#include "types.h"
#include "declarations.h"

#ifdef BUILD_SAM3X8E
#include "wdt.h"
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK64
#include "mk64f12.h"
#endif //BUILD_MK64

void WDTHA_vStart(uint16, uint16);
bool WDTHA_boReset(uint16);

#endif //WDTHA_H



