/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2011, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  FlashProgramOnce.c                                   *
* DATE          :  Dec 27,2011                                          *
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
0.2.2       02.21.2011      FPT Team      Removed endianness condition
                                          preprocessor
0.2.3       09.15.2011      FPT Team      Update to support K70         
0.2.4       11.15.2011      FPT Team      Update to support FTFx_KX_1024K_0K_16K_4K_0K
                                          derivative.       
0.2.5       12.27.2011      FPT Team      Update to support more Kinetis derivatives.
0.3.1		06.01.2012		FPT Team	  Update to program a data record (4 or 8 bytes)
										  specified by recordIndex
										  instead of entire 64 byte OTP region
0.3.3		08.03.2012		FPT Team	  Optimize code size for L2K/L1PT
										  Nevis2, and Anguilla Silver   										  
*************************************************************************/
/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"
/************************************************************************
*
*  Function Name    : FlashProgramOnce.c
*  Description      : Program a data record into a dedicated 64 bytes 
*  					  (divided into 16 data records) region in
*                     the P-Flash IFR which stores critical information
*                     for the user
*  Arguments        : PFLASH_SSD_CONFIG, UINT8 , UINT8*, pFLASHCOMMANDSEQUENCE
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
UINT32 __attribute__((optimize("O4"))) FlashProgramOnce(PFLASH_SSD_CONFIG PSSDConfig, \
                                   UINT8 recordIndex,\
                                   UINT8* pDataArray, \
                                   pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#else
UINT32 FlashProgramOnce(PFLASH_SSD_CONFIG PSSDConfig, \
							 UINT8 recordIndex,\
							 UINT8* pDataArray, \
							 pFLASHCOMMANDSEQUENCE FlashCommandSequence)
#endif                                                                     
{
#if ((FTFx_KX_512K_512K_16K_4K_4K == FLASH_DERIVATIVE) || (FTFx_KX_1024K_0K_16K_4K_0K == FLASH_DERIVATIVE))    
    UINT8  pCommandArray[0xC]; /* command sequence array */
#else
    UINT8  pCommandArray[0x8]; /* command sequence array */
#endif 
    UINT32 returnCode;       /* return code variable */
    UINT8  commandArraySize = 0x07;      

    /* set the default return code as FTFx_OK */
    returnCode = FTFx_OK;
    /* set the counter value for 0 */

    /* preparing passing parameter for program once command */
    /* 1st element for the FCCOB register */
    pCommandArray[0] = FTFx_PROGRAM_ONCE;

    /* preparing the rest of the parameters to be passed for the FCCOB registers */
    pCommandArray[1] = recordIndex;
    pCommandArray[2] = 0x00;
    pCommandArray[3] = 0x00;
    pCommandArray[4] = pDataArray[0];
    pCommandArray[5] = pDataArray[1];
    pCommandArray[6] = pDataArray[2];
    pCommandArray[7] = pDataArray[3];
#if ((FTFx_KX_512K_512K_16K_4K_4K == FLASH_DERIVATIVE) || (FTFx_KX_1024K_0K_16K_4K_0K == FLASH_DERIVATIVE))
    pCommandArray[8] = pDataArray[4];
    pCommandArray[9] = pDataArray[5];
    pCommandArray[10] = pDataArray[6];
    pCommandArray[11] = pDataArray[7];
    commandArraySize = 0xB;
#endif

    /* calling flash command sequence API to execute the command */
    returnCode = FlashCommandSequence(PSSDConfig, commandArraySize, pCommandArray);


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


