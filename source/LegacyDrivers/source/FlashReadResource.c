/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashReadResource.c                                  *
* DATE          :  Dec 27,2011                                          *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b39392@freescale.com                                   *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.09.2010      FPT Team      Initial Version
0.1.0       06.11.2010      FPT Team      Finalize to 0.1.0 
0.1.3       09.16.2010      FPT Team      Finalize to 0.1.3 
0.2.0       09.27.2010      FPT Team      Finalize to 0.2.0
0.2.1       01.28.2011      FPT Team      Updated the preprocesor 
                                          condition to enter Debug state
0.2.2       09.15.2011      FPT Team      Update to support K70
                                          Update the address alignment checking
                                          Update the range checking
                                          Add code to convert from system address to internal address
0.2.3       11.15.2011     FPT Team       Update to support FTFx_KX_1024K_0K_16K_4K_0K
                                          derivative.
0.2.4       12.27.2011     FPT Team       Update to support more Kinetis derivatives.
0.3.1		05.16.2012 		FPT Team	  Update to support Nevis2 devices
0.3.3		08.03.2012		FPT Team	  Optimize code size for L2K/L1PT
										  Nevis2, and Anguilla Silver   
*************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"

/************************************************************************
*
*  Function Name    : FlashReadResource.c
*  Description      : This function is provided for the user to read data
*                     from P-Flash IFR and D-Flash IFR space.
*  Arguments        : PFLASH_SSD_CONFIG, UINT32, UINT8*, pFLASHCOMMANDSEQUENCE
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
UINT32 __attribute__((optimize("O4")))FlashReadResource(PFLASH_SSD_CONFIG PSSDConfig, \
                                                UINT32 destination, \
                                                UINT8* pDataArray, \
                                                pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#else
UINT32 FlashReadResource(PFLASH_SSD_CONFIG PSSDConfig, \
                                                UINT32 destination, \
                                                UINT8* pDataArray, \
                                                pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#endif
{
    UINT8  pCommandArray[9]; /* command sequence array */
    UINT32 returnCode;       /* return code variable */
    
    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
        
    /* check if the destination is longword aligned or not */
    if (destination & (RDRSRC_ALIGN_SIZE-1))
    {
        /* return an error code FTFx_ERR_ADDR */
        returnCode = FTFx_ERR_ADDR;
        goto EXIT;
    }
    
    /* check for valid range of the target addresses */    
    if((destination < ( PSSDConfig->PFlashBlockBase + BYTE2WORD(PFLASH_IFR_OFFSET) )) || \
        (destination >= ( PSSDConfig->PFlashBlockBase + BYTE2WORD(PFLASH_IFR_SIZE ))))
    {
#if(DEBLOCK_SIZE)    	
        if((destination < ( PSSDConfig->DFlashBlockBase + BYTE2WORD(DFLASH_IFR_OFFSET) )) || \
            (destination >= ( PSSDConfig->DFlashBlockBase + BYTE2WORD(DFLASH_IFR_SIZE))))
        {
 
#endif /* End of #if(DEBLOCK_SIZE) */ 
        	/* return an error code FTFx_ERR_RANGE */
            returnCode = FTFx_ERR_RANGE;
            goto EXIT;
#if(DEBLOCK_SIZE) 
        }
        else
        {
             /* Convert System memory address to FTFx internal memory address */
#if DSC_56800EX == CPU_CORE
        	destination = (2*(destination - PSSDConfig->DFlashBlockBase)+ 0x800000);
#else    	
        	destination = destination - PSSDConfig->DFlashBlockBase + 0x800000;
#endif  
        }
#endif /* End of if(DEBLOCK_SIZE) */        
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
#if DSC_56800EX == CPU_CORE
    	destination = (2*(destination - PSSDConfig->PFlashBlockBase));
#else    	
    	destination -= PSSDConfig->PFlashBlockBase;
#endif 
    }

    /* preparing passing parameter for read resource command */
    /* 1st element for the FCCOB register */
    pCommandArray[0] = FTFx_READ_RESOURCE;
    pCommandArray[1] = (UINT8)(destination >> 16);
    pCommandArray[2] = (UINT8)((destination >> 8) & 0xFF);
    pCommandArray[3] = (UINT8)(destination & 0xFF);
#if ((FTFx_KX_512K_512K_16K_4K_4K == FLASH_DERIVATIVE) || (FTFx_KX_1024K_0K_16K_4K_0K == FLASH_DERIVATIVE))
    pCommandArray[4] = 0x00;
    /* calling flash command sequence API to execute the command */
    returnCode = FlashCommandSequence(PSSDConfig, 4, pCommandArray);
#else
    pCommandArray[8] = 0x00;
    /* calling flash command sequence API to execute the command */
    returnCode = FlashCommandSequence(PSSDConfig, 8, pCommandArray);
#endif    
        
    /* checking for the success of command execution */
    if(FTFx_OK != returnCode)
    {
        goto EXIT;
    }
    else
    {
        /* do nothing */
    }
    
    /* Read the data from the FCCOB registers into the pDataArray */
    pDataArray[0] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB4_OFFSET);
    pDataArray[1] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB5_OFFSET);
    pDataArray[2] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB6_OFFSET);
    pDataArray[3] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB7_OFFSET);
#if ((FTFx_KX_512K_512K_16K_4K_4K == FLASH_DERIVATIVE) || (FTFx_KX_1024K_0K_16K_4K_0K == FLASH_DERIVATIVE))
    pDataArray[4] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB8_OFFSET);
    pDataArray[5] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB9_OFFSET);
    pDataArray[6] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOBA_OFFSET);
    pDataArray[7] = REG_READ(PSSDConfig->ftfxRegBase + FTFx_SSD_FCCOBB_OFFSET);
#endif

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
