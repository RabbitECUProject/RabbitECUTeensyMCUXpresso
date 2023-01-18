/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PIMHA_H
#define PIMHA_H

#include <stddef.h>
#include "CPUAbstract.h"
#include "declarations.h"
#include "PIMAPI.h"
#include "build.h"

#ifdef BUILD_SAM3X8E
#include "pio.h"
#endif

#include "IOAPI.h"
#include "types.h"

#if defined(BUILD_MK60)
typedef PORT_Type tstPortModule;
typedef GPIO_Type tstGPIOModule;
#endif //BUILD_MK60

#if defined(BUILD_MK64)
typedef PORT_Type tstPortModule;
typedef GPIO_Type tstGPIOModule;
#include "mk64f12.h"
#endif //BUILD_MK64

#if defined(BUILD_MKS20)
typedef PORT_Type tstPortModule;
typedef GPIO_Type tstGPIOModule;
#include "mks20f12.h"
#endif //BUILD_MKS20

#ifdef BUILD_SAM3X8E
typedef Pio tstGPIOModule;
#endif

#ifdef BUILD_MK60
#define PIMHA_nReg32Set                                                                                                       \
{                                                                                                                           \
    { (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(3), REGSET_enOr },              \
    { (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(3), REGSET_enOr },              \
    { (volatile uint32*)(PORTC_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(3), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTC_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(3), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[24])), (uint32)PORT_PCR_MUX(2), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[25])), (uint32)PORT_PCR_MUX(2), REGSET_enOverwrite },       \
    { NULL, 0, REGSET_enOverwrite}                                                                                          \
};
#endif //BUILD_MK60

#ifdef BUILD_MK64
#define PIMHA_nReg32Set                                                                                                       \
{                                                                                                                           \
    { (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(3), REGSET_enOr },              \
    { (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(3), REGSET_enOr },              \
    { (volatile uint32*)(PORTC_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(3), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTC_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(3), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[24])), (uint32)PORT_PCR_MUX(2), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[25])), (uint32)PORT_PCR_MUX(2), REGSET_enOverwrite },       \
    { NULL, 0, REGSET_enOverwrite}                                                                                          \
};
#endif //BUILD_MK64

#ifdef BUILD_MKS20
#define PIMHA_nReg32Set                                                                                                       \
{                                                                                                                           \
    { (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(3), REGSET_enOr },              \
    { (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(3), REGSET_enOr },              \
    { (volatile uint32*)(PORTC_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(3), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTC_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(3), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[24])), (uint32)PORT_PCR_MUX(2), REGSET_enOverwrite },       \
    { (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[25])), (uint32)PORT_PCR_MUX(2), REGSET_enOverwrite },       \
    { NULL, 0, REGSET_enOverwrite}                                                                                          \
};
#endif //BUILD_MKS20


#ifdef BUILD_SAM3X8E
#define PIMHA_nReg32Set                                                                                                       \
{                                                                                                                           \
	{ NULL, 0, REGSET_enOverwrite}                                                                                          \
};
#endif //BUILD_SAM3X8E

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
#define PIMHA_nPortPointerMap \
{                           \
    PORTA,                \
    PORTB,                \
    PORTC,                \
    PORTD,                \
    PORTE                 \
}					
#endif //BUILD_MK60	

#ifdef BUILD_SAM3X8E
#define PIMHA_nPortPointerMap \
{                           \
	{PIOA},                 \
	{PIOB},                 \
	{PIOC},                 \
	{PIOD}                  \
}
#endif //BUILD_SAM3X8E						

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
#define PIMHA_nGPIOPointerMap \
{                           \
    PTA,                  \
    PTB,                  \
    PTC,                  \
    PTD,                  \
    PTE                   \
}					
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
#define PIMHA_nGPIOPointerMap \
{                           \
	{PIOA},                  \
	{PIOB},                  \
	{PIOC},                  \
	{PIOD},                  \
	{PIOD}                   \
}
#endif					

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
#define PIMHA_nPortClockMasks \
{                           \
    SIM_SCGC5_PORTA_MASK, \
    SIM_SCGC5_PORTB_MASK, \
    SIM_SCGC5_PORTC_MASK, \
    SIM_SCGC5_PORTD_MASK, \
    SIM_SCGC5_PORTE_MASK  \
}
#endif  //BUILD_MK6X

/* The clock bit is borrowed from the IRQ# */
#ifdef BUILD_SAM3X8E
#define PIMHA_nPortClockMasks \
{                           \
	PIOA_IRQn, \
	PIOB_IRQn, \
	PIOC_IRQn, \
	PIOD_IRQn  \
}
#endif  //BUILD_MK60

#ifdef BUILD_MK60
#define PIMHA_xRequestPortClock(x)    \
/* turn on PORTA clock */           \
SIM_vSetReg32(SIM_SCGC5, x);        \
PIM_u32PortClockRequested |= x	
#endif //BUILD_MK60	

#ifdef BUILD_MK64
#define PIMHA_xRequestPortClock(x)    \
/* turn on PORTA clock */           \
SIM_vSetReg32(SIM_SCGC5, x);        \
PIM_u32PortClockRequested |= x
#endif //BUILD_MK64

#ifdef BUILD_MKS20
#define PIMHA_xRequestPortClock(x)    \
/* turn on PORTA clock */           \
SIM_vSetReg32(SIM_SCGC5, x);        \
PIM_u32PortClockRequested |= x
#endif //BUILD_MKS20


#ifdef BUILD_SAM3X8E
#define PIMHA_xRequestPortClock(x) SIMHA_boEnablePeripheralClock(x)
#endif //BUILD_SAM3X8E


void PIMHA_vStart(uint32* const);
void PIMHA_vRun(uint32* const);
void PIMHA_vTerminate(uint32* const);
void PIMHA_vInitPortBit(PIMAPI_tenPort, IOAPI_tenEHIOType, uint32);
void PIMHA_vAssertPortBit(PIMAPI_tenPort, uint32, IOAPI_tenTriState);
void PIMHA_vSetPortMux(PIMAPI_tenPort, IOAPI_tenEHIOType, uint32, uint32);
bool PIMHA_boGetPortBitState(PIMAPI_tenPort, uint32);


#endif //PIMHA_H

