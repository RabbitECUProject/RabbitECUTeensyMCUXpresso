/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          IOAPI                                                  */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed I/O               */
/*                     initialization, functions and events                   */
/*                                                                            */
/* FILE NAME:          IOAPI.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IOAPI_H
#define IOAPI_H

#include "build.h"
#include "SYS.h"
#include "PROTAPI.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{
	IOAPI_IO_RX = 0,
	IOAPI_IO_TX = 1,
	IOAPI_IO_MEM_READ = 2,
	IOAPI_IO_MEM_WRITE = 3
} IOAPI_enTransferType;
/*CR1_103*/

typedef enum
{
	IOAPI_enPortComms,
	IOAPI_enPortSerialTransfer,
	IOAPI_enPortNone
} IOAPI_tenPortMode;

#if defined(BUILD_MK60) && defined(BUILD_ECUHOST_1_4)
typedef enum
{
	EH_IO_ADSE1 = 0,
	EH_IO_ADSE2 = 1,
	EH_IO_ADSE3 = 2,
	EH_IO_ADSE4 = 3,
	EH_IO_ADSE5 = 4,
	EH_IO_ADSE6 = 5,
	EH_IO_ADSE7 = 6,
	EH_IO_ADSE8 = 7,
	EH_IO_ADSE9 = 8,
	EH_IO_ADSE10 = 9,
	EH_IO_ADSE11 = 10,
	EH_IO_ADSE12 = 11,	
	EH_IO_GPSE1 = 12,
	EH_IO_GPSE2 = 13,
	EH_IO_GPSE5 = 14,
	EH_IO_GPSE6 = 15,
	EH_IO_GPSE7 = 16,
	EH_IO_GPSE8 = 17,
	EH_IO_GPSE9 = 18,
	EH_IO_GPSE10 = 19,
	EH_I_ADD1 = 20,
	EH_I_ADD2 = 21,
	EH_I_ADD3 = 22,
	EH_I_ADD4 = 23,
	EH_I_ADD5 = 24,
	EH_I_ADD6 = 25,
	EH_I_ADD7 = 26,
	EH_I_ADD8 = 27,	
	EH_IO_CAN2T = 28,
	EH_IO_CAN2R = 29,		
	EH_I_CMP1 = 30,
	EH_I_CMP2 = 31,
	EH_I_CMP3 = 32,	
	IO_AD_Count = 33,
	EH_IO_UART1_TX = 34,
	EH_IO_UART1_RX = 35,
	EH_IO_UART1_CTS = 36,
	EH_IO_UART1_RTS = 37,
	EH_IO_UART2_TX = 38,
	EH_IO_UART2_RX = 39,
	EH_IO_UART2_CTS = 40,
	EH_IO_UART2_RTS = 41,	
	EH_IO_CAN1T = 42,
	EH_IO_CAN1R = 43,
	EH_IO_TMR1 = 44,
	EH_IO_TMR2 = 45,
	EH_IO_TMR3 = 46,
	EH_IO_TMR4 = 47,
	EH_IO_TMR5 = 48,
	EH_IO_TMR6 = 49,
	EH_IO_TMR7 = 50,
	EH_IO_TMR8 = 51,
	EH_IO_TMR9 = 52,
	EH_IO_TMR10 = 53,
	EH_IO_TMR11 = 54,
	EH_IO_TMR12 = 55,
	EH_IO_TMR13 = 56,
	EH_IO_TMR14 = 57,
	EH_IO_TMR15 = 58,
	EH_IO_TMR16 = 59,	
	EH_IO_IIC1_SCL = 60,
	EH_IO_IIC1_SDA = 61,	
	EH_I_CMP4 = 62,
	EH_I_CMP5 = 63,	
	EH_O_DAC1 = 64,
	EH_O_DAC2 = 65,
	EH_IO_GP3 = 66,
	EH_IO_GP4 = 67,	
	EH_IO_K33A = 68,
	EH_IO_USB5V = 69,
	IO_Total_Discrete_Count = 70,
	EH_VIO_IIC1 = 100,
	EH_VIO_IIC2 = 101,
	EH_VIO_SPI1 = 102,
	EH_VIO_SPI2 = 103,		
	EH_VIO_UART1 = 104,
	EH_VIO_UART2 = 105,
	EH_VIO_UART3 = 106,
	EH_VIO_UART4 = 107,
	EH_VIO_UART5 = 108,
	EH_VIO_UART6 = 109,
	EH_VIO_CAN1 = 110,
	EH_VIO_CAN2 = 111,
	EH_VIO_ENET_CH1 = 112,
	EH_VIO_ENET_CH2 = 113,
	EH_VIO_ENET_CH3 = 114,
	EH_VIO_ENET_CH4 = 115,
	EH_VIO_TPM0 = 116,
	EH_VIO_TPM1 = 117,
	EH_VIO_TPM2 = 118,
	EH_VIO_FTM3 = 119, 
	IO_Total_Count,
	EH_IO_Invalid
} IOAPI_tenEHIOResource;	

#define EH_FIRST_TMR      EH_IO_TMR1
#define EH_LAST_TMR	      EH_IO_TMR16
#endif //BUILD_MK60

#ifdef BUILD_MK64
typedef enum
{
	EH_IO_GPSE1 = 0,
	EH_IO_GPSE2 = 1,
	EH_IO_GPSE3 = 2,
	EH_IO_GPSE4 = 3,
	EH_IO_GPSE5 = 4,
	EH_IO_GPSE6 = 5,
	EH_IO_GPSE7 = 6,
	EH_IO_GPSE8 = 7,
	EH_I_ADD1 = 8,
	EH_I_ADD2 = 9,
	EH_I_CMP1 = 10,
	EH_I_CMP2 = 11,
	IO_AD_Count = 12,
	EH_IO_CAN1T = 13,
	EH_IO_CAN1R = 14,
	EH_IO_UART1_TX = 15,
	EH_IO_UART1_RX = 16,
	EH_IO_UART1_CTS = 17,
	EH_IO_UART1_RTS = 18,
	EH_IO_UART2_TX = 19,
	EH_IO_UART2_RX = 20,
	EH_IO_SPI1_MISO = 21,
	EH_IO_SPI1_MOSI = 22,
	EH_IO_SPI1_CLK = 23,
	EH_IO_SPI1_CS = 24,
	EH_IO_TMR1 = 25,
	EH_IO_TMR2 = 26,
	EH_IO_TMR3 = 27,
	EH_IO_TMR4 = 28,
	EH_IO_TMR5 = 29,
	EH_IO_TMR6 = 30,
	EH_IO_TMR7 = 31,
	EH_IO_TMR8 = 32,
	EH_IO_TMR9 = 33,
	EH_IO_TMR10 = 34,
	EH_IO_TMR11 = 35,
	EH_IO_TMR12 = 36,
	EH_IO_TMR13 = 37,
	EH_IO_TMR14 = 38,
	EH_IO_TMR15 = 39,
	EH_IO_TMR16 = 40,
	EH_IO_TMR17 = 41,
	EH_IO_TMR18 = 42,
	EH_IO_IIC1_SCL = 43,
	EH_IO_IIC1_SDA = 44,
	EH_O_DAC1 = 45,
	EH_O_DAC2 = 46,
	EH_IO_GP1 = 47,
	EH_IO_GP2 = 48,
	EH_IO_GP3 = 49,
	EH_IO_GP4 = 50,
	EH_IO_GP5 = 51,
	EH_IO_GP6 = 52,
	EH_IO_GP7 = 53,
	EH_IO_GP8 = 54,
	EH_IO_GP9 = 55,
	EH_IO_GP10 = 56,
	EH_IO_GP11 = 57,
	EH_IO_GP12 = 58,
	EH_IO_GP13 = 59,
	EH_IO_GP14 = 60,
	EH_IO_K33A = 61,
	EH_IO_USB5V = 62,
	IO_Total_Discrete_Count,
	EH_VIO_IIC1 = 100,
	EH_VIO_IIC2 = 101,
	EH_VIO_SPI1 = 102,
	EH_VIO_SPI2 = 103,
	EH_VIO_UART1 = 104,
	EH_VIO_UART2 = 105,
	EH_VIO_UART3 = 106,
	EH_VIO_UART4 = 107,
	EH_VIO_UART5 = 108,
	EH_VIO_UART6 = 109,
	EH_VIO_CAN1 = 110,
	EH_VIO_USB = 111,
	EH_VIO_ENET_CH1 = 112,
	EH_VIO_ENET_CH2 = 113,
	EH_VIO_ENET_CH3 = 114,
	EH_VIO_ENET_CH4 = 115,
	EH_VIO_TPM0 = 116,
	EH_VIO_TPM1 = 117,
	EH_VIO_TPM2 = 118,
	EH_VIO_FTM3 = 119,
	EH_VIO_ADC0 = 120,
	EH_VIO_ADC1 = 121,
	EH_VIO_ADC2 = 122,
	EH_VIO_ADC3 = 123,
	EH_VIO_REL1 = 124,
	EH_VIO_REL2 = 125,
	EH_VIO_REL3 = 126,
	EH_VIO_REL4 = 127,
	EH_VIO_REL5 = 128,
	EH_VIO_REL6 = 129,
	EH_VIO_REL7 = 130,
	EH_VIO_REL8 = 131,
	IO_Total_Count,
	EH_IO_Invalid
} IOAPI_tenEHIOResource;

#define EH_FIRST_TMR        EH_IO_TMR1
#define EH_LAST_TMR	        EH_IO_TMR18
#define EH_FIRST_UART       EH_VIO_UART1
#define EH_LAST_UART        EH_VIO_UART2
#define EH_FIRST_IIC        EH_VIO_IIC1
#define EH_LAST_IIC         EH_VIO_IIC2
#define EH_FIRST_SPI        EH_VIO_SPI1
#define EH_LAST_SPI         EH_VIO_SPI2
#define EH_FIRST_CAN        EH_VIO_CAN1
#define EH_LAST_CAN         EH_VIO_CAN1
#define EH_FIRST_USB        EH_VIO_USB
#define EH_LAST_USB         EH_VIO_USB
#define EH_FIRST_VIO_COMMS  EH_VIO_IIC1
#define EH_LAST_VIO_COMMS   EH_VIO_ENET_CH2

#endif //BUILD_MK64


#ifdef BUILD_MKS20
typedef enum
{
	EH_IO_GPSE1 = 0,
	EH_IO_GPSE2 = 1,
	EH_IO_GPSE3 = 2,
	EH_IO_GPSE4 = 3,
	EH_IO_GPSE5 = 4,
	EH_IO_GPSE6 = 5,
	EH_IO_GPSE7 = 6,
	EH_IO_GPSE8 = 7,
	EH_IO_GPSE9 = 8,
	EH_IO_GPSE10 = 9,
	EH_IO_GPSE11 = 10,
	EH_I_ADD1 = 11,
	EH_I_ADD2 = 12,
	EH_I_ADD3 = 13,
	EH_I_ADD4 = 14,
	EH_I_ADD5 = 15,
	EH_I_CMP1 = 16,
	EH_I_CMP2 = 17,
	EH_I_CMP3 = 18,
	IO_AD_Count = 19,
	EH_IO_CAN1T = 20,
	EH_IO_CAN1R = 21,
	EH_IO_UART1_TX = 22,
	EH_IO_UART1_RX = 23,
	EH_IO_SPI1_MISO = 24,
	EH_IO_SPI1_MOSI = 25,
	EH_IO_SPI1_CLK = 26,
	EH_IO_TMR1 = 27,
	EH_IO_TMR2 = 28,
	EH_IO_TMR3 = 29,
	EH_IO_TMR4 = 30,
	EH_IO_TMR5 = 31,
	EH_IO_TMR6 = 32,
	EH_IO_TMR7 = 33,
	EH_IO_TMR8 = 34,
	EH_IO_TMR9 = 35,
	EH_IO_TMR10 = 36,
	EH_IO_GP1 = 37,
	EH_IO_GP2 = 38,
	EH_IO_GP3 = 39,
	EH_IO_GP4 = 40,
	EH_IO_GP5 = 41,
	EH_IO_GP6 = 42,
	EH_IO_GP7 = 43,
	EH_IO_GP8 = 44,
	EH_IO_GP9 = 45,
	IO_Total_Discrete_Count,
	EH_VIO_IIC1 = 47,
	EH_VIO_SPI1 = 48,
	EH_VIO_SPI2 = 49,
	EH_VIO_UART1 = 50,
	EH_VIO_UART2 = 51,
	EH_VIO_UART3 = 52,
	EH_VIO_CAN1 = 53,
	EH_VIO_USB = 54,
	EH_VIO_TPM0 = 55,
	EH_VIO_TPM1 = 56,
	EH_VIO_TPM2 = 57,
	EH_VIO_ADC0 = 58,
	EH_VIO_REL1 = 59,
	EH_VIO_REL2 = 60,
	EH_VIO_REL3 = 61,
	EH_VIO_REL4 = 62,
	EH_VIO_REL5 = 63,
	EH_VIO_REL6 = 64,
	EH_VIO_REL7 = 65,
	EH_VIO_REL8 = 66,
	EH_VIO_EXPBIT0 = 67,
	EH_VIO_EXPBIT1 = 68,
	EH_VIO_EXPBIT2 = 69,
	EH_VIO_EXPBIT3 = 70,
	EH_VIO_EXPBIT4 = 71,
	EH_VIO_EXPBIT5 = 72,
	EH_VIO_EXPBIT6 = 73,
	EH_VIO_EXPBIT7 = 74,
	EH_VIO_EXPBIT8 = 75,
	EH_VIO_EXPBIT9 = 76,
	EH_VIO_EXPBIT10 = 77,
	EH_VIO_EXPBIT11 = 78,
	EH_VIO_EXPBIT12 = 79,
	EH_VIO_EXPBIT13 = 80,
	EH_VIO_EXPBIT14 = 81,
	EH_VIO_EXPBIT15 = 82,
	EH_VIO_EXPBIT16 = 83,
	EH_VIO_EXPBIT17 = 84,
	EH_VIO_EXPBIT18 = 85,
	EH_VIO_EXPBIT19 = 86,
	EH_VIO_EXPBIT20 = 87,
	EH_VIO_EXPBIT21 = 88,
	EH_VIO_EXPBIT22 = 89,
	EH_VIO_EXPBIT23 = 90,
	EH_VIO_EXPBIT24 = 91,
	EH_VIO_EXPBIT25 = 92,
	EH_VIO_EXPBIT26 = 93,
	EH_VIO_EXPBIT27 = 94,
	EH_VIO_EXPBIT28 = 95,
	EH_VIO_EXPBIT29 = 96,
	EH_VIO_EXPBIT30 = 97,
	EH_VIO_EXPBIT31 = 98,
	EH_VIO_IMPADC0 = 99,
	EH_VIO_IMPADC1 = 100,
	EH_VIO_IMPADC2 = 101,
	EH_VIO_IMPADC3 = 102,
	EH_VIO_IMPADC4 = 103,
	EH_VIO_IMPADC5 = 104,
	EH_VIO_IMPADC6 = 105,
	EH_VIO_IMPADC7 = 106,
	EH_VIO_IMPADC8 = 107,
	EH_VIO_IMPADC9 = 108,
	EH_VIO_IMPADC10 = 109,
	EH_VIO_IMPADC11 = 110,
	EH_VIO_IMPADC12 = 111,
	EH_VIO_IMPADC13 = 112,
	EH_VIO_IMPADC14 = 113,
	EH_VIO_IMPADC15 = 114,
	IO_Total_Count,
	EH_IO_Invalid
} IOAPI_tenEHIOResource;

#define EH_FIRST_TMR        EH_IO_TMR1
#define EH_LAST_TMR	        EH_IO_TMR10
#define EH_FIRST_UART       EH_VIO_UART1
#define EH_LAST_UART        EH_VIO_UART3
#define EH_FIRST_IIC        EH_VIO_IIC1
#define EH_LAST_IIC         EH_VIO_IIC1
#define EH_FIRST_SPI        EH_VIO_SPI1
#define EH_LAST_SPI         EH_VIO_SPI2
#define EH_FIRST_CAN        EH_VIO_CAN0
#define EH_LAST_CAN         EH_VIO_CAN0
#define EH_FIRST_USB        EH_VIO_USB
#define EH_LAST_USB         EH_VIO_USB
#define EH_FIRST_VIO_COMMS  EH_VIO_IIC1
#define EH_LAST_VIO_COMMS   EH_VIO_CAN0

#endif //BUILD_MKS20


typedef enum
{
	IOAPI_enDIOOutput,
	IOAPI_enDIOInput,
	IOAPI_enGPSE,
	IOAPI_enADD,
	IOAPI_enDAC,
	IOAPI_enCaptureCompare,
	IOAPI_enPWM,
	IOAPI_enIICBus,
	IOAPI_enIISBus,
	IOAPI_enSPIBus,
	IOAPI_enUARTBus,
	IOAPI_enCANBus,
	IOAPI_enENETChannel,
	IOAPI_enUSBChannel,
	IOAPI_enTEPM
} IOAPI_tenEHIOType;

typedef enum
{
    IOAPI_enEdgeRising,
	IOAPI_enEdgeFalling,
	IOAPI_enEdgeBoth
} IOAPI_tenEdgePolarity;

typedef enum
{
	IOAPI_enLow,
	IOAPI_enHigh,
	IOAPI_enToggle,
	IOAPI_enHiZ,
	IOAPI_enError
} IOAPI_tenTriState;

typedef enum
{
	IOAPI_enWeak,
	IOAPI_enStrong
} IOAPI_tenDriveStrength;

typedef void (*IOAPI_tpfTransferCB)(IOAPI_tenEHIOResource, puint8, uint32, IOAPI_enTransferType);
	
typedef struct
{
	uint32 u32MasterAddress;
	uint32 u32SlaveAddress;	
} tstIICNetInfo;

typedef struct
{
	uint32 u32Dummy1;
	uint32 u32Dummy2;
} tstUARTNetInfo;
		
typedef struct
{
	uint32 u32GlobalCANDiagAddress;
	uint32 u32CANDiagAddress;
	uint32 u32CANPriorityAddress[4];
	uint32 u32CANPriorityMask[4];
} tstCANNetInfo;	

typedef struct
{
	uint16 u16RPCREQSourcePort;
	uint16 u16RPCREQDestPort;
} tstLANNetInfo;

typedef struct
{
	union
	{
		tstIICNetInfo stIICNetInfo;
		tstCANNetInfo stCANNetInfo;
		tstLANNetInfo stLANNetInfo;
		tstUARTNetInfo stUARTNetInfo;
	} uNetInfo;
} IOAPI_tstNetConfig;

typedef struct
{
	IOAPI_tenEHIOResource enRXPin;
	IOAPI_tenEHIOResource enTXPin;
	IOAPI_tenEHIOResource enRTSPin;
	IOAPI_tenEHIOResource enCTSPin;
} IOAPI_tstUARTPinInfo;

typedef struct
{
	IOAPI_tenEHIOResource enRXPin;
	IOAPI_tenEHIOResource enTXPin;
} IOAPI_tstCANPinInfo;
/*CR1_104*/

typedef struct
{
	IOAPI_tenEHIOResource enSDAPin;
	IOAPI_tenEHIOResource enSCLPin;
} IOAPI_tstIICPinInfo;

typedef struct
{
	IOAPI_tenEHIOResource enMOSIPin;
	IOAPI_tenEHIOResource enMISOPin;
	IOAPI_tenEHIOResource enSCKPin;
	bool boCaptureRising;
	bool boShiftRising;
	bool boIdleHigh;
	bool boDataIdleHigh;
} IOAPI_tstSPIPinInfo;

typedef struct
{ 
	union
	{
		IOAPI_tstUARTPinInfo stUARTPinInfo;
		IOAPI_tstIICPinInfo stIICPinInfo;
		IOAPI_tstSPIPinInfo stSPIPinInfo;
		IOAPI_tstCANPinInfo stCANPinInfo;
	} uPinInfo;
} IOAPI_tstPinConfig;

typedef struct
{
	IOAPI_tstNetConfig stNetConfig;
	IOAPI_tstPinConfig stPinConfig;
	IOAPI_tenPortMode enPortMode;
	uint32 u32BaudRateHz;
	uint32 u32TXPacketRateMs;
	IOAPI_tenEHIOResource enVIOResource;
	PROTAPI_tenLLProtocol enLLProtocol;
	PROTAPI_tenNLProtocol enNLProtocol;	
	PROTAPI_tenTLProtocol enTLProtocol;
	IOAPI_tpfTransferCB pfRXTransferCB;
	IOAPI_tpfTransferCB pfTXTransferCB;	
} IOAPI_tstPortConfigCB;


typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_enTransferType enTransferType;
	IOAPI_tpfTransferCB pfCB;
	void* pvData;
	uint32 u32ByteCount;
	bool boBlockingMode;
} IOAPI_tstTransferCB;

#endif //IOAPI_H

