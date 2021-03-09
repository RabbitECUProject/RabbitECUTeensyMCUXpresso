/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  NormalDemo.h                                         *
* DATE          :  February 10,2011                                     *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b28216@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       09.21.2010      FPT Team        Initial Version
0.1.0       02.10.2011      FPT Team        Update to support K40 and K60
*************************************************************************/

#ifndef _NORMALDEMO_H_
#define _NORMALDEMO_H_

// #if (FLASH_DERIVATIVE == FTFx_KX_256K_256K_4K_2K_2K)
//     #include "MK40N512VMD100.h"
// #else   /* FLASH_DERIVATIVE == FTFx_KX_512K_0K_0K_2K_0K */
//     #include "MK60N512VMD100.h"
// #endif

#define READ_NORMAL_MARGIN        0x00
#define READ_USER_MARGIN          0x01
#define READ_FACTORY_MARGIN       0x02

#define EE_ENABLE                 0x00
#define RAM_ENABLE                0xFF
#define DEBUGENABLE               0x00
#define PFLASH_IFR_ADDR           0x00000000
#define DFLASH_IFR_ADDR           0x00800000

#define SECTOR_SIZE               0x00000800 /* 2 KB size */
#define BUFFER_SIZE_BYTE          0x10

/* FTFL module base */
#define FTFx_REG_BASE           0x40020000
#define DFLASH_IFR_BASE         0x10000000      /* unused */
#define PFLASH_BLOCK_BASE       0x00000000
#define DEFLASH_BLOCK_BASE      0x10000000      /* There is not DFlash */
#define EERAM_BLOCK_BASE        0x14000000

#define PBLOCK_SIZE             0x00080000      /* 512 KB size */
#define EERAM_BLOCK_SIZE        0x00004000      /* 16 KB size */

#define pFlashInit                      (FlashInit)
#define pDEFlashPartition               (DEFlashPartition)
#define pSetEEEEnable                   (SetEEEEnable)
#define pPFlashSetProtection            (PFlashSetProtection)
#define pPFlashGetProtection            (PFlashGetProtection)
#define pFlashVerifySection             (FlashVerifySection)
#define pFlashVerifyBlock               (FlashVerifyBlock)
#define pFlashVerifyAllBlock            (FlashVerifyAllBlock)
#define pFlashSetInterruptEnable        (FlashSetInterruptEnable)
#define pFlashSecurityBypass            (FlashSecurityBypass)
#define pFlashReadResource              (FlashReadResource)
#define pFlashReadOnce                  (FlashReadOnce)
#define pFlashProgramSection            (FlashProgramSection)
#define pFlashProgramOnce               (FlashProgramOnce)
#define pFlashProgramLongword           (FlashProgramLongword)
#define pFlashProgramCheck              (FlashProgramCheck)
#define pFlashGetSecurityState          (FlashGetSecurityState)
#define pFlashGetInterruptEnable        (FlashGetInterruptEnable)
#define pFlashEraseSuspend              (FlashEraseSuspend)
#define pFlashEraseBlock                (FlashEraseBlock)
#define pFlashEraseAllBlock             (FlashEraseAllBlock)
#define pFlashCommandSequence           (FlashCommandSequence)
#define pFlashCheckSum                  (FlashCheckSum)
#define pEERAMSetProtection             (EERAMSetProtection)
#define pEERAMGetProtection             (EERAMGetProtection)
#define pEEEWrite                       (EEEWrite)
#define pDFlashSetProtection            (DFlashSetProtection)
#define pDFlashGetProtection            (DFlashGetProtection)
#define pFlashEraseResume               (FlashEraseResume)
#define pFlashEraseSector               (FlashEraseSector)

void ErrorTrap(UINT32 returnCode);

#endif /* _ISRDEMO_H_ */
