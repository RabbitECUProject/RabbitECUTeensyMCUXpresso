/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashEraseSuspend.c                                  *
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
0.3.1		06.06.2012		FPT Team	  Add waiting for CCIF bit after setting ERSSUSP                                          
0.3.3		08.03.2012		FPT Team	  Optimize code size for L2K/L1PT
										  Nevis2, and Anguilla Silver   
*************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"

/************************************************************************
*
*  Function Name    : FlashEraseSuspend.c
*  Description      : This function is used to suspend a current operation
*                     of flash erase sector command.
*  Arguments        : PFLASH_SSD_CONFIG
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
UINT32 __attribute__((optimize("O4"))) FlashEraseSuspend(PFLASH_SSD_CONFIG PSSDConfig)
#else
UINT32 FlashEraseSuspend(PFLASH_SSD_CONFIG PSSDConfig)
#endif
{
    UINT32 returnCode;      /* return code variable */
    
    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
    
    /* check CCIF bit of the flash status register */
    if(FALSE == (REG_BIT_TEST(PSSDConfig->ftfxRegBase + FTFx_SSD_FSTAT_OFFSET, FTFx_SSD_FSTAT_CCIF)))
    {
        /* If the command write sequence in progressing, */
        /* Set ERSSUSP bit in FCNFG register */
        REG_BIT_SET(PSSDConfig->ftfxRegBase + FTFx_SSD_FCNFG_OFFSET, FTFx_SSD_FCNFG_ERSSUSP);
        while(FALSE == (REG_BIT_TEST(PSSDConfig->ftfxRegBase + FTFx_SSD_FSTAT_OFFSET, FTFx_SSD_FSTAT_CCIF)))
        {
            /* wait till CCIF bit is set */
        }
    }
    else
    {
        /* Do nothing */
    }
    
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