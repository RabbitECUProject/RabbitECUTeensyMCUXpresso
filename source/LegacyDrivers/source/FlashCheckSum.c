/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashCheckSum.c                                      *
* DATE          :  January 28,2011                                      *
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
0.2.2       02.21.2011      FPT Team      Fixed boundary condition
0.3.1		05.16.2012 		FPT Team	  Update to support Nevis2 devices
0.3.3		08.03.2012		FPT Team	  Optimize code sizefor L2K/L1PT
										  Nevis2, adn Anguilla Silver
************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"
#if (CPU_CORE == DSC_56800EX)
	#define WORD2BYTE(x) ((x)<<1)
#else
	#define WORD2BYTE(x) (x)
#endif
/***********************************************************************
*
*  Function Name    : FlashCheckSum.c
*  Description      : This function is used to calculate checksum value 
*                     for the specified flash range.
*  Arguments        : PFLASH_SSD_CONFIG,UINT32 ,UINT32 ,UINT32*
*  Return Value     : UINT32
*
************************************************************************/
/* Enable size optimization for Anguilla Silver and Nevis2 derivatives */
#if((CPU_CORE == DSC_56800EX)&&(COMPILER == CW))
#pragma optimize_for_size on
#pragma optimization_level 4
#endif
/* Enable size optimization for L2K and L1PT derivatives */
#if((CPU_CORE == ARM_CM0PLUS)&&(COMPILER == CW))
UINT32 __attribute__((optimize("O4"))) FlashCheckSum(PFLASH_SSD_CONFIG PSSDConfig, \
                                UINT32 destination, \
                                UINT32 size, \
                                UINT32* PSum)
#else
UINT32 FlashCheckSum(PFLASH_SSD_CONFIG PSSDConfig, \
								UINT32 destination, \
								UINT32 size, \
								UINT32* PSum)
#endif                                
{
    UINT32 counter;          /* Counter for callback operation */
    UINT32 data;             /* Data read from Flash address */
    UINT32 returnCode;       /* Return code variable */
    UINT32 endAddress;       /* P Flash end address */
    
    /* initialize counter variable */
    counter = 0;
    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
        
    /* calculating Flash end address */
    endAddress = WORD2BYTE(destination) + size;
    
    /* check for valid range of the target addresses */
    if((destination < PSSDConfig->PFlashBlockBase) || \
        (endAddress > (WORD2BYTE(PSSDConfig->PFlashBlockBase) + PSSDConfig->PFlashBlockSize)))
    {
#if(DEBLOCK_SIZE)    	
        if((destination < PSSDConfig->DFlashBlockBase) || \
            (endAddress > (WORD2BYTE(PSSDConfig->DFlashBlockBase) + PSSDConfig->DFlashBlockSize)))
        {
#endif /* End of if(DEBLOCK_SIZE) */         	
            /* return an error code FTFx_ERR_RANGE */
            returnCode = FTFx_ERR_RANGE;
            goto EXIT;
#if(DEBLOCK_SIZE)              
        }
        else
        {
        		/* No Error: Do nothing */
        }
#endif /* End of if(DEBLOCK_SIZE) */        
    }
    else
    {
        /* No Error: Do nothing */
    }
#if DSC_56800EX == CPU_CORE
	destination *=2;
	endAddress = destination + size;
#endif	
    if(FTFx_OK == returnCode)
    {       
        *PSum = 0;
        /* doing sum operation */
        while(destination < endAddress)
        {          
            data = READ8(destination);
            *PSum += (UINT32)data;
            destination += 1;
            size -= 1;

            /* Check if need to serve callback function */
            if((++counter) >= FLASH_CALLBACK_CS)
            {
                /* serve callback function if counter reaches limitation */
                if(NULL_CALLBACK != PSSDConfig->CallBack)
                {
                    (PSSDConfig->CallBack)();
                }
                else
                {
                    /* do nothing */
                }

                /* Reset counter */
                counter = 0;
            }
            else
            {
                /* do nothing */
            }
        }            
    }
    else
    {
        /* error occurred */
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
