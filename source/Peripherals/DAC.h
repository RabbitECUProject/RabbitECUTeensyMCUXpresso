/******************************************************************************/
/* DAC header file                                                            */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef DAC_H
#define DAC_H

#include "CPUAbstract.h"
#include "DACAPI.h"
#include "DACHA.h"
#include "declarations.h"
#include "IOAPI.h"
#include "os.h"
#include "regset.h"
#include "SIM.h"
#include "SYSAPI.h"
#include "sys.h"
#include "types.h"

#define DAC_nDACCount 2u
#define DAC_nQueueSize 16u

void DAC_vStart(puint32 const);
void DAC_vRun(puint32 const);
void DAC_vTerminate(puint32 const);
SYSAPI_tenSVCResult DAC_vInitDACResource(IOAPI_tenEHIOResource, DACAPI_tstDACCB*);
void DAC_vWriteDACQueue(IOAPI_tenEHIOResource, DACAPI_ttOutputVoltage*);	
#endif // DAC_H

