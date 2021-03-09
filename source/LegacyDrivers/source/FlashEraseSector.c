/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashEraseSector.c                                   *
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
0.2.2       12.27.2011      FPT Team      Update to support more Kinetis derivatives.
0.3.1		05.16.2012 		FPT Team	  Update to support Nevis2 devices
0.3.3		08.03.2012		FPT Team	  Optimized for L2K/L1PT
										  Update to support Anguilla Silver derivatives
*************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"

/****************************************************************************
*
*  Function Name    : FlashEraseSector
*  Description      : Perform erase operation on Flash
*  Arguments        : PFLASH_SSD_CONFIG, UINT32, UINT32, pFLASHCOMMANDSEQUENCE
*  Return Value     : UINT32
*
*****************************************************************************/
#if((CPU_CORE == DSC_56800EX)&&(COMPILER == CW))
#pragma optimize_for_size on
#pragma optimization_level 4
#endif
/* Enable size optimization for L2K and L1PT derivatives */
#if((CPU_CORE == ARM_CM0PLUS)&&(COMPILER == CW))
UINT32 __attribute__((optimize("O4"))) FlashEraseSector(PFLASH_SSD_CONFIG PSSDConfig, \
                                   UINT32 destination, \
                                   UINT32 size, \
                                   pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#else
UINT32 FlashEraseSector(PFLASH_SSD_CONFIG PSSDConfig, \
								  UINT32 destination, \
								  UINT32 size, \
								  pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#endif                                                                      
{
    UINT8 pCommandArray[4]; /* command sequence array */
    UINT32 returnCode;      /* return code variable */
    UINT32 endAddress;      /* storing end address */
    UINT32 sectorSize;        /* size of one sector */
    
    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
    
    /* calculating Flash end address */
    endAddress = destination + BYTE2WORD(size);
    /* check for valid range of the target addresses */
    if((destination < PSSDConfig->PFlashBlockBase) || \
        (endAddress > (PSSDConfig->PFlashBlockBase + BYTE2WORD(PSSDConfig->PFlashBlockSize))))
    {
#if (DEBLOCK_SIZE)    	
        if((destination < PSSDConfig->DFlashBlockBase) || \
            (endAddress > (PSSDConfig->DFlashBlockBase + BYTE2WORD(PSSDConfig->DFlashBlockSize))))
        {
#endif /* End of if(DEBLOCK_SIZE) */ 
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
		#else    	
				destination = destination - PSSDConfig->DFlashBlockBase + 0x800000;
		#endif            
				sectorSize = FTFx_DSECTOR_SIZE;				
        }
#endif /* End of #if(DEBLOCK_SIZE) */        
    }
    else
    {
        /* Convert System memory address to FTFx internal memory address */
#if DSC_56800EX == CPU_CORE
    	destination = (2*(destination - PSSDConfig->PFlashBlockBase));
#else    	
    	destination -= PSSDConfig->PFlashBlockBase;
#endif        
        sectorSize = FTFx_PSECTOR_SIZE;
    }
    
    /* check if the destination is sector aligned or not */
    if (destination & (sectorSize-1))
    {
        /* return an error code FTFx_ERR_ADDR */
        returnCode = FTFx_ERR_ADDR;
        goto EXIT;
    }
    
    /* check if the size is sector alignment or not */
    if(size & (sectorSize-1))
    {
        /* return an error code FTFx_ERR_SIZE */
        returnCode = FTFx_ERR_SIZE;
        goto EXIT;
    }
        
    /* check for error return code */
    if(FTFx_OK == returnCode)
    {

        while(size > 0)
        {
            /* preparing passing parameter to erase a flash block */
            pCommandArray[0] = FTFx_ERASE_SECTOR;
            pCommandArray[1] = (UINT8)(destination >> 16);
            pCommandArray[2] = (UINT8)((destination >> 8) & 0xFF);
            pCommandArray[3] = (UINT8)(destination & 0xFF);
              
            /* calling flash command sequence function to execute the command */
            returnCode = FlashCommandSequence(PSSDConfig, 3, pCommandArray);
            
            /* checking the success of command execution */
            if(FTFx_OK != returnCode)
            {
                break;
            }
            else
            {
                /* update size and destination address */
                size -= sectorSize;
                destination += sectorSize;
            }
        }
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

