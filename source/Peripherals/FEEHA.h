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
#ifndef FEEHA_H
#define FEEHA_H

#include <string.h>
#include <stddef.h>
#include "COMMONNL.h"
#include "CRC16.h"
#include "declarations.h"
#include "DLL.h"
#include "FEE.h"
#include "IOAPI.h"
#include "macros.h"
#include "os.h"
#include "regset.h"
#include "sys.h"
#include "types.h"


#if defined(BUILD_MK60) || defined(BUILD_MK64)
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"
//#include "NormalDemo.h"

#define M8(adr)    (*((volatile unsigned char  *) (adr)))
#define M16(adr)   (*((volatile unsigned short *) (adr)))
#define M32(adr)   (*((volatile unsigned long  *) (adr)))

// FEE programming interface address ranges
#define FEEHA_PFLASH_START 			0x00000000u    		// Program Flash base address
#define FEEHA_PFLASH_END	 			0x0007FFFFu				// Program Flash end address
#define FEEHA_DFLASH_START 			0x10000000u   		// Data    Flash base address
#define FEEHA_DFLASH_END	 			0x1001FFFFu				// Data		 Flash end address
#define FEEHA_EEPROM_START			0x14000000u				// EEprom	 Base address
#define FEEHA_EEPROM_END				0x14003FFFu				// EEprom  Base address
#define FEEHA_WORK_DATA_START		    0x20000000u
#define FEEHA_WORK_DATA_END			0x2001FFFFu
#define FEEHA_WORK_DATA_MAX			0x4000u

// FEE sector sizes
#define FEEHA_PFLASH_SCTR_BYTES	4096u							// Program Flash bytes per sector
#define FEEHA_DFLASH_SCTR_BYTES	4096u							// Data 	 Flash bytes per sector
#define FEEHA_EEPROM_SCTR_BYTES	1024u							// EEprom        bytes per sector

#define FEEHA_PFLASH_BYTES_PHRS	0x10							// Program Flash bytes per phrase
#define FEEHA_DFLASH_BYTES_PHRS	0x10							// Data 	 Flash bytes per phrase
#define FEEHA_EEPROM_BYTES_PHRS	0x10							// EEprom        bytes per phrase

#define FEEHA_PFLASH_BYTES_WORD	0x08							// Program Flash bytes per word
#define FEEHA_DFLASH_BYTES_WORD	0x08							// Data 	 Flash bytes per word
#define FEEHA_EEPROM_BYTES_WORD	0x01							// EEprom        bytes per word

#define FEEHA_PFLASH_SCTR_WORDS	(FEEHA_PFLASH_SCTR_BYTES / FEEHA_PFLASH_BYTES_WORD)
#define FEEHA_DFLASH_SCTR_WORDS	(FEEHA_DFLASH_SCTR_BYTES / FEEHA_DFLASH_BYTES_WORD)
#define FEEHA_EEPROM_SCTR_WORDS	(FEEHA_EEPROM_SCTR_BYTES / FEEHA_EEPROM_BYTES_WORD)

#define FEEHA_PFLASH_SCTR_PHRS  (FEEHA_PFLASH_SCTR_BYTES / FEEHA_PFLASH_BYTES_PHRS)
#define FEEHA_DFLASH_SCTR_PHRS  (FEEHA_DFLASH_SCTR_BYTES / FEEHA_DFLASH_BYTES_PHRS)
#define FEEHA_EEPROM_SCTR_PHRS  (FEEHA_EEPROM_SCTR_BYTES / FEEHA_EEPROM_BYTES_PHRS)

// FEE Commands
#define FEEHA_CMD_READ1SB 0x00            				// Verify block erased
#define FEEHA_CMD_READ1SS 0x01            				// Verify section erased
#define FEEHA_CMD_PRGCHK	0x02										// Checks programmed phrases at margin read
#define FEEHA_CMD_READIFR	0x03										// READ IFR field
#define FEEHA_CMD_PRG     0x07										// Program 8 bytes
#define FEEHA_CMD_ERASEB	0x08										// Erase Flash block
#define FEEHA_CMD_ERASES	0x09										// Erase Flash sector
#define FEEHA_CMD_PRGS    0x0B										// program data from section program buffer
#define FEEHA_CMD_READ1SL	0x40										// Verify all blocks erased
#define FEEHA_CMD_ERASEL	0x44										// Erase all blocks

// FEE constants
#define FEEHA_PFLASH_PRT3	0xFF
#define FEEHA_PFLASH_PRT2 0xFF
#define FEEHA_PFLASH_PRT1 0xFF
#define FEEHA_PFLASH_PRT0 0xFE
#define FEEHA_DFLASH_PROT	0xFF
#define FEEHA_EFLASH_PROT	0xFF
#define FEEHA_SEC         0x02

// FEE records addresses
#define FEEHA_ADCREC_ADDRESS 0x14003FC0u

// FEE macros
#define FEEHA_xWaitCmdToComplete()	((pstFTFE -> FSTAT) & FTFE_FSTAT_CCIF_MASK) != FTFE_FSTAT_CCIF_MASK

typedef FTFE_Type tstFTFEModule;
typedef FLASH_SSD_CONFIG tstFTFEConfig;


#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
typedef Efc tstFTFEModule;
typedef struct 
{
    bool boDummy;
} tstFTFEConfig;//matthew to be found in driver later

// FEE programming interface address ranges
#define FEEHA_PFLASH_START 			0x00000000u    		// Program Flash base address
#define FEEHA_PFLASH_END	 		0x0007FFFFu				// Program Flash end address
#define FEEHA_DFLASH_START 			0x10000000u   		// Data    Flash base address
#define FEEHA_DFLASH_END	 		0x1007FFFFu				// Data		 Flash end address
#define FEEHA_EEPROM_START			0x000c0000u				// EEprom	 Base address
#define FEEHA_EEPROM_END			0x000c7FFFu				// EEprom  Base address
#define FEEHA_WORK_DATA_START		0x20070000u
#define FEEHA_WORK_DATA_END			0x2007FFFFu
#define FEEHA_WORK_DATA_MAX			0x4000u

#define FEEHA_EXC_RECS_START        (FEEHA_EEPROM_START + 0x7000)
#define FEEHA_EXC_RECS_END          (FEEHA_EEPROM_START + 0x7000 + 0xfff)

// FEE sector sizes
#define FEEHA_PFLASH_SCTR_BYTES	4096u							// Program Flash bytes per sector
#define FEEHA_DFLASH_SCTR_BYTES	4096u							// Data 	 Flash bytes per sector
#define FEEHA_EEPROM_SCTR_BYTES	1024u							// EEprom        bytes per sector

#define FEEHA_PFLASH_BYTES_PHRS	0x10							// Program Flash bytes per phrase
#define FEEHA_DFLASH_BYTES_PHRS	0x10							// Data 	 Flash bytes per phrase
#define FEEHA_EEPROM_BYTES_PHRS	0x10							// EEprom        bytes per phrase

#define FEEHA_PFLASH_BYTES_WORD	0x08							// Program Flash bytes per word
#define FEEHA_DFLASH_BYTES_WORD	0x08							// Data 	 Flash bytes per word
#define FEEHA_EEPROM_BYTES_WORD	0x01							// EEprom        bytes per word

#define FEEHA_PFLASH_SCTR_WORDS	(FEEHA_PFLASH_SCTR_BYTES / FEEHA_PFLASH_BYTES_WORD)
#define FEEHA_DFLASH_SCTR_WORDS	(FEEHA_DFLASH_SCTR_BYTES / FEEHA_DFLASH_BYTES_WORD)
#define FEEHA_EEPROM_SCTR_WORDS	(FEEHA_EEPROM_SCTR_BYTES / FEEHA_EEPROM_BYTES_WORD)

#define FEEHA_PFLASH_SCTR_PHRS  (FEEHA_PFLASH_SCTR_BYTES / FEEHA_PFLASH_BYTES_PHRS)
#define FEEHA_DFLASH_SCTR_PHRS  (FEEHA_DFLASH_SCTR_BYTES / FEEHA_DFLASH_BYTES_PHRS)
#define FEEHA_EEPROM_SCTR_PHRS  (FEEHA_EEPROM_SCTR_BYTES / FEEHA_EEPROM_BYTES_PHRS)

#endif //BUILD_SAM3X8E


#ifdef BUILD_MK60
#define FEEHA_nReg8Set 																																																\
{																																																										\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT3)), (uint8)FEE_PFLASH_PRT3, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT2)), (uint8)FEE_PFLASH_PRT2, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT1)), (uint8)FEE_PFLASH_PRT1, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT0)), (uint8)FEE_PFLASH_PRT0, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FDPROT)), (uint8)FEE_DFLASH_PROT, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FEPROT)), (uint8)FEE_EFLASH_PROT, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }																		\
}
#endif

#ifdef BUILD_MK64
#define FEEHA_nReg8Set 																																																\
{																																																										\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT3)), (uint8)FEEHA_PFLASH_PRT3, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT2)), (uint8)FEEHA_PFLASH_PRT2, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT1)), (uint8)FEEHA_PFLASH_PRT1, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT0)), (uint8)FEEHA_PFLASH_PRT0, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FDPROT)), (uint8)FEEHA_DFLASH_PROT, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FEPROT)), (uint8)FEEHA_EFLASH_PROT, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }																		\
}
#endif //BUILD_MK64

#ifdef BUILD_SAM3X8E
#define FEEHA_nReg8Set                   \
{                                        \
	{ NULL, 0, REGSET_enOverwrite }      \
}
#endif

bool FEEHA_boStart(uint32* const);
void FEEHA_vRun(uint32* const);
void FEEHA_vTerminate(uint32* const);
void FEEHA_vStartSBL(void);
bool FEEHA_boCheckPartition(void);
bool FEEHA_boSetWorkingData(puint8, uint16);
bool FEEHA_boNVMWorkingCopy(bool, bool);
bool FEEHA_boNVMClear(void);
bool FEEHA_boPartition(void);
bool FEEHA_boWriteControlBlock(COMMONNL_tstRXLargeBuffer* const, 
															 uint8* const, 
															 uint32);
bool FEEHA_boUpdateControlBlock(uint32);
bool FEEHA_boEraseForDownload(puint8, uint32);
bool FEEHA_boWriteNVM(puint8, puint8, uint32);
bool FEEHA_boWriteSector(void);
#endif //FEEHA_H

