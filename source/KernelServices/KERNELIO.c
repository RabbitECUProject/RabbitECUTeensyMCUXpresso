/******************************************************************************/
/*    Copyright (c) 2018 MD Automotive Controls. Original Work.               */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   20-06-2018 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <DECLARATIONS.h>
#include <KERNELIO.h>
#include "mk64f12.h"
#include "OS.h"
#include "DAC.h"
#include "PERCAN.h"
#include "PERUART.h"
#include "IIC.h"
#include "PIM.h"
#include "ADCAPI.h"
#include "DACAPI.h"
#include "IOAPI.h"
#include "IICAPI.h"
#include "PERSPI.h"
#include "PERADC.h"
#include "TEPM.h"
#include "RESM.h"

extern SYSAPI_tenSVCResult VCOM_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB);
const IO_tstEHPadResource IO_rastEHPadResource[IO_Total_Discrete_Count + KERNEL_DISCRETE_HIDDEN] = IO_nPadResourceMap;
IOAPI_tenEHIOResource IO_aenEHIOMasterList[RESM_nMastersMax];

static void IO_vSetIOMux(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, uint32);
static void IO_vClearMasterList(void);

void IO_vInitDIOResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enIOType, IOAPI_tenDriveStrength enDriveStrength)
{
	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Init DIO only if port bit non zero signifies DIO supported */
	{	
		PIM_vInitPortBit(IO_rastEHPadResource[enIOResource].enPort, enIOType, IO_rastEHPadResource[enIOResource].u32PortBit);	
	}
}

void IO_vInitADCResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tstADCCB* pstADCCB)
{
	if (((0u != IO_rastEHPadResource[enIOResource].boIsADSE) && (IOAPI_enADSE == enEHIOType))
		|| ((0u != IO_rastEHPadResource[enIOResource].boIsADD) && (IOAPI_enADD == enEHIOType)))
	/* Init DIO only if port bit non zero signifies DIO supported */
	{	
#if defined(BUILD_KERNEL) || defined(BUILD_KERNEL_APP)
		ADC_vInitADCResource(enIOResource, enEHIOType, pstADCCB);			
#endif		
	}
}

void IO_vInitDACResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enEHIOType, DACAPI_tstDACCB* pstDACCB)
{
	if ((0u != IO_rastEHPadResource[enIOResource].boIsDAC) && (IOAPI_enDAC == enEHIOType))

	/* Init DIO only if port bit non zero signifies DIO supported */
	{
#if defined(BUILD_KERNEL) || defined(BUILD_KERNEL_APP)
		DAC_vInitDACResource(enIOResource, pstDACCB);			
#endif		
	}
}


SYSAPI_tenSVCResult IO_enInitCommsResource(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	SYSAPI_tenSVCResult enSVCResult;
	uint32 u32MuxSel;
	
	if ((EH_VIO_IIC1 == enEHIOResource) || (EH_VIO_IIC2 == enEHIOResource))
	{
#ifdef BUILD_PBL
#elif defined(BUILD_SBL)
#elif defined(BUILD_KERNEL)
		enSVCResult = IIC_enInitBus(enEHIOResource, pstPortConfigCB);
#elif defined(BUILD_KERNEL_APP)
		enSVCResult = IIC_enInitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stIICPinInfo.enSDAPin, IOAPI_enIICBus, 5u);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stIICPinInfo.enSCLPin, IOAPI_enIICBus, 5u);
#endif		
	}
	
	else if ((EH_VIO_UART1 == enEHIOResource) 
				|| (EH_VIO_UART2 == enEHIOResource) 
				|| (EH_VIO_UART3 == enEHIOResource) 
				|| (EH_VIO_UART4 == enEHIOResource) 
				|| (EH_VIO_UART5 == enEHIOResource) 	
				|| (EH_VIO_UART6 == enEHIOResource))
	{
		enSVCResult = UART_enInitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stUARTPinInfo.enRXPin, IOAPI_enUARTBus, 3u);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stUARTPinInfo.enTXPin, IOAPI_enUARTBus, 3u);
	}	
	
#ifdef EH_VIO_CAN2
	else if ((EH_VIO_CAN1 == enEHIOResource) 	
				|| (EH_VIO_CAN2 == enEHIOResource))
#endif
#ifndef EH_VIO_CAN2
	else if (EH_VIO_CAN1 == enEHIOResource)
#endif

	{
		u32MuxSel = CAN_u32InitBus(enEHIOResource, pstPortConfigCB);

		if (~0 != u32MuxSel)
		{
			IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stCANPinInfo.enRXPin, IOAPI_enCANBus, u32MuxSel);
			IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stCANPinInfo.enTXPin, IOAPI_enCANBus, u32MuxSel);
			enSVCResult = SYSAPI_enOK;
		}
		else
		{
			enSVCResult = SYSAPI_enFail;
		}
	}

#ifdef EH_VIO_SPI2
	else if ((EH_VIO_SPI1 == enEHIOResource)
				|| (EH_VIO_SPI2 == enEHIOResource))
#endif
#ifndef EH_VIO_SPI2
	else if (EH_VIO_SPI1 == enEHIOResource)
#endif

	{
		u32MuxSel = SPI_u32InitBus(enEHIOResource, pstPortConfigCB);

		if (~0 != u32MuxSel)
		{
			IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMOSIPin, IOAPI_enSPIBus, u32MuxSel);
			IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMISOPin, IOAPI_enSPIBus, u32MuxSel);
			IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enSCKPin, IOAPI_enSPIBus, u32MuxSel);
			enSVCResult = SYSAPI_enOK;
		}
		else
		{
			enSVCResult = SYSAPI_enFail;
		}
	}

	
#ifdef BUILD_ENET
	else if ((EH_VIO_ENET_CH1 == enEHIOResource)
				|| (EH_VIO_ENET_CH2 == enEHIOResource)
				|| (EH_VIO_ENET_CH3 == enEHIOResource)
				|| (EH_VIO_ENET_CH4 == enEHIOResource))
	{
		enSVCResult = ENE_enInitChannel(enEHIOResource, pstPortConfigCB);	
	}
#endif
	
	else if (EH_VIO_USB == enEHIOResource)
	{
		enSVCResult = VCOM_enInitBus(enEHIOResource, pstPortConfigCB);
	}

	return enSVCResult;
}

SYSAPI_tenSVCResult IO_enInitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB, bool boPWMMode)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enOK;
	uint32 u32MuxSel;
	uint32 u32Masters;
	uint32 u32MasterIDX = 0;
	bool boMasterOK = FALSE;
	SYSAPI_ttClientHandle tMasterClientHandle;
	
	IO_vClearMasterList();
	u32Masters = RESM_u32GetMasterEHIOResourceList(enEHIOResource, (IOAPI_tenEHIOResource*)&IO_aenEHIOMasterList);
	
#if defined(BUILD_PBL) || defined(BUILD_SBL) || defined (BUILD_KERNEL)
	while (u32Masters > u32MasterIDX)
	{
		tMasterClientHandle = RESM_u32GetMasterEHIOResource(IO_aenEHIOMasterList[u32MasterIDX]);
		if (OS_stSVCDataStruct.tClientHandle != tMasterClientHandle)
		{
			if (0 == tMasterClientHandle)
			{
				enSVCResult = SYSAPI_enMasterResourceUninitialised;
			}
			else
			{
				enSVCResult = SYSAPI_enMasterResourceMismatch;				
			}
			boMasterOK = FALSE;
			OS_stSVCDataStruct.pvData = (void*)&IO_aenEHIOMasterList;
		}
		u32MasterIDX++;
	}
		
	u32MuxSel = TEPM_u32InitTEPMChannel(enEHIOResource, pstTEPMChannelCB);
#endif

#ifdef BUILD_KERNEL_APP
	while (u32Masters > u32MasterIDX)
	{
		tMasterClientHandle = RESM_u32GetMasterEHIOResource(IO_aenEHIOMasterList[u32MasterIDX]);
		if (OS_stSVCDataStruct.tClientHandle != tMasterClientHandle)
		{
			if (0 == tMasterClientHandle)
			{
				enSVCResult = SYSAPI_enMasterResourceUninitialised;
			}
			else
			{
				enSVCResult = SYSAPI_enMasterResourceMismatch;				
			}
			boMasterOK = FALSE;
			OS_stSVCDataStruct.pvData = (void*)&IO_aenEHIOMasterList;
		}
		u32MasterIDX++;
	}
		
	u32MuxSel = TEPM_u32InitTEPMChannel(enEHIOResource, pstTEPMChannelCB, boPWMMode);
#endif	
	IO_vSetIOMux(enEHIOResource, IOAPI_enTEPM, u32MuxSel);
	
	/* TODO suppress warning */
	(void)boMasterOK;
	return enSVCResult;
}

void IO_vAssertDIOResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenTriState enTriState)
{
	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Assert DIO only if port bit non zero signifies DIO supported */
	{	
		PIM_vAssertPortBit(IO_rastEHPadResource[enIOResource].enPort, IO_rastEHPadResource[enIOResource].u32PortBit, enTriState);	
	}		
}

bool IO_boGetDIOResource(IOAPI_tenEHIOResource enIOResource)
{
	bool boPinState;

	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Assert DIO only if port bit non zero signifies DIO supported */
	{
		boPinState = PIM_boGetPortBitState(IO_rastEHPadResource[enIOResource].enPort, IO_rastEHPadResource[enIOResource].u32PortBit);
	}

	return boPinState;
}

IOAPI_tenTriState IO_enGetDIOResourceState(IOAPI_tenEHIOResource enIOResource)
{
	IOAPI_tenTriState enTriState = IOAPI_enError;
	bool boBitStateHigh;
	
	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Assert DIO only if port bit non zero signifies DIO supported */
	{	
		boBitStateHigh = PIM_boGetPortBitState(IO_rastEHPadResource[enIOResource].enPort, IO_rastEHPadResource[enIOResource].u32PortBit);	
		enTriState = TRUE == boBitStateHigh ? IOAPI_enHigh : IOAPI_enLow;
	}		
	
	return enTriState;
}

static void IO_vSetIOMux(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enIOType, uint32 u32MuxSel)
{
	uint32 u32PortBit = IO_rastEHPadResource[enIOResource].u32PortBit;
	PIMAPI_tenPort enPort = IO_rastEHPadResource[enIOResource].enPort;

	PIM_vSetPortMux(enPort, enIOType, u32PortBit, u32MuxSel);
}

static void IO_vClearMasterList(void)
{
	uint32 u32MasterIDX;
	
	for (u32MasterIDX = 0; u32MasterIDX < RESM_nMastersMax; u32MasterIDX++)
	{	
		IO_aenEHIOMasterList[u32MasterIDX] = EH_IO_Invalid;
	}
}
