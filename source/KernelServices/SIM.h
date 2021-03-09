/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SIM Header File                                        */
/* DESCRIPTION:        This code is provides enum definitions                 */
/*                                                                            */
/* FILE NAME:          SIM.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <SYS.h>
#ifdef KERNEL

#ifndef SIM_H
#define SIM_H

#include <TYPES.h>
#include "simha.h"


void SIM_vSetReg32(SIMHA_tenReg, uint32);
void SIM_vClrReg32(SIMHA_tenReg, uint32);
void SIM__vSetRegAddress(SIMHA_tenReg);

#endif // SIM_H

#endif // KERNEL
