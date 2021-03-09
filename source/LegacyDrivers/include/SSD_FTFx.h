/************************************************************************
* (c) Copyright Freescale Semiconductor, Inc 2012, All Rights Reserved  *
*************************************************************************

*************************************************************************
*                                                                       *
*        Standard Software Flash Driver For FTFx                        *
*                                                                       *
* FILE NAME     :  SSD_FTFx.h                                           *
* DATE          :  June 20, 2012                                        *
*                                                                       *
* AUTHOR        :  FPT Team                                             *
* E-mail        :  b39392@freescale.com                                 *
*                                                                       *
*************************************************************************/

/************************** CHANGES *************************************
0.0.1       06.09.2010      FPT Team      Initial Version
0.1.0       06.11.2010      FPT Team      Finalize to 0.1.0
0.1.1       08.16.2010      FPT Team      Update some macros for
                                          FTFx_KX_256K_256K_4K_2K_2K derivative
0.1.2       08.26.2010      FPT Team      Removed EEEBlockBase element in
                                          _ssd_config structure, 
0.1.3       09.16.2010      FPT Team      Updated some macros for
                                          FTFx_KX_256K_256K_4K_2K_2K derivative
0.2.0       09.27.2010      FPT Team      Removed some macros that is not 
                                          used.
0.2.1       01.28.2011      FPT Team      Updated to support 
                                          FTFx_KX_512K_0K_0K_2K_0K,
                                          FTFx_JX_128K_32K_2K_1K_1K,
                                          and FTFx_FX_256K_32K_2K_1K_1K
                                          derivatives.
0.2.2       04.18.2011      FPT Team      Add Swap state definitions of
                                          FTFx_PFLASH_SWAP.
0.2.2       09.15.2011      FPT Team      Add FlashProgramPhrase
                                          Add macros for K70
                                          Remove unused macros
0.2.3       11.15.2011      FPT Team      Updated some macros for
                                          FTFx_KX_1024K_0K_16K_4K_0K derivative.    
0.2.4       12.23.2011      FPT Team      Update to support more Kinetis derivatives.  
0.2.5       04.26.2012      FPT Team      Update to support swap in FTFx_KX_512K_0K_0K_2K_0K derivative     
                                          Add definition of NULL_SWAP_CALLBACK to 
                                          fix incompatible function type of null pointer bug in IAR compiler
0.3.1        05.16.2012     FPT Team      Update to support 
                                          FTFx_NX_256K_32K_2K_2K_1K
                                          FTFx_NX_128K_32K_2K_2K_1K
                                          FTFx_NX_96K_32K_2K_2K_1K
                                          FTFx_NX_64K_32K_2K_2K_1K
                                          derivatives
                                          Change prototype of FlashReadOnce and FlashProgramOnce functions
0.3.2        06.20.2012     FPT Team      Update to support more L2K and L1PT derivatives.
                                          Change format of SSD_FTFx.h
0.3.3		 08.10.2012     FPT Team      Update to support Anguilla Silver derivatives                                          
*************************************************************************/
#ifndef _SSD_FTFx_H_
#define _SSD_FTFx_H_

#include "SSD_FTFx_Common.h"

/* Derivative selection */
#define FLASH_DERIVATIVE                  FTFx_KX_256K_256K_4K_2K_2K 

/* Compiler selection */
#define COMPILER                          CW

/* Select file .h for each derivatives */
#if (FTFx_KX_256K_256K_4K_2K_2K == FLASH_DERIVATIVE)
    #include "FTFx_KX_256K_256K_4K_2K_2K.h"
#elif (FTFx_KX_512K_0K_0K_2K_0K == FLASH_DERIVATIVE)
    #include "FTFx_KX_512K_0K_0K_2K_0K.h"
#elif (FTFx_JX_128K_32K_2K_1K_1K == FLASH_DERIVATIVE)
    #include "FTFx_JX_128K_32K_2K_1K_1K.h"
#elif (FTFx_FX_256K_32K_2K_1K_1K == FLASH_DERIVATIVE)
    #include "FTFx_FX_256K_32K_2K_1K_1K.h"
#elif (FTFx_KX_512K_512K_16K_4K_4K == FLASH_DERIVATIVE)
    #include "FTFx_KX_512K_512K_16K_4K_4K.h"
#elif (FTFx_KX_1024K_0K_16K_4K_0K == FLASH_DERIVATIVE)
    #include "FTFx_KX_1024K_0K_16K_4K_0K.h"
#elif ((FTFx_KX_128K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_KX_64K_0K_0K_1K_0K == FLASH_DERIVATIVE)\
	||(FTFx_KX_32K_0K_0K_1K_0K == FLASH_DERIVATIVE))
    #include "FTFx_KX_(128_64_32)K_0K_0K_1K_0K.h"
#elif ((FTFx_KX_128K_32K_2K_1K_1K == FLASH_DERIVATIVE)||(FTFx_KX_64K_32K_2K_1K_1K == FLASH_DERIVATIVE)\
	||(FTFx_KX_32K_32K_2K_1K_1K == FLASH_DERIVATIVE))
    #include "FTFx_KX_(128_64_32)K_32K_2K_1K_1K.h"   
#elif ((FTFx_KX_256K_32K_2K_2K_1K == FLASH_DERIVATIVE)||(FTFx_KX_128K_32K_2K_2K_1K == FLASH_DERIVATIVE)\
	||(FTFx_KX_64K_32K_2K_2K_1K == FLASH_DERIVATIVE))
    #include "FTFx_KX_(256_128_64)K_32K_2K_2K_1K.h"    
#elif ((FTFx_NX_256K_32K_2K_2K_1K == FLASH_DERIVATIVE)||(FTFx_NX_128K_32K_2K_2K_1K == FLASH_DERIVATIVE)\
	||(FTFx_NX_96K_32K_2K_2K_1K == FLASH_DERIVATIVE)||(FTFx_NX_64K_32K_2K_2K_1K == FLASH_DERIVATIVE))
    #include "FTFx_NX_(256_128_96_64)K_32K_2K_2K_1K.h"      
#elif ((FTFx_LX_128K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_LX_64K_0K_0K_1K_0K == FLASH_DERIVATIVE)\
	||(FTFx_LX_32K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_LX_16K_0K_0K_1K_0K == FLASH_DERIVATIVE)\
	||(FTFx_LX_8K_0K_0K_1K_0K == FLASH_DERIVATIVE))
    #include "FTFx_LX_(128_64_32_16_8)K_0K_0K_1K_0K.h"
#elif ((FTFx_AX_64K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_AX_48K_0K_0K_1K_0K == FLASH_DERIVATIVE)\
	||(FTFx_AX_32K_0K_0K_1K_0K == FLASH_DERIVATIVE)||(FTFx_AX_16K_0K_0K_1K_0K == FLASH_DERIVATIVE))
	#include "FTFx_AX_(64_48_32_16)K_0K_0K_1K_0K.h"
#endif
                                    
#endif /* _SSD_FTFx_H_ */

