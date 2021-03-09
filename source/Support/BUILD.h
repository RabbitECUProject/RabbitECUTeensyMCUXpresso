/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      Build Header File                                      */
/* DESCRIPTION:        This code module provides build options for compiling  */
/*                     the project to PBL, SBL, Flash Kernel, RAM Kernel+APP  */
/*                     Debugging or Flash User APP and WIFI/ETHERNET switch   */
/* FILE NAME:          BUILD.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef BUILD_H
#define BUILD_H

#ifndef BUILD_MK64
#define BUILD_MK64
#endif

//#define BUILD_SPARKDOG_TEENSY_ADAPT
#define BUILD_SPARKDOG_PF

#define BUILD_USER
//#define BUILD_BSP_IAC_STEPPER
#define BUILD_FME
#define BUILD_BSP_AFM_FREQ
#define BUILD_KERNEL_APP
#define BUILD_PACKING __attribute__((packed))
#define BUILD_PACKING_ETHERNET __attribute__((packed))
#define USB_DATA_ALIGNMENT __attribute__ ((aligned (8)))
#define USB_DATA_BUFF_SIZE 256

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#ifdef BUILD_PBL
/* Build the Primary BootLoader to be resident in FLASH @ 0x00000000 */
	#define BUILD_DL_ADDR_MIN 0x20000000
	#define BUILD_DL_ADDR_MAX 0x2000ffff
#endif	

#ifdef BUILD_SBL
/* Build the Secondary BootLoader to be resident in RAM @ 0x20000000 */
	#define BUILD_DL_ADDR_MIN 0x00010000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
#endif	

#ifdef BUILD_UBL
/* Build the User BootLoader to be resident in RAM @ 0x20000000 */
	#define BUILD_DL_ADDR_MIN 0x00020000
	#define BUILD_DL_ADDR_MAX 0x0002ffff
#endif	

#ifdef BUILD_KERNEL
/* Build the KERNEL for release to be resident in FLASH @ 0x00010000 */
	#define BUILD_DL_ADDR_MIN 0x00070000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP
#endif	

#ifdef BUILD_KERNEL_APP
/* Build the KERNEL and APP for testing to be resident in FLASH @ 0x00070000 */
	#define BUILD_DL_ADDR_MIN 0x00070000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP
	#define BUILD_USER
#endif	

#ifdef BUILD_USER_APP
/* Build the User APP for release be resident in FLASH @ 0x00070000 */
	#define BUILD_DL_ADDR_MIN 0x00070000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP
	#define BUILD_USER
#endif	

#define PBL_WORD_ADDR		            0x2000fffc
#define RAM_START_ADDR					0x20000000
#define RAM_END_ADDR                    0x2008ffff
#define LSU_DEVICE_COUNT				1u

#endif // BUILD_H

