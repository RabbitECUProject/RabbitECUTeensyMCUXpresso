/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      FME Header                                             */
/* DESCRIPTION:        This code assist with failure mode effects             */
/*                     processing                                             */
/*                                                                            */
/* FILE NAME:          FME.c                                                  */
/* REVISION HISTORY:   24-06-2020 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef FME_H
#define FME_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"
#include "SYS.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
typedef enum
{
	FME_enPPSPair,
	FME_enPedalTransfer,
	FME_enTPSMSingle,
	FME_enPPSMSingle,
	FME_enPPSSSingle,
	FME_enDiagCount
} FME_tenDiags;

typedef enum
{
	FME_enRelationship,
	FME_enSingle,
	FME_enDiagTypesCount
} FME_tenDiagTypes;

typedef enum
{
	FME_enNoFault,
	FME_enFaultActive,
	FME_enFaultCleared
} FME_tenFaultState;

typedef struct
{
	FME_tenDiagTypes enDiagtype;
	uint32 u32FilterDataMin;
	uint32 u32FilterDataMax;
	uint32 u32FaultCountLimit;
} FME_tstDiagControlData;

typedef struct
{
	uint32 u32FilterData;
	uint32 u32FaultCounts;
	FME_tenFaultState enFaultState;
} FME_tstDiagRunningData;

#define FME_nControlData \
{	\
	{FME_enRelationship, 800, 3000, 200},\
	{FME_enRelationship, 800, 3000, 500},\
	{FME_enSingle, 250, 2900, 30},\
	{FME_enSingle, 250, 2900, 30},\
	{FME_enSingle, 190, 1450, 30},\
}\

#define FME_nBLIP_THROTTLE_DELTA_MAX 1000
#define FME_nOVERRIDE_DELTA_MAX 500

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void FME_enUpdateDiagState(FME_tenDiags enDiags, uint32 u32Data1, uint32 u32Data2);
FME_tenFaultState FME_enGetDiagState(FME_tenDiags enDiags);
void FME_vDiagClearHistory(void);

#endif // FME_H

