/******************************************************************************/
/* data link layer header file                                                */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef DLL_H
#define DLL_H

//#include "netif/ethernetif.h"
#include <TYPES.h>
#include "OS.h"
#include "IOAPI.h"

#define RX_BUFF_SIZE 							192u
#define TX_BUFF_SIZE 							32u
#define RX_SEG_FRAME_UNSEG 				0x00u
#define RX_SEG_FRAME_ONE 					0x10u
#define RX_SEG_FRAME_SEQ 					0x20u
#define TX_BUFF_COMPLETE					0xFFu
#define RX_BUFF_FULL							0xFFu
#define DLL_nTXIPBufferSize				1024u

#define	RX_BUFF_SMALL_SIZE	21u
#define DLL_ETH_SOURCE_PORT_OPEN 1
#define DLL_ETH_SOURCE_IP_OPEN   1

/* Warning do NOT make the working buffer
   sizes LESS than the max frame bytes!! */
#define DLL_nIICTXWorkBuffMaxBytes		16u
#define DLL_nSPITXWorkBuffMaxBytes		16u
#define DLL_nUARTTXWorkBuffMaxBytes		24u
#define DLL_nCANTXWorkBuffMaxBytes		256u
#define DLL_nUSBTXWorkBuffMaxBytes		256u
#define DLL_nENETTXWorkBuffMaxBytes		1024u

#define DLL_nIICTXWorkBuffCount				2u
#define DLL_nSPITXWorkBuffCount				2u
#define DLL_nUARTTXWorkBuffCount			2u
#define DLL_nCANTXWorkBuffCount				1u
#define DLL_nUSBTXWorkBuffCount				1u
#define DLL_nENETTXWorkBuffCount			1u
#define DLL_nClientTXBufferCount	    (DLL_nIICTXWorkBuffCount +		\
																			DLL_nSPITXWorkBuffCount +			\
																			DLL_nUARTTXWorkBuffCount +		\
																			DLL_nCANTXWorkBuffCount +			\
																			DLL_nENETTXWorkBuffCount)

#define DLL_nIICVirtualChannelCount		(EH_VIO_SPI1 - EH_VIO_IIC1)
#define DLL_nSPIVirtualChannelCount		(EH_VIO_UART1 - EH_VIO_SPI1)
#define DLL_nUARTVirtualChannelCount	(EH_VIO_CAN1 - EH_VIO_UART1)
#define DLL_nCANVirtualChannelCount		(EH_VIO_USB - EH_VIO_CAN1)

#if defined(BUILD_MK60)
#define DLL_nUSBVirtualChannelCount		(EH_VIO_ENET_CH1 - EH_VIO_USB)
#define DLL_nENETVirtualChannelCount	(EH_VIO_TPM0 - EH_VIO_ENET_CH1)
#endif //defined(BUILD_MK60)

#if defined(BUILD_MKS20)
#define DLL_nUSBVirtualChannelCount		1
#define DLL_nENETVirtualChannelCount	0
#endif //defined(BUILD_MKS20)

#define DLL_nIICTXQueueByteCount			(DLL_nIICTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nSPITXQueueByteCount			(DLL_nSPITXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nUARTTXQueueByteCount			(DLL_nUARTTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nCANTXQueueByteCount			(DLL_nCANTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nUSBTXQueueByteCount			(DLL_nUSBTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nENETTXQueueByteCount			(DLL_nENETTXWorkBuffMaxBytes * 2u) + 1u

#define DLL_nMaxTXFrameBytes    \
{                               \
	16,                         \
	16,                         \
	16,                         \
	16,                         \
	24,                         \
	24,                         \
	24,                         \
	24,                         \
	24,                         \
	24,                         \
	12,                         \
	256,                        \
	1024,                       \
	1024,                       \
	1024,                       \
	1024                        \
}

#ifdef BUILD_PBL
	#define DLL_xGetClientHandle()	0xf0000000
#endif
#ifdef BUILD_SBL
	#define DLL_xGetClientHandle()	0xf0000000
#endif
#ifdef BUILD_KERNEL
	#define DLL_xGetClientHandle()	OS_stSVCDataStruct.tClientHandle | 0xf0000000
#endif
#ifdef BUILD_KERNEL_APP
	#define DLL_xGetClientHandle()	OS_stSVCDataStruct.tClientHandle | 0xf0000000
#endif

#if RX_BUFF_SIZE > 256u
	#warning "RX buffer size"
#endif

#if TX_BUFF_SIZE > 256u
	#warning "TX buffer size"
#endif

#define DLL_nVirtualChannelCount (EH_VIO_TPM0 - EH_VIO_IIC1)
#define DLL_nTXBytesBufferSize	25u

typedef void (*pfCallBackDLLToModule)(uint8*);
typedef sint32 DLL_tDLLVirtualChannel;

typedef struct
{
	uint8 u8Data[RX_BUFF_SIZE];
	uint8 u8DataCount;
	uint8 u8RXByteIDX;
	uint8 biFramed : 1;
	uint8 biWithCRC : 1;
	uint8 biRXFull : 1;
	uint8 biSpare : 5;
} DLL_tstRXDLLData;


typedef struct
{
	puint8 pu8TXData;
	uint8 u8ByteCount;
} DLL_tstTXCB;

typedef struct
{
	uint8 u8Data[DLL_nTXBytesBufferSize];
	uint8 u8DataCount;
	uint8 u8TXByteIDX;
	uint8 biFramed : 1;
	uint8 biWithCRC : 1;
	uint8 biTXEmpty : 1;
	uint8 biSpare : 5;	
} DLL_tstTXBytesDLLData;

typedef struct
{
	uint8 u8Data[RX_BUFF_SMALL_SIZE];
	uint8* pu8RX;
	uint8 u8RXRequestedCount;
	uint8 u8RXReceivedCount;
} DLL_tstRXSmallBuffer;


DLL_tstRXDLLData* DLL_pstGetRXBuffer(IOAPI_tenEHIOResource);
void DLL_vFrameRXCB(IOAPI_tenEHIOResource, puint8);
void DLL_vStart(puint32 const);
void DLL_vRun(puint32 const);
void DLL_vTerminate(puint32 const);
void DLL_vReceiveUARTBytes(IOAPI_tenEHIOResource, DLL_tstRXDLLData*);
bool DLL_vQueueCANMessage(IOAPI_tenEHIOResource, PROTAPI_tstCANMsg*);	
bool DLL_boQueueMessage(IOAPI_tenEHIOResource, puint8, uint32);
DLL_tDLLVirtualChannel DLL_tGetVirtualChannel(IOAPI_tenEHIOResource);
bool DLL_boInitDLLChannel(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
IOAPI_tenPortMode DLL_enGetChannelMode(IOAPI_tenEHIOResource);
void* DLL_pvGetBuffered(IOAPI_tenEHIOResource);

#endif // DLL_H

