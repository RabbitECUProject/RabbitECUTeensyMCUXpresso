/******************************************************************************/
/* CPU abstraction c file                                                     */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#include "TYPES.h"
#include "CPUAbstract.h"

bool CPU_boCriticalNestCounter;

void CPU_vStart(void)
{
	CPU_boCriticalNestCounter = 0;
}

void CPU_vEnterCritical(void)
{
	CPU_xEnterCritical();
}

void CPU_vExitCritical(void)
{
	CPU_xExitCritical();
}

void CPU_vDisableMPROT(void)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64)
	SYSMPU->CESR = 0;
#endif
}


