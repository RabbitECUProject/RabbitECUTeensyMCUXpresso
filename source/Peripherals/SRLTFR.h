/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      SRLTFR                                                 */
/* DESCRIPTION:        Serial Transfer header file                            */
/* FILE NAME:          SRLTFR.h                                               */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SRLTFR_H
#define SRLTFR_H

#include <stddef.h>
#include <SYS.h>
#include <TYPES.h>
#include "CQUEUE.h"

#define SRLTFR_nTransfersMax 5u

#ifdef _SRLTFR_C
    #undef EXTERN
	#define EXTERN
#else
    #undef EXTERN
	#define EXTERN extern
#endif

EXTERN IOAPI_tstTransferCB SRLTFR_astTransferInfo[SRLTFR_nTransfersMax];
EXTERN CQUEUE_tstQueue SRLTFR_stTransferQueue;

void SRLTFR_vStart(uint32* const);
void SRLTFR_vRun(uint32* const);
void SRLTFR_vTerminate(uint32* const);
SYSAPI_tenSVCResult SRLTFR_enEnqueue(IOAPI_tstTransferCB*);
void SRLTFR_vNotifyCB(IOAPI_tenEHIOResource);


#endif // CAN_H

