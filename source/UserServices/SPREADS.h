/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SPREADS_H
#define SPREADS_H

#include "SPREADSAPI.h"
#include "SYSAPI.h"

#define SPREAD_nKernelSpreadCount	55u
#define SPREAD_nUserSpreadCount	    45u

void SPREADS_vStart(puint32 const pu32Arg);
void SPREADS_vRun(puint32 const pu32Arg);
void SPREADS_vTerminate(puint32 const pu32Arg);
SPREADAPI_ttSpreadIDX SPREAD_tRequestKernelSpread(SPREADAPI_tstSpreadCB*);
bool SPREAD_vCalculate(SPREADAPI_ttSpreadIDX);
SPREADAPI_tstSpreadResult SPREAD_stGetSpread(SPREADAPI_ttSpreadIDX);
SPREADAPI_tstSpreadResult* SPREAD_pstGetSpread(SPREADAPI_ttSpreadIDX);

#endif // TEPM_H

