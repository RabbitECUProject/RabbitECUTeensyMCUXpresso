/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2012, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FTFx_KX_512K_0K_0K_2K_0K.h                           *
* DATE          :  June 19, 2012                                        *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b39392@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.19.2012      FPT Team      Initial Version
*************************************************************************/

#ifndef _FTFx_KX_512K_0K_0K_2K_0K_H_
#define _FTFx_KX_512K_0K_0K_2K_0K_H_

#include "SSD_FTFx_Common.h"

#define BYTE2WORD(x) (x)

/* Endianness */
#define ENDIANNESS              LITTLE_ENDIAN

/* cpu core */
#define CPU_CORE                ARM_CM4
/* PFlash sector size */ 
#define FTFx_PSECTOR_SIZE       0x00000800      /* 2 KB size */
/* DFlash sector size */ 
#define FTFx_DSECTOR_SIZE       0x00000000      /* 0 KB size */
#define DEBLOCK_SIZE            0x00000000      /* 0 KB size */
/*destination to read Dflash IFR area*/
#define DFLASH_IFR_READRESOURCE_ADDRESS   0x8000FC 

/* Address offset and size of PFlash IFR and DFlash IFR */
#define PFLASH_IFR_OFFSET                 0x00000000
#define PFLASH_IFR_SIZE                   0x00000100
#define DFLASH_IFR_OFFSET                 0xFFFFFFFF /* reserved */
#define DFLASH_IFR_SIZE                   0xFFFFFFFF /* reserved */
      
/* Size for checking alignment of a section */
#define ERSBLK_ALIGN_SIZE       FTFx_LONGWORD_SIZE      /* check align of erase block function */
#define PGMCHK_ALIGN_SIZE       FTFx_LONGWORD_SIZE      /* check align of program check function */
#define PPGMSEC_ALIGN_SIZE      FTFx_PHRASE_SIZE        /* check align of program section function */
#define DPGMSEC_ALIGN_SIZE      FTFx_PHRASE_SIZE        /* check align of program section function */
#define RD1BLK_ALIGN_SIZE       FTFx_LONGWORD_SIZE      /* check align of verify block function */
#define PRD1SEC_ALIGN_SIZE      FTFx_PHRASE_SIZE        /* check align of verify section function */
#define DRD1SEC_ALIGN_SIZE      FTFx_PHRASE_SIZE        /* check align of verify section function */
#define SWAP_ALIGN_SIZE         FTFx_LONGWORD_SIZE      /* check align of swap function*/
#define RDRSRC_ALIGN_SIZE       FTFx_LONGWORD_SIZE      /* check align of read resource function */
#define RDONCE_INDEX_MAX        0xF                     /* maximum index in read once command */


/* -------------------- Function Pointer ------------------------------- */
typedef UINT32 (*pFLASHCOMMANDSEQUENCE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                         UINT8 index, \
                                         UINT8* pCommandArray);

/* Flash initialization */
typedef UINT32 (*pFLASHINIT) (PFLASH_SSD_CONFIG PSSDConfig);

/* P-Flash get protection */
typedef UINT32 (*pPFLASHGETPROTECTION) (PFLASH_SSD_CONFIG PSSDConfig, \
                                        UINT32*  protectStatus);
                                              
/* P-Flash set protection */
typedef UINT32 (*pPFLASHSETPROTECTION) (PFLASH_SSD_CONFIG  PSSDConfig, \
                                        UINT32  protectStatus);
                                                        
/* Flash get interrupt enable */
typedef UINT32 (*pFLASHGETINTERRUPTENABLE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                            UINT8* interruptState);

/* Flash set interrupt enable */
typedef UINT32 (*pFLASHSETINTERRUPTENABLE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                            UINT8 interruptState);

/* Flash get security state */
typedef UINT32 (*pFLASHGETSECURITYSTATE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                          UINT8* securityState);
                                                        
/* Flash security bypass */
typedef UINT32 (*pFLASHSECURITYBYPASS) (PFLASH_SSD_CONFIG PSSDConfig, \
                                        UINT8* keyBuffer, \
                                        pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash erase all Block */
typedef UINT32 (*pFLASHERASEALLBLOCK) (PFLASH_SSD_CONFIG PSSDConfig, \
                                       pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                
/* Flash erase Block */
typedef UINT32 (*pFLASHERASEBLOCK) (PFLASH_SSD_CONFIG PSSDConfig, \
                                    UINT32 destination, \
                                    pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                    
/* Flash erase sector */
typedef UINT32 (*pFLASHERASESECTOR) (PFLASH_SSD_CONFIG PSSDConfig, \
                                     UINT32 destination, \
                                     UINT32 size, \
                                     pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash erase suspend */
typedef UINT32 (*pFLASHERASESUSPEND) (PFLASH_SSD_CONFIG PSSDConfig);

/* Flash erase resume */
typedef UINT32 (*pFLASHERASERESUME) (PFLASH_SSD_CONFIG PSSDConfig);                                                       
    
/* Flash program Section */
typedef UINT32 (*pFLASHPROGRAMSECTION) (PFLASH_SSD_CONFIG PSSDConfig, \
                                        UINT32 destination, \
                                        UINT16 Number, \
                                        pFLASHCOMMANDSEQUENCE FlashCommandSequence);                                                
                                   
/* Flash checksum */
typedef UINT32 (*pFLASHCHECKSUM) (PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT32 destination, \
                                  UINT32 size, \
                                  UINT32* PSum);
                                           
/* Flash verify all block */
typedef UINT32 (*pFLASHVERIFYALLBLOCK) (PFLASH_SSD_CONFIG PSSDConfig, \
                                        UINT8 marginLevel, \
                                        pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash verify block */
typedef UINT32 (*pFLASHVERIFYBLOCK) (PFLASH_SSD_CONFIG PSSDConfig, \
                                     UINT32 destination, \
                                     UINT8 marginLevel, \
                                     pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash verify section*/
typedef UINT32 (*pFLASHVERIFYSECTION) (PFLASH_SSD_CONFIG PSSDConfig, \
                                       UINT32 destination, \
                                       UINT16 Number, \
                                       UINT8 marginLevel, \
                                       pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash read once */
typedef UINT32 (*pFLASHREADONCE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT8* pDataArray, \
                                  pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash program once */
typedef UINT32 (*pFLASHPROGRAMONCE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                     UINT8* pDataArray, \
                                     pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash program check */
typedef UINT32 (*pFLASHPROGRAMCHECK) (PFLASH_SSD_CONFIG PSSDConfig, \
                                      UINT32  destination, \
                                      UINT32  size, \
                                      UINT8*  pExpectedData, \
                                      UINT32* pFailAddr, \
                                      UINT8*  pFailData, \
                                      UINT8   marginLevel, \
                                      pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                                      
/* Flash read resource */
typedef UINT32 (*pFLASHREADRESOURCE) (PFLASH_SSD_CONFIG PSSDConfig, \
                                      UINT32 destination, \
                                      UINT8* pDataArray, \
                                      pFLASHCOMMANDSEQUENCE FlashCommandSequence);                                                                                    

/* Flash program Longword */
typedef UINT32 (*pFLASHPROGRAMLONGWORD) (PFLASH_SSD_CONFIG PSSDConfig, \
                                         UINT32 destination, \
                                         UINT32 size, \
                                         UINT32 source, \
                                         pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* PFlash Get swap status */
typedef UINT32 (*pPFLASHGETSWAPSTATUS) (PFLASH_SSD_CONFIG PSSDConfig, \
                                        UINT32 flashAddress, \
                                        UINT8* pCurrentSwapMode, \
                                        UINT8* pCurrentSwapBlockStatus, \
                                        UINT8* pNextSwapBlockStatus, \
                                        pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Swap the two P-Flash blocks with each other */
typedef UINT32 (*PFLASHSWAP)(PFLASH_SSD_CONFIG PSSDConfig, \
                             UINT32 flashAddress, \
                             PFLASH_SWAP_CALLBACK pSwapCallback, \
                             pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                         
/*---------------- Function Prototypes for Flash SSD --------------------*/

/* Flash initialization */
extern UINT32 FlashInit(PFLASH_SSD_CONFIG PSSDConfig);

/* Internal function. Called by driver APIs only */
/* Flash command sequence */
extern UINT32 FlashCommandSequence(PFLASH_SSD_CONFIG PSSDConfig, \
                                   UINT8 index, \
                                   UINT8* pCommandArray);

/* P-Flash get protection */
extern UINT32 PFlashGetProtection(PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT32*  protectStatus);      

/* P-Flash set protection */
extern UINT32 PFlashSetProtection(PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT32  protectStatus);                                                
                                                
/* Flash get interrupt enable */
extern UINT32 FlashGetInterruptEnable(PFLASH_SSD_CONFIG PSSDConfig, \
                                      UINT8* interruptState);

/* Flash set interrupt enable */
extern UINT32 FlashSetInterruptEnable(PFLASH_SSD_CONFIG PSSDConfig, \
                                      UINT8 interruptState);

/* Flash get security state */
extern UINT32 FlashGetSecurityState(PFLASH_SSD_CONFIG PSSDConfig, \
                                    UINT8* securityState);

/* Flash security bypass */
extern UINT32 FlashSecurityBypass(PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT8* keyBuffer, \
                                  pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash erase all Block */
extern UINT32 FlashEraseAllBlock(PFLASH_SSD_CONFIG PSSDConfig, \
                                 pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                
/* Flash erase Block */
extern UINT32 FlashEraseBlock(PFLASH_SSD_CONFIG PSSDConfig, \
                                   UINT32 destination, \
                                   pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                    
/* Flash erase sector */
extern UINT32 FlashEraseSector(PFLASH_SSD_CONFIG PSSDConfig, \
                               UINT32 destination, \
                               UINT32 size, \
                               pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash erase suspend */
extern UINT32 FlashEraseSuspend(PFLASH_SSD_CONFIG PSSDConfig);

/* Flash erase resume */
extern UINT32 FlashEraseResume(PFLASH_SSD_CONFIG PSSDConfig);
                                                
/* Flash program Section */
extern UINT32 FlashProgramSection(PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT32 destination, \
                                  UINT16 Number, \
                                  pFLASHCOMMANDSEQUENCE FlashCommandSequence);                                                
   
/* Flash checksum */
extern UINT32 FlashCheckSum(PFLASH_SSD_CONFIG PSSDConfig, \
                            UINT32 destination, \
                            UINT32 size, \
                            UINT32* PSum);
                                           
/* Flash verify all block */
extern UINT32 FlashVerifyAllBlock(PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT8 marginLevel, \
                                  pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash verify block */
extern UINT32 FlashVerifyBlock(PFLASH_SSD_CONFIG PSSDConfig, \
                               UINT32 destination, \
                               UINT8 marginLevel, \
                               pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash verify section*/
extern UINT32 FlashVerifySection(PFLASH_SSD_CONFIG PSSDConfig, \
                                 UINT32 destination, \
                                 UINT16 Number, \
                                 UINT8 marginLevel, \
                                 pFLASHCOMMANDSEQUENCE FlashCommandSequence);        

/* Flash read once */
extern UINT32 FlashReadOnce(PFLASH_SSD_CONFIG PSSDConfig, \
                            UINT8 recordIndex,\
                            UINT8* pDataArray, \
                            pFLASHCOMMANDSEQUENCE FlashCommandSequence);

/* Flash program once */
extern UINT32 FlashProgramOnce(PFLASH_SSD_CONFIG PSSDConfig, \
                               UINT8 recordIndex,\
                               UINT8* pDataArray, \
                               pFLASHCOMMANDSEQUENCE FlashCommandSequence);
/* Flash program check */
extern UINT32 FlashProgramCheck(PFLASH_SSD_CONFIG PSSDConfig, \
                                UINT32  destination, \
                                UINT32  size, \
                                UINT8*  pExpectedData, \
                                UINT32* pFailAddr, \
                                UINT8*  pFailData, \
                                UINT8   marginLevel, \
                                pFLASHCOMMANDSEQUENCE FlashCommandSequence);
                                        
/* Flash read resource */
extern UINT32 FlashReadResource(PFLASH_SSD_CONFIG PSSDConfig, \
                                UINT32 destination, \
                                UINT8* pDataArray, \
                                pFLASHCOMMANDSEQUENCE FlashCommandSequence); 
                                      
/* Flash program Longword */
extern UINT32 FlashProgramLongword(PFLASH_SSD_CONFIG PSSDConfig, \
                                   UINT32 destination, \
                                   UINT32 size, \
                                   UINT32 source, \
                                   pFLASHCOMMANDSEQUENCE FlashCommandSequence);         
       
/* Get Swap status of P-Flash */
extern UINT32 PFlashGetSwapStatus(PFLASH_SSD_CONFIG PSSDConfig, \
                                  UINT32 flashAddress, \
                                  UINT8* pCurrentSwapMode, \
                                  UINT8* pCurrentSwapBlockStatus, \
                                  UINT8* pNextSwapBlockStatus, \
                                  pFLASHCOMMANDSEQUENCE FlashCommandSequence);
/* Swap the two P-Flash blocks with each other */
extern UINT32 PFlashSwap(PFLASH_SSD_CONFIG PSSDConfig, \
                         UINT32 flashAddress, \
                         PFLASH_SWAP_CALLBACK pSwapCallback, \
                         pFLASHCOMMANDSEQUENCE FlashCommandSequence);

#endif  /* _FTFx_KX_512K_0K_0K_2K_0K_H_ */

