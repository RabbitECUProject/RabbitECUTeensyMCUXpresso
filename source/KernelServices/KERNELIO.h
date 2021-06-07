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
#ifndef KERNELIO_H
#define KERNELIO_H

#include <SYS.h>
#include "SYSAPI.h"
#include "IOAPI.h"
#include "PIMAPI.h"
#include "ADCAPI.h"
#include "DACAPI.h"
#include "TEPMAPI.h"

#define KERNEL_RXD1              71
#define KERNEL_RXD0              72
#define KERNEL_RXDV              73
#define KERNEL_TXEN              74
#define KERNEL_TXD0              75
#define KERNEL_TXD1              76
#define KERNEL_MDIO              77
#define KERNEL_MDC               78
#define KERNEL_CPURX             79
#define KERNEL_CPUTX             80
#define KERNEL_DISCRETE_HIDDEN   11
	
typedef struct
{
	IOAPI_tenEHIOResource enEHIO;
	PIMAPI_tenPort enPort;
	uint32 u32PortBit;
	bool boIsADSE;	
	bool boIsADD;
	bool boIsDAC;
} IO_tstEHPadResource;

#ifdef BUILD_MK60
#define IO_nPadResourceMap	{           \
	{EH_IO_ADSE1,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},			\
	{EH_IO_ADSE2,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_6, TRUE, FALSE, FALSE},			\
	{EH_IO_ADSE3,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_5, TRUE, FALSE, FALSE},			\
	{EH_IO_ADSE4,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},			\
	{EH_IO_ADSE5,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_11, TRUE, FALSE, FALSE},			\
	{EH_IO_ADSE6,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},			\
	{EH_IO_ADSE7,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE8,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE9,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE10, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE11, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE12, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_GPSE1, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE2, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE5, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_27, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE6, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_28, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE7, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE8, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE9, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_8, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE10, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},			\
	{EH_I_ADD1,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD2,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD3,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD4,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD5,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD6,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD7,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_I_ADD8,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},								\
	{EH_IO_CAN2T,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_24, TRUE, FALSE, FALSE},			\
	{EH_IO_CAN2R,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_25, TRUE, FALSE, FALSE},			\
	{EH_I_CMP1,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_I_CMP2,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_I_CMP3,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{IO_AD_Count,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_UART1_TX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_19, TRUE, FALSE, FALSE},	\
	{EH_IO_UART1_RX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_18, TRUE, FALSE, FALSE},	\
	{EH_IO_UART1_CTS, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_17, TRUE, FALSE, FALSE},	\
	{EH_IO_UART1_RTS, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_16, TRUE, FALSE, FALSE},	\
	{EH_IO_UART2_TX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_15, TRUE, FALSE, FALSE},	\
	{EH_IO_UART2_RX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_14, TRUE, FALSE, FALSE},	\
	{EH_IO_UART2_CTS, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_13, TRUE, FALSE, FALSE},	\
	{EH_IO_UART2_RTS,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_12, TRUE, FALSE, FALSE},	\
	{EH_IO_CAN1T,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_CAN1R,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_TMR1, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR2, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_6, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR3, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_5, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR4, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR5, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR6, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR7, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR8, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_0, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR9, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR10, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR11, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_11, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR12,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_12, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR13,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR14,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR15,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR16,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},			\
	{EH_IO_IIC1_SCL, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},					\
	{EH_IO_IIC1_SDA, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},					\
	{EH_I_CMP4,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_I_CMP5,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_O_DAC1,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, TRUE},								\
	{EH_O_DAC2,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, TRUE},								\
	{EH_IO_GP3, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_8, TRUE, FALSE, FALSE},				\
	{EH_IO_GP4, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},				\
	{EH_IO_K33A, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_IO_USB5V,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{IO_Total_Discrete_Count, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},	\
	{KERNEL_RXD1, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_12, FALSE, FALSE, FALSE},		\
	{KERNEL_RXD0, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_13, FALSE, FALSE, FALSE},		\
	{KERNEL_RXDV, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_14, FALSE, FALSE, FALSE},		\
	{KERNEL_TXEN, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_15, FALSE, FALSE, FALSE},		\
	{KERNEL_TXD0, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_16, FALSE, FALSE, FALSE},		\
	{KERNEL_TXD1, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_17, FALSE, FALSE, FALSE},		\
	{KERNEL_MDIO, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_0, FALSE, FALSE, FALSE},			\
	{KERNEL_MDC, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_1, FALSE, FALSE, FALSE},			\
	{KERNEL_CPURX, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_16, FALSE, FALSE, FALSE},		\
	{KERNEL_CPUTX, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_17, FALSE, FALSE, FALSE}		\
}	
#endif

#ifdef BUILD_MK64
#define IO_nPadResourceMap {\
	{EH_IO_GPSE1,	PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_17, TRUE, FALSE, FALSE},     \
	{EH_IO_GPSE2,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_0, TRUE, FALSE, FALSE},     \
	{EH_IO_GPSE3,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_0, TRUE, FALSE, FALSE},    \
	{EH_IO_GPSE4,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},     \
	{EH_IO_GPSE5,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},     \
	{EH_IO_GPSE6,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},     \
	{EH_IO_GPSE7, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},       \
	{EH_IO_GPSE8, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_5, TRUE, FALSE, FALSE},       \
	{EH_I_ADD1,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                         \
	{EH_I_ADD2,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                         \
	{EH_I_CMP1,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_6, TRUE, FALSE, FALSE},         \
	{EH_I_CMP2,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},         \
	{IO_AD_Count,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},                    \
	{EH_IO_CAN1T,	PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_12, FALSE, FALSE, FALSE},   \
	{EH_IO_CAN1R,	PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_13, FALSE, FALSE, FALSE},   \
	{EH_IO_UART1_TX, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_16, FALSE, FALSE, FALSE},  \
	{EH_IO_UART1_RX, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_17, FALSE, FALSE, FALSE},  \
	{EH_IO_UART1_CTS, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_11, TRUE, FALSE, FALSE},  \
	{EH_IO_UART1_RTS, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},  \
	{EH_IO_UART2_TX, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_8, FALSE, FALSE, FALSE},    \
	{EH_IO_UART2_RX, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_9, FALSE, FALSE, FALSE},    \
	{EH_IO_SPI1_MISO, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_23, FALSE, FALSE, FALSE}, \
	{EH_IO_SPI1_MOSI, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_22, FALSE, FALSE, FALSE}, \
	{EH_IO_SPI1_CLK, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_21, FALSE, FALSE, FALSE},  \
	{EH_IO_SPI1_CS, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_20, FALSE, FALSE, FALSE},   \
	{EH_IO_TMR1, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_0, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR2, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_7, FALSE, FALSE, FALSE},        \
	{EH_IO_TMR3, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_4, FALSE, FALSE, FALSE},        \
	{EH_IO_TMR4, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_2, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR5, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_3, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR6, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_3, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR7, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_4, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR8, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_18, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR9, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_19, FALSE, FALSE, FALSE},       \
	{EH_IO_TMR10, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_8, TRUE, FALSE, FALSE},       \
	{EH_IO_TMR11, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},       \
	{EH_IO_TMR12, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},       \
	{EH_IO_TMR13, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_11, TRUE, FALSE, FALSE},     \
	{EH_IO_TMR14, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},     \
	{EH_IO_TMR15, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_5, TRUE, FALSE, FALSE},	      \
	{EH_IO_TMR16, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_6, TRUE, FALSE, FALSE},       \
	{EH_IO_TMR17, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},      \
	{EH_IO_TMR18, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},      \
	{EH_IO_IIC1_SCL, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_24, TRUE, FALSE, FALSE},   \
    {EH_IO_IIC1_SDA, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_25, TRUE, FALSE, FALSE},   \
	{EH_O_DAC1, PIMAPI_enPortInvalid, 0u, FALSE, FALSE, TRUE},                        \
	{EH_O_DAC2, PIMAPI_enPortInvalid, 0u, FALSE, FALSE, TRUE},                        \
	{EH_IO_GP1, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_12, FALSE, FALSE, FALSE},             \
	{EH_IO_GP2, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_15, FALSE, FALSE, FALSE},             \
	{EH_IO_GP3, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_10, FALSE, FALSE, FALSE},				\
	{EH_IO_GP4, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_11, FALSE, FALSE, FALSE},				\
	{EH_IO_GP5, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_5, FALSE, FALSE, FALSE},				\
	{EH_IO_GP6, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_14, FALSE, FALSE, FALSE},				\
	{EH_IO_GP7, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_13, FALSE, FALSE, FALSE},				\
	{EH_IO_GP8, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_14, FALSE, FALSE, FALSE},				\
	{EH_IO_GP9, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_15, FALSE, FALSE, FALSE},				\
	{EH_IO_GP10, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_16, FALSE, FALSE, FALSE},				\
	{EH_IO_GP11, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_28, FALSE, FALSE, FALSE},				\
	{EH_IO_GP12, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_29, FALSE, FALSE, FALSE},				\
	{EH_IO_GP13, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_26, FALSE, FALSE, FALSE},				\
	{EH_IO_GP14, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_11, FALSE, FALSE, FALSE},				\
	{EH_IO_K33A, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_26, FALSE, FALSE, FALSE},							\
	{EH_IO_USB5V,	PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_5, FALSE, FALSE, FALSE},						\
	{IO_Total_Discrete_Count, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},	\
}
#endif




bool IO_boGetDIOResource(IOAPI_tenEHIOResource);
IOAPI_tenTriState IO_enGetDIOResourceState(IOAPI_tenEHIOResource);
void IO_vInitDIOResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, IOAPI_tenDriveStrength);
void IO_vAssertDIOResource(IOAPI_tenEHIOResource, IOAPI_tenTriState);
void IO_vInitADCResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, ADCAPI_tstADCCB*);
void IO_vInitDACResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, DACAPI_tstDACCB*);
SYSAPI_tenSVCResult IO_enInitCommsResource(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
SYSAPI_tenSVCResult IO_enInitTEPMChannel(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMChannelCB*, bool boPWMMode);
	
#endif //KERNELIO_H
