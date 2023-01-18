/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "CPUAbstract.h"
#include "regset.h"
#include "sys.h"
#include "declarations.h"
#include "os.h"
#include "PIMHA.h"
#include "SIM.h"
#include "PIMAPI.h"
#include "regset.h"
#include "MATH.h"
#include "IOAPI.h"

const REGSET_tstReg32Val PIMHA_rastPIMReg32Val[] = PIMHA_nReg32Set;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
tstPortModule* PIMHA_rapstPIMPortTypeMap[] = PIMHA_nPortPointerMap;
tstGPIOModule* PIMHA_rapstPIMGPIOTypeMap[] = PIMHA_nGPIOPointerMap;
uint32 PIM_u32PortClockRequested;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E

#endif //SAM3X8E


#if defined(BUILD_SAM3X8E)
#include "sam3x8e.h"
typedef struct Pio tstGPIOModule;
#endif

const uint32 PIMHA_rau32PIMClockMasks[] = PIMHA_nPortClockMasks;


void PIMHA_vStart(uint32* const u32Stat)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	/* Request Kernel required port clocks */
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTA_MASK);
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTB_MASK);
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTC_MASK);
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTD_MASK);	/* TODO not kernel required! */
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTE_MASK);	
#endif

#ifdef BUILD_SAM3X8E
	PIMHA_xRequestPortClock(PIOA_IRQn);
	PIMHA_xRequestPortClock(PIOB_IRQn);
	PIMHA_xRequestPortClock(PIOC_IRQn);
	PIMHA_xRequestPortClock(PIOD_IRQn);
#endif //BUILD_SAM3X8E
	
#ifdef BUILD_MK60
	REGSET_vInitReg32(&PIMHA_rastPIMReg32Val[0]);
#endif //BUILD_MK60
}


void PIMHA_vInitPortBit(PIMAPI_tenPort enPort, IOAPI_tenEHIOType enIOType, uint32 u32PortBit)
{
	//if (0 == (PIMHA_rau32PIMClockMasks[enPort] & PIM_u32PortClockRequested))
	/* Request the port clock if not yet requested */
	//{
	//	PIMHA_xRequestPortClock(PIM_rau32PIMClockMasks[enPort]);
	//}
	
	PIMHA_vSetPortMux(enPort, enIOType, u32PortBit, 1);
}

void PIMHA_vAssertPortBit(PIMAPI_tenPort enPort, uint32 u32PortBit, IOAPI_tenTriState enTriState)
{	
	switch (enTriState)
	{
		case IOAPI_enLow:
		{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
			tstGPIOModule* pstGPIO = PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR |= u32PortBit;
			pstGPIO->PCOR = u32PortBit;
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
			tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PIO_CODR = u32PortBit;
#endif //BUILD_SAM3X8E
			break;						
		}
		case IOAPI_enHigh:
		{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
			tstGPIOModule* pstGPIO = PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR |= u32PortBit;
			pstGPIO->PSOR = u32PortBit;
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
			tstGPIOModule* pstGPIO = PIMHA_rapstPIMPortTypeMap[enPort];
			pstGPIO->PIO_SODR = u32PortBit;
#endif //BUILD_SAM3X8E
			break;			
		}		
		case IOAPI_enToggle:
		{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
			volatile tstGPIOModule* pstGPIO = PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR |= u32PortBit;
			pstGPIO->PTOR = u32PortBit;
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
			tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
			if (0 < (u32PortBit & pstGPIO->PIO_ODSR))
			{
			    pstGPIO->PIO_CODR = u32PortBit;
			}
			else
			{
			    pstGPIO->PIO_SODR = u32PortBit;
			}
#endif //BUILD_SAM3X8E
			break;			
		}	
		case IOAPI_enHiZ:
		{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
			tstGPIOModule* pstGPIO = PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR &= ~u32PortBit;
			pstGPIO->PCOR = u32PortBit;
#endif //BUILD_MK6X
			break;					
		}
		default:
		{

		}
	}
}

bool PIMHA_boGetPortBitState(PIMAPI_tenPort enPort, uint32 u32PortBit)
{
	bool boBitHigh = false;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	volatile tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
	boBitHigh = 0 < ((pstGPIO->PDIR) & u32PortBit);
#endif //BUILD_MK6X


#ifdef BUILD_SAM3X8E
	tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
	boBitHigh = (0 < (pstGPIO->PIO_PDSR & u32PortBit)) ? TRUE : FALSE;
#endif //BUILD_SAM3X8E



	return boBitHigh;	
}

void PIMHA_vSetPortMux(PIMAPI_tenPort enPort, IOAPI_tenEHIOType enIOType, uint32 u32PortBit, uint32 u32MuxSel)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	uint16 u16PortBit = MATH_u32MaskToIDX(u32PortBit);
	volatile tstPortModule* pstPort = (tstPortModule*)PIMHA_rapstPIMPortTypeMap[enPort];
	uint16 u16RegOffset = offsetof(tstPortModule, PCR[u16PortBit]);
	*(puint32)((uint32)pstPort + (uint32)u16RegOffset) &= ~PORT_PCR_MUX_MASK;
	*(puint32)((uint32)pstPort + (uint32)u16RegOffset) |= PORT_PCR_MUX(u32MuxSel);
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	tstGPIOModule* pstPort = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
    pio_type_t pio_type;

	switch (enIOType)
	{
	    case IOAPI_enDIOOutput: pio_type = PIO_OUTPUT_0; break;
		case IOAPI_enDIOInput: pio_type = PIO_INPUT; break;
		case IOAPI_enTEPM: pio_type = u32MuxSel == 0 ? PIO_PERIPH_A : PIO_PERIPH_B; break;
		case IOAPI_enSPIBus: pio_type = u32MuxSel == 0 ? PIO_PERIPH_A : PIO_PERIPH_B; break;
		case IOAPI_enCANBus: pio_type = u32MuxSel == 0 ? PIO_PERIPH_A : PIO_PERIPH_B; break;
		default: pio_type = PIO_INPUT; break;
	}

	pio_configure(pstPort, pio_type, u32PortBit, PIO_DEFAULT);
#endif //BUILD_SAM3X8E
}




			
			
