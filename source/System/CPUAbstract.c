/******************************************************************************/
/* CPU abstraction c file                                                     */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#include <TYPES.h>
#include "CPUAbstract.h"

bool CPU_boCriticalNestCounter;

void CPU_vStart(void)
{
	CPU_boCriticalNestCounter = 0;
}

void CPU_vEnterCritical(void)
{
	CPU_xEnterCritical();
	//CPU_boCriticalNestCounter++;	
}

void CPU_vExitCritical(void)
{
	//if (1 == CPU_boCriticalNestCounter)
	//{
		CPU_xExitCritical();
	//	CPU_boCriticalNestCounter = 0;
	//}
	//else
	//{
	//	CPU_boCriticalNestCounter--;			
	//}
}

void CPU_vDisableMPROT(void)
{
	SYSMPU->CESR = 0;
}


