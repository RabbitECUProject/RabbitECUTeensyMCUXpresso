/******************************************************************************/
/* CPU general hardware abstraction header file                               */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/* This file contains macros that are invoked in hardware independent files   */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#include <SYS.h>
#ifdef KERNEL

#ifndef HARDABSTRACT_H
#define HARDABSTRACT_H

#include "mks20f12.h"

#define CPUA_OS_u32GetSysTimeStamp()			0xFFFFFF;

#define	CPU_xEnterCritical()		__disable_irq()
#define CPU_xExitCritical()		__enable_irq()

void CPU_vStart(void);
void CPU_vEnterCritical(void);
void CPU_vExitCritical(void);
void CPU_vDisableMPROT(void);

#endif // HARDABSTRACT_H
#endif // KERNEL
