/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashProgramLongword.c                               *
* DATE          :  January 28,2011                                      *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b39392@freescale.com                                   *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.09.2010      FPT Team      Initial Version
0.1.0       06.11.2010      FPT Team      Finalize to 0.1.0 
0.1.3       09.16.2010      FPT Team      Updated to support little endian
0.2.0       09.27.2010      FPT Team      Finalize to 0.2.0
0.2.1       01.28.2011      FPT Team      Updated the preprocesor 
                                          condition to enter Debug state
0.3.1		05.16.2012 		FPT Team	  Update to support Nevis2 devices
0.3.3		08.03.2012		FPT Team	  Optimized for L2K/L1PT
										  Update to support Anguilla Silver derivatives
*************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"

/************************************************************************
*
*  Function Name    : FlashProgramLongword.c
*  Description      : Program data into Flash
*  Arguments        : PFLASH_SSD_CONFIG, UINT32, UINT32, UINT32,
*                     pFLASHCOMMANDSEQUENCE
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
UINT32 __attribute__((optimize("O4"))) FlashProgramLongword(PFLASH_SSD_CONFIG PSSDConfig, \
                                       UINT32 destination, \
                                       UINT32 size, \
                                       UINT32 source, \
                                       pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#else
UINT32 FlashProgramLongword(PFLASH_SSD_CONFIG PSSDConfig, \
									  UINT32 destination, \
									  UINT32 size, \
									  UINT32 source, \
									  pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#endif                                                                              
{
    UINT8 pCommandArray[8]; /* command sequence array */
    UINT32 returnCode;      /* return code variable */
    UINT32 endAddress;      /* storing end address */

    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;

    /* calculating Flash end address */
    endAddress = destination + BYTE2WORD(size);
        
    /* check if the destination is Longword aligned or not */
    if (destination & (BYTE2WORD(FTFx_LONGWORD_SIZE)-1))
    {
        /* return an error code FTFx_ERR_ADDR */
        returnCode = FTFx_ERR_ADDR;
        goto EXIT;
    }
        
    /* check if the size is Longword alignment or not */
    if(size & (FTFx_LONGWORD_SIZE-1))
    {
        /* return an error code FTFx_ERR_SIZE */
        returnCode = FTFx_ERR_SIZE;
            goto EXIT;
    }
    
    /* check for valid range of the target addresses */
    if((destination < PSSDConfig->PFlashBlockBase) || \
        (endAddress > (PSSDConfig->PFlashBlockBase + BYTE2WORD(PSSDConfig->PFlashBlockSize))))
    {
#if(DEBLOCK_SIZE)    	
        if((destination < PSSDConfig->DFlashBlockBase) || \
            (endAddress > (PSSDConfig->DFlashBlockBase + BYTE2WORD(PSSDConfig->DFlashBlockSize))))
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
    }
    
    /* check for error return code */
    if(FTFx_OK == returnCode)
    {
        while(size > 0)
        {
            /* preparing passing parameter to program the flash block */
            pCommandArray[0] = FTFx_PROGRAM_LONGWORD;
            pCommandArray[1] = (UINT8)(destination >> 16);
            pCommandArray[2] = (UINT8)((destination >> 8) & 0xFF);
            pCommandArray[3] = (UINT8)(destination & 0xFF);
        
#if (ENDIANNESS == BIG_ENDIAN)  /* Big Endian */ 
            pCommandArray[4] = READ8(source);
            pCommandArray[5] = READ8(source + 1);
            pCommandArray[6] = READ8(source + 2);
            pCommandArray[7] = READ8(source + 3);
#else /* Little Endian */
            pCommandArray[4] = READ8(source + 3);
            pCommandArray[5] = READ8(source + 2);
            pCommandArray[6] = READ8(source + 1);
            pCommandArray[7] = READ8(source);
#endif
            
            /* calling flash command sequence function to execute the command */
            returnCode = FlashCommandSequence(PSSDConfig, 7, pCommandArray);
          
            /* checking for the success of command execution */
            if(FTFx_OK != returnCode)
            {
                break;
            }
            else
            {
                /* update destination address for next iteration */
                destination += FTFx_LONGWORD_SIZE;
                /* update size for next iteration */
                size -= FTFx_LONGWORD_SIZE;
                        /* increment the source adress by 1 */
                source += FTFx_LONGWORD_SIZE;
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


