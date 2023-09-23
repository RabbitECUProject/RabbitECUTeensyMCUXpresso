/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Conversion                                             */
/* DESCRIPTION:        This code module provides data conversion functions    */
/*                     functions for the user application                     */
/* FILE NAME:          CONV.c                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include <BUILD.h>

#ifdef BUILD_USER

#include "conv.h"


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
GPM6_ttVolts CONV_tADCToVolts(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	GPM6_ttVolts tVolts;/*CR1_17*/
	
#ifdef BUILD_SPARKDOG_PF
	switch (enEHIOResource)
	{	
	    case EH_IO_GPSE1:
	    case EH_IO_GPSE2:
	    case EH_IO_GPSE3:
	    case EH_IO_GPSE4:
	    case EH_IO_GPSE5:
	    case EH_IO_GPSE7:
	    case EH_IO_GPSE8:
		{
		    tVolts = USERCAL_stRAMCAL.userCalADRefH * u32ADCResult;
		    tVolts = (101 * tVolts) / (4095 * 50);
			break;
		}
	    case EH_IO_GPSE6:
		{
		    tVolts = USERCAL_stRAMCAL.userCalADRefH * u32ADCResult;
		    tVolts = (144 * tVolts) / (4095 * 22);
			break;
		}
		default:
		{
			tVolts = 0;/*CR1_17*/
		}
	}
#endif //BUILD_SPARKDOG_PF

#ifdef BUILD_SPARKDOG_TEENSY_ADAPT
	switch (enEHIOResource)
	{
	    case EH_IO_GPSE1:
	    case EH_IO_GPSE3:
	    case EH_IO_GPSE4:
	    case EH_IO_GPSE5:
	    case EH_IO_GPSE6:
	    case EH_IO_GPSE7:
	    case EH_IO_GPSE8:
	    case EH_IO_TMR15:
	    case EH_IO_TMR16:
		{
		    tVolts = USERCAL_stRAMCAL.userCalADRefH * u32ADCResult;
		    tVolts = (101 * tVolts) / (4095 * 50);
			break;
		}
	    case EH_IO_GPSE2:
		{
		    tVolts = USERCAL_stRAMCAL.userCalADRefH * u32ADCResult;
		    tVolts = (144 * tVolts) / (4095 * 22);
			break;
		}
	    case EH_IO_IIC1_SCL:
	    {
	    	tVolts = (34u * USERCAL_stRAMCAL.userCalVRef445) / 33u
                - (USERCAL_stRAMCAL.userCalADRefH * u32ADCResult) / (32000);
	    	break;
	    }
	    case EH_IO_UART1_CTS:
	    {
	    	tVolts = (220u * u32ADCResult) / 69888u;
	    	break;
	    }
		default:
		{
			tVolts = 0;/*CR1_17*/
		}
	}
#endif //BUILD_SPARKDOG_TEENSY_ADAPT
	
#if defined(BUILD_SPARKDOG_MKS20)
	switch (enEHIOResource)
	{
		case EH_IO_GPSE1:
		case EH_IO_GPSE2:
		case EH_IO_GPSE3:
		case EH_IO_GPSE4:
		case EH_IO_GPSE5:
		case EH_IO_GPSE6:
		case EH_IO_GPSE7:

		case EH_IO_GPSE9:
		case EH_IO_GPSE10:
		case EH_IO_GPSE11:
		case EH_I_ADD1:
		case EH_I_ADD2:
		case EH_I_ADD3:
		case EH_I_ADD4:
		case EH_I_ADD5:
		{
		    tVolts = USERCAL_stRAMCAL.userCalADRefH * u32ADCResult;
		    tVolts = (101 * tVolts) / (4095 * 50);
			break;
		}
		case EH_IO_GPSE8:
		{
		    tVolts = USERCAL_stRAMCAL.userCalADRefH * u32ADCResult;
		    tVolts = (144 * tVolts) / (4095 * 22);
		    break;
		}
		default:
		{
			tVolts = 0;
			break;
		}
	}
#endif //BUILD_SPARKDOG_MKS20

	return tVolts;	
}

GPM6_ttOhms CONV_tADCToOhms(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	GPM6_ttOhms Ohms;
	
	switch (enEHIOResource)
	{	
		case EH_I_ADD1:
		{
			Ohms = 9 * (u32ADCResult + 8600);
			break;
		}
		default:
		{
			Ohms = 0;
			break;
		}
	}
	
	return Ohms;
}

GPM6_ttOhms CONV_tVoltsAmpsToOhms(GPM6_ttVolts tVolts, GPM6_ttAmps tAmps)/*CR1_17*/
{
	GPM6_ttOhms tOhms = GPM6_ttOhmsMAXVAL;
	
	if (0 != tAmps)/*CR1_20*/
	{
		tOhms = (tVolts * 1000u) / tAmps;/*CR1_17*/
	}

	return tOhms;
}

GPM6_ttAmps CONV_tOhmsVoltsToAmps(GPM6_ttOhms tOhms, GPM6_ttVolts tVolts)/*CR1_17*/
{
	GPM6_ttAmps tAmps = GPM6_ttAmpsMAXVAL;	
	
	if (0 != tOhms)/*CR1_20*/
	{
		tAmps = (tVolts * 1000u) / tOhms;/*CR1_17*/
	}
	
	return tAmps;
}			
			
#endif //BUILD_USER
