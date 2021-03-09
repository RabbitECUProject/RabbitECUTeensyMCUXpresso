/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2012, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  SSD_FTFx_Common.h                                    *
* DATE          :  Jun 20, 2012                                         *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b39392@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.20.2012      FPT Team      Initial Version
*************************************************************************/
#ifndef _SSD_FTFx_COMMON_H_
#define _SSD_FTFx_COMMON_H_
#include "SSD_Types.h"
/*------------------------- Configuration Macros -----------------------*/
/* Define derivatives with rule: FTFx_KX_Pblocksize_Dblocksize_EERAMsize_Psectorsize_Dsectorsize */

#define FTFx_KX_256K_256K_4K_2K_2K        1    /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_512K_0K_0K_2K_0K          2    /* Kinetis - ARM Cortex M4 core */
#define FTFx_JX_128K_32K_2K_1K_1K         3    /* ColdFire core */
#define FTFx_FX_256K_32K_2K_1K_1K         4    /* ColdFire core */
#define FTFx_KX_512K_512K_16K_4K_4K       5    /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_1024K_0K_16K_4K_0K        6    /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_32K_0K_0K_1K_0K           7    /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_32K_32K_2K_1K_1K          8    /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_64K_0K_0K_1K_0K           9    /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_64K_32K_2K_1K_1K          10   /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_128K_0K_0K_1K_0K          11   /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_128K_32K_2K_1K_1K         12   /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_64K_32K_2K_2K_1K          13   /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_128K_32K_2K_2K_1K         14   /* Kinetis - ARM Cortex M4 core */
#define FTFx_KX_256K_32K_2K_2K_1K         15   /* Kinetis - ARM Cortex M4 core */
#define FTFx_NX_256K_32K_2K_2K_1K         16   /* Nevis2 - 56800EX 32 bit DSC core */
#define FTFx_NX_128K_32K_2K_2K_1K         17   /* Nevis2 - 56800EX 32 bit DSC core */
#define FTFx_NX_96K_32K_2K_2K_1K          18   /* Nevis2 - 56800EX 32 bit DSC core */
#define FTFx_NX_64K_32K_2K_2K_1K          19   /* Nevis2 - 56800EX 32 bit DSC core */
#define FTFx_LX_128K_0K_0K_1K_0K          20   /* L2K -  ARM Cortex M0 core */
#define FTFx_LX_64K_0K_0K_1K_0K           21   /* L2K -  ARM Cortex M0 core */
#define FTFx_LX_32K_0K_0K_1K_0K           22   /* L2K & L1PT -  ARM Cortex M0 core */
#define FTFx_LX_16K_0K_0K_1K_0K           23   /* L1PT -  ARM Cortex M0 core */
#define FTFx_LX_8K_0K_0K_1K_0K            24   /* L1PT -  ARM Cortex M0 core */
#define FTFx_AX_64K_0K_0K_1K_0K           25   /* Anguilla_Silver - 56800EX 32 bit DSC core */
#define FTFx_AX_48K_0K_0K_1K_0K           26   /* Anguilla_Silver - 56800EX 32 bit DSC core */
#define FTFx_AX_32K_0K_0K_1K_0K           27   /* Anguilla_Silver - 56800EX 32 bit DSC core */
#define FTFx_AX_16K_0K_0K_1K_0K           28   /* Anguilla_Silver - 56800EX 32 bit DSC core */

/* Define compiler */
#define CW                                0   /* CodeWarrior Compiler */
#define IAR                               1   /* IAR Compiler */

/* Endianness */
#define BIG_ENDIAN                        0   /* Big Endian */
#define LITTLE_ENDIAN                     1   /* Little Endian */

/* cpu cores */
#define COLDFIRE                          0   /* ColdFire core */
#define ARM_CM4                           1   /* ARM Cortex M4 core */
#define DSC_56800EX                       2   /* 32 bit DSC core */
#define ARM_CM0PLUS                       3     /* ARM Cortex M0 core */

/* Word size */
#define FTFx_WORD_SIZE                    0x0002     /* 2 bytes */
  
/* Longword size */
#define FTFx_LONGWORD_SIZE                0x0004     /* 4 bytes */

/* Phrase size */
#define FTFx_PHRASE_SIZE                  0x0008     /* 8 bytes */

/* Double-phrase size */
#define FTFx_DPHRASE_SIZE                 0x0010     /* 16 bytes */

/*------------ Return Code Definition for FTFx SSD ---------------------*/
#define FTFx_OK                            0x0000
#define FTFx_ERR_SIZE                      0x0001
#define FTFx_ERR_RANGE                     0x0002
#define FTFx_ERR_ACCERR                    0x0004
#define FTFx_ERR_PVIOL                     0x0008
#define FTFx_ERR_MGSTAT0                   0x0010
#define FTFx_ERR_CHANGEPROT                0x0020
#define FTFx_ERR_EEESIZE                   0x0040
#define FTFx_ERR_EFLASHSIZE                0x0080
#define FTFx_ERR_ADDR                      0x0100
#define FTFx_ERR_NOEEE                     0x0200
#define FTFx_ERR_EFLASHONLY                0x0400
#define FTFx_ERR_DFLASHONLY                0x0800
#define FTFx_ERR_RDCOLERR                  0x1000
#define FTFx_ERR_RAMRDY                    0x2000

/* Flash security status */
#define FLASH_SECURITY_STATE_KEYEN         0x80
#define FLASH_SECURITY_STATE_UNSECURED     0x02

#define FLASH_NOT_SECURE                   0x01
#define FLASH_SECURE_BACKDOOR_ENABLED      0x02
#define FLASH_SECURE_BACKDOOR_DISABLED     0x04

/* macro for flash configuration field offset */
#define FLASH_CNFG_START_ADDRESS           0x400
#define FLASH_CNFG_END_ADDRESS             0x40F

/* EERAM Function Control Code */
#define EEE_ENABLE                         0x00
#define EEE_DISABLE                        0xFF

/*-------------- Read/Write/Set/Clear Operation Macros -----------------*/
#define REG_BIT_SET(address, mask)      (*(VUINT8*)(address) |= (mask))
#define REG_BIT_CLEAR(address, mask)    (*(VUINT8*)(address) &= ~(mask))
#define REG_BIT_TEST(address, mask)     (*(VUINT8 *)(address) & (UINT8)(mask))
#define REG_WRITE(address, value)       (*(VUINT8*)(address) = (value))
#define REG_READ(address)               ((UINT8)(*(VUINT8*)(address)))
#define REG_WRITE32(address, value)     (*(VUINT32*)(address) = (value))
#define REG_READ32(address)             ((UINT32)(*(VUINT32*)(address)))

#define WRITE8(address, value)          (*(VUINT8*)(address) = (value))
#define READ8(address)                  ((UINT8)(*(VUINT8*)(address)))
#define SET8(address, value)            (*(VUINT8*)(address) |= (value))
#define CLEAR8(address, value)          (*(VUINT8*)(address) &= ~(value))
#define TEST8(address, value)           (*(VUINT8*)(address) & (value))

#define WRITE16(address, value)         (*(VUINT16*)(address) = (value))
#define READ16(address)                 ((UINT16)(*(VUINT16*)(address)))
#define SET16(address, value)           (*(VUINT16*)(address) |= (value))
#define CLEAR16(address, value)         (*(VUINT16*)(address) &= ~(value))
#define TEST16(address, value)          (*(VUINT16*)(address) & (value))

#define WRITE32(address, value)         (*(VUINT32*)(address) = (value))
#define READ32(address)                 ((UINT32)(*(VUINT32*)(address)))
#define SET32(address, value)           (*(VUINT32*)(address) |= (value))
#define CLEAR32(address, value)         (*(VUINT32*)(address) &= ~(value))
#define TEST32(address, value)          (*(VUINT32*)(address) & (value))

/*--------------------- CallBack function period -----------------------*/
#define FLASH_CALLBACK_CS               1          /* Check Sum */

/*-------------------- Callback function prototype ---------------------*/
typedef void (* PCALLBACK)(void);
typedef BOOL (* PFLASH_SWAP_CALLBACK)(UINT8);

/*--------------------Null Callback function defination ----------------*/
#define NULL_CALLBACK                   ((PCALLBACK)0xFFFFFFFF)
#define NULL_SWAP_CALLBACK              ((PFLASH_SWAP_CALLBACK)0xFFFFFFFF)

/*---------------- Flash SSD Configuration Structure -------------------*/
typedef struct _ssd_config
{
    UINT32      ftfxRegBase;        /* FTFx control register base */
    UINT32      PFlashBlockBase;    /* base address of PFlash block */
    UINT32      PFlashBlockSize;    /* size of PFlash block */
    UINT32      DFlashBlockBase;    /* base address of DFlash block */
    UINT32      DFlashBlockSize;    /* size of DFlash block */
    UINT32      EERAMBlockBase;     /* base address of EERAM block */
    UINT32      EERAMBlockSize;     /* size of EERAM block */
    UINT32      EEEBlockSize;       /* size of EEE block */
    BOOL        DebugEnable;        /* background debug mode enable bit */
    PCALLBACK   CallBack;           /* pointer to callback function */
} FLASH_SSD_CONFIG, *PFLASH_SSD_CONFIG;

/* PFlash swap states */
#define FTFx_SWAP_UNINIT            0x00
#define FTFx_SWAP_READY             0x01
#define FTFx_SWAP_INIT              0x01
#define FTFx_SWAP_UPDATE            0x02
#define FTFx_SWAP_UPDATE_ERASED     0x03
#define FTFx_SWAP_COMPLETE          0x04
                                      
#endif /* _SSD_FTFx_COMMON_H_ */

