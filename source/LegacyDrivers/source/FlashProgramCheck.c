/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashProgramCheck.c                                  *
* DATE          :  Dec 27,2011                                          *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b39392@freescale.com                                   *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.09.2010      FPT Team      Initial Version
0.1.0       06.11.2010      FPT Team      Finalize to 0.1.0 
0.1.3       09.16.2010      FPT Team      Updated to support little endian,
                                          changed returnCode of checking 
                                          size alignment from 
                                          FTFx_ERR_ADDR to FTFx_ERR_SIZE
0.2.0       09.27.2010      FPT Team      Finalize to 0.2.0
0.2.1       01.28.2011      FPT Team      Updated the preprocesor 
                                          condition to enter Debug state
0.2.2       09.15.2011      FPT Team      Update to support K70: do not get 
                                          failedData from FCCOB register
0.2.3       11.15.2011     FPT Team       Updated to support FTFx_KX_1024K_0K_16K_4K_0K 
                                          derivative.
0.2.4       12.27.2011     FPT Team       Update to support more Kinetis derivatives.
0.3.1       05.16.2012     FPT Team       Update to support Nevis2 devices
0.3.2       07.17.2012     FPT Team       Update to support L2K and L1PT derivatives
0.3.3		08.03.2012		FPT Team	  Optimized for L2K/L1PT
										  Update to support Anguilla Silver derivatives
*************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"

/************************************************************************
*
*  Function Name    : FlashProgramCheck.c
*  Description      : The Program Check command tests a previously 
*                     programmed P-Flash or D-Flash longword to see 
*                     if it reads correctly at the specifed margin level.
*  Arguments        : PFLASH_SSD_CONFIG, UINT32,UINT32, UINT8*, UINT32*,
*                     UINT8*, UINT8, pFLASHCOMMANDSEQUENCE
*  Return Value     : UINT32
*
*************************************************************************/
/* Enable size optimization for Anguilla Silver and Nevis2 derivatives */
#if((CPU_CORE == DSC_56800EX)&&(COMPILER == CW))
#pragma optimize_for_size on
#pragma optimization_level 4
#endif
/* Enable size optimization for L2K and L1PT derivatives */
#if((CPU_CORE == ARM_CM0PLUS)&&(COMPILER == CW))
UINT32 __attribute__((optimize("O4"))) FlashProgramCheck(PFLASH_SSD_CONFIG PSSDConfig, \
                                                UINT32  destination, \
                                                UINT32  size, \
                                                UINT8*  pExpectedData, \
                                                UINT32* pFailAddr, \
                                                UINT8*  pFailData, \
                                                UINT8   marginLevel, \
                                                pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#else
UINT32 FlashProgramCheck(PFLASH_SSD_CONFIG PSSDConfig, \
												UINT32  destination, \
												UINT32  size, \
												UINT8*  pExpectedData, \
												UINT32* pFailAddr, \
												UINT8*  pFailData, \
												UINT8   marginLevel, \
												pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#endif                                                
{
    UINT8 pCommandArray[12]; /* command sequence array */
    UINT32 returnCode;      /* return code variable */
#if DSC_56800EX == CPU_CORE
    UINT8 PFlashFlag;
#endif    
    
    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
    
    /* check if the destination is Longword aligned or not */
    if ((destination & (PGMCHK_ALIGN_SIZE - 1)))
    {
        /* return an error code FTFx_ERR_ADDR */
        returnCode = FTFx_ERR_ADDR;
        goto EXIT;
    }
    
    /* check if the size is Longword aligned or not */
#if DSC_56800EX == CPU_CORE    
    if (size & (2*PGMCHK_ALIGN_SIZE - 1))
#else
    if (size & (PGMCHK_ALIGN_SIZE - 1))
#endif    	
    {
        /* return an error code FTFx_ERR_ADDR */
        returnCode = FTFx_ERR_SIZE;
        goto EXIT;
    }

    /* check for valid range of the target addresses */    
    if((destination < PSSDConfig->PFlashBlockBase) || \
        ((destination+BYTE2WORD(size)) > (PSSDConfig->PFlashBlockBase + BYTE2WORD(PSSDConfig->PFlashBlockSize))))
    {
#if(DEBLOCK_SIZE)    	
        if((destination < PSSDConfig->DFlashBlockBase) || \
            ((destination+BYTE2WORD(size)) > (PSSDConfig->DFlashBlockBase + BYTE2WORD(PSSDConfig->DFlashBlockSize))))
        {
#endif /* End of #if(DEBLOCK_SIZE) */         	
            /* return an error code FTFx_ERR_RANGE */
            returnCode = FTFx_ERR_RANGE;
            goto EXIT;
#if (DEBLOCK_SIZE)            
        }
        else
        {
            /* Convert System memory address to FTFx internal memory address */
	#if DSC_56800EX == CPU_CORE
			destination = (2*(destination - PSSDConfig->DFlashBlockBase)+ 0x800000);
			PFlashFlag = 0;
	#else    	
			destination = destination - PSSDConfig->DFlashBlockBase + 0x800000;
	#endif  
        }
#endif /* End of #if(DEBLOCK_SIZE) */        
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
#if DSC_56800EX == CPU_CORE
    	destination = (2*(destination - PSSDConfig->PFlashBlockBase));
    	PFlashFlag = 1;
#else    	
    	destination -= PSSDConfig->PFlashBlockBase;
#endif 
    }
    while (size)
    {
        /* check for error return code */
        if(FTFx_OK == returnCode)
        {
            /* preparing passing parameter to program check the flash block */
            pCommandArray[0] = FTFx_PROGRAM_CHECK;
            pCommandArray[1] = (UINT8)(destination >> 16);
            pCommandArray[2] = (UINT8)((destination >> 8) & 0xFF);
            pCommandArray[3] = (UINT8)(destination & 0xFF);
            pCommandArray[4] = marginLevel;        
#if (ENDIANNESS == BIG_ENDIAN)  /* Big Endian */
            pCommandArray[8] = *pExpectedData;
            pCommandArray[9] = *(pExpectedData+1);
            pCommandArray[10] = *(pExpectedData+2);
            pCommandArray[11] = *(pExpectedData+3);
#else /* Little Endian */
            pCommandArray[8] = *(pExpectedData+3);
            pCommandArray[9] = *(pExpectedData+2);
            pCommandArray[10] = *(pExpectedData+1);
            pCommandArray[11] = *pExpectedData;
#endif
            /* calling flash command sequence function to execute the command */
            returnCode = FlashCommandSequence(PSSDConfig, 11, pCommandArray);
            
            /* checking for the success of command execution */
            if(FTFx_OK != returnCode)
            {
#if DSC_56800EX == CPU_CORE
            	/* Convert to global address in Nevis2 */
            	if(PFlashFlag)
            	{
            		*pFailAddr = (destination >>1) + PSSDConfig->PFlashBlockBase;
            	}
#if (DEBLOCK_SIZE) 
            	else
            	{
            		*pFailAddr = (destination>>1) + PSSDConfig->DFlashBlockBase - 0x400000;
            	}
#endif
#else            	
            	*pFailAddr  =  destination;
#endif
            	/* Read fail returned data: if K70, Nevis2, L1PT, L2K are selected */
#if ((FTFx_KX_512K_512K_16K_4K_4K == FLASH_DERIVATIVE) || (FTFx_KX_1024K_0K_16K_4K_0K == FLASH_DERIVATIVE)\
		||(FTFx_NX_256K_32K_2K_2K_1K == FLASH_DERIVATIVE)||(FTFx_NX_128K_32K_2K_2K_1K == FLASH_DERIVATIVE)\
		||(FTFx_NX_96K_32K_2K_2K_1K == FLASH_DERIVATIVE)||(FTFx_NX_64K_32K_2K_2K_1K == FLASH_DERIVATIVE)\
        ||(FTFx_LX_128K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_LX_96K_0K_0K_1K_0K == FLASH_DERIVATIVE)\
        ||(FTFx_LX_64K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_LX_32K_0K_0K_1K_0K == FLASH_DERIVATIVE)\
        ||(FTFx_LX_8K_0K_0K_1K_0K == FLASH_DERIVATIVE))
                *pFailData = 0x0;
                *(pFailData+1) = 0x0;
                *(pFailData+2) = 0x0;
                *(pFailData+3) = 0x0;
#else /*other derivative */               
#if (ENDIANNESS == BIG_ENDIAN)  /* Big Endian */  
                *pFailData = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB4_OFFSET);
                *(pFailData+1) = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB5_OFFSET);
                *(pFailData+2) = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB6_OFFSET);
                *(pFailData+3) = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB7_OFFSET);    
#else /* Little Endian */
                *(pFailData+3) = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB4_OFFSET);
                *(pFailData+2) = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB5_OFFSET);
                *(pFailData+1) = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB6_OFFSET);
                *pFailData = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB7_OFFSET);
#endif /*of ENDIANNESS */
#endif /* of FLASH_DERIVATIVE */
                goto EXIT;
            }
        }
        size-=FTFx_LONGWORD_SIZE;
        pExpectedData+=FTFx_LONGWORD_SIZE;
        destination+= FTFx_LONGWORD_SIZE;
    }

EXIT:
    /* Enter Debug state if enabled */
    if (TRUE == (PSSDConfig->DebugEnable))
    {
#if ((CPU_CORE == ARM_CM4) && (COMPILER == IAR))    /* Kx Products */
        asm
        (
            " BKPT #0 \n "           /* enter Debug state */
        );
#endif
#if ((CPU_CORE == DSC_56800EX)&&(COMPILER == CW))     
        asm
        (
        		debughlt           /* enter Debug state */
        );
#endif 
    }

    return(returnCode);
}
#if((CPU_CORE == DSC_56800EX)&&(COMPILER == CW))
#pragma optimize_for_size reset
#endif
/* end of file */


