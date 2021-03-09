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
#ifndef FEE_H
#define FEE_H

#include <string.h>
#include <stddef.h>
#include "COMMONNL.h"
#include "CRC16.h"
#include "sys.h"
#include "types.h"


// FEE MDAC error codes
typedef enum
{
	enErrNone,
	enErrPViol,
	enErrOOR,
	enErrAlign,
	enErrContig,
	enErrAccess,
	enErrBusy
} FEE_tenPgmErrCode;

typedef enum
{
	enMarginNormal,
	enMarginUser,
	enMarginFactory
} FEE_tenMarginChoice;

#define FEE_nReg8Set 																																																\
{																																																										\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT3)), (uint8)FEE_PFLASH_PRT3, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT2)), (uint8)FEE_PFLASH_PRT2, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT1)), (uint8)FEE_PFLASH_PRT1, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT0)), (uint8)FEE_PFLASH_PRT0, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FDPROT)), (uint8)FEE_DFLASH_PROT, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FEPROT)), (uint8)FEE_EFLASH_PROT, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }																		\
}

typedef struct
{
	puint8 pu8WorkingData;
	sint16 s16WorkingDataCount;
} FEE_tstWorkingPage;

typedef struct
{
	COMMONNL_tstRXLargeBuffer* pstSourceBuffer;
	uint8* pu8TargetAddress;
	uint32 u32WriteCount;
	uint32 u32AccumulateCount;
	bool boProgramming;
	bool boProgErr;
} FEE_tstWriteControlBlock;

void FEE_vStart( uint32* const );
void FEE_vRun( uint32* const );
void FEE_vTerminate( uint32* const );
void FEE_vStartSBL(void);
bool FEE_boCheckPartition(void);
bool FEE_boSetWorkingData(puint8, uint16);
bool FEE_boNVMWorkingCopy(bool, bool);
bool FEE_boNVMClear(void);
bool FEE_boPartition(void);
bool FEE_boWriteControlBlock(COMMONNL_tstRXLargeBuffer* const, 
															 uint8* const, 
															 uint32 );
bool FEE_boUpdateControlBlock( uint32);
bool FEE_boEraseForDownload(puint8, uint32);
bool FEE_boWriteNVM(puint8, puint8, uint32);

#endif // FEE_H

