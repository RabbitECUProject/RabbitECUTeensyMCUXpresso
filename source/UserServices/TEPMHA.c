/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      TEPM Hardware Abstraction                              */
/* DESCRIPTION:        Timed Event Programs module provides services for the  */
/*                     output of timed pulse-trains                           */
/* FILE NAME:          TEPMHA.c                                               */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _TEPMHA_C

#include "CPUAbstract.h"
#include <string.h>
#include "build.h"
#include "CEM.h"
#include "client.h"
#include "CPUAbstract.h"
#include "CQUEUE.h"
#include "declarations.h"
#include "stddef.h"
#include "types.h"
#include "IRQ.h"
#include "TEPM.h"
#include "TEPMHA.h"
#include "TEPMAPI.h"
#include "MSG.h"
#include "SIM.h"
#include "REGSET.h"
#include "RESM.h"
#include "IOAPI.h"
#include "SYSAPI.h"

//#pragma GCC optimize ("O0")
//#pragma optimize level=0 


/* Private data declarations
   -------------------------*/
const TEPM_tstTEPMChannel TEPMHA_rastTEPMChannel[] = TEPMHA_nChannelInfo;
const TEPM_tstTEPMReverseChannel TEPMHA_rastTEPMReverseChannel[] = TEPMHA_nChannelReverseInfo;
TEPMAPI_ttEventTime* TEPMHA_ptMasterClock;
uint32 TEPMHA_u32MissingRepeats;

#define TEPM_nTableCount sizeof(TEPMHA_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/

static void* TEPMHA_pvGetModule(IOAPI_tenEHIOResource);	


#ifdef BUILD_SAM3X8E
static uint32 TEPMHA_pstGetFTMChannel(IOAPI_tenEHIOResource);
static void TEPMHA_vSyncModules(void);
static bool TEPMHA_boModuleIsTimer(void*);
static bool TEPMHA_boModuleIsPWM(void*);
static IOAPI_tenEHIOResource TEPMHA_enGetParentResourceFromVIO(IOAPI_tenEHIOResource);
#endif //BUILD_SAM3X8E


static TEPMHA_tenTimerModule TEPMHA_enGetEnumFromVIO(IOAPI_tenEHIOResource);

/* Public function definitions
   ---------------------------*/	 

void TEPMHA_vInitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB)
{

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	vpuint32 vpuFTMReg;
	uint32 u32ControlWord = 0;
	tstTimerModule* pstTimerModule;
	uint32 u32ChannelIDX;
	uint32 u32TableIDX;
	pstTimerModule = TEPMHA_pvGetModule(enEHIOResource);
	u32ChannelIDX = TEPMHA_u32GetTimerHardwareChannel(enEHIOResource);
	vpuFTMReg = (vpuint32)((uint32)pstTimerModule + (uint32)offsetof(tstTimerModule, CONTROLS[u32ChannelIDX]));

	switch (pstTEPMChannelCB->enAction)
	{
		case TEPMAPI_enSetHigh:
		u32ControlWord |=
        (TIMERPREFIX(_CnSC_MSA_MASK) | TIMERPREFIX(_CnSC_ELSA_MASK) | TIMERPREFIX(_CnSC_ELSB_MASK)); break;
		case TEPMAPI_enSetLow:
		u32ControlWord |=
        (TIMERPREFIX(_CnSC_MSA_MASK) | TIMERPREFIX(_CnSC_ELSB_MASK)); break;
		case TEPMAPI_enToggle: u32ControlWord |=
        (TIMERPREFIX(_CnSC_MSA_MASK) | TIMERPREFIX(_CnSC_ELSB_MASK)); break;
		case TEPMAPI_enCapRising: u32ControlWord |=
		(TIMERPREFIX(_CnSC_ELSA_MASK)); break;
		case TEPMAPI_enCapFalling: u32ControlWord |=
		(TIMERPREFIX(_CnSC_ELSB_MASK)); break;
		case TEPMAPI_enCapAny:	u32ControlWord |=
		(TIMERPREFIX(_CnSC_ELSB_MASK) | TIMERPREFIX(_CnSC_ELSA_MASK)); break;
		case TEPMAPI_enNoAction: u32ControlWord |=
		(TIMERPREFIX(_CnSC_MSA_MASK)); break;
		default: break;
	}
		
	u32ControlWord = (TRUE == pstTEPMChannelCB->boInterruptEnable) ?
	u32ControlWord | TIMERPREFIX(_CnSC_CHIE_MASK) : u32ControlWord;

	*vpuFTMReg = u32ControlWord;

	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	if (TRUE == pstTEPMChannelCB->boInterruptEnable)
	{
		IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32TableIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
	}

#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	tstTimerModule* pstTimerModule;
	vpuint32 vpuFTMReg;
	uint32 u32ChannelIDX;
	uint32 u32ChannelSubIDX;
	uint32 u32ControlWord = 0;
	pwm_channel_t stPWMChannelCB;
	tstPWMModule* pstPWMModule;
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	if (TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType == TEPMHA_enCapCom)
	{
	    /* If this resource is associated with a timer module */
		pstTimerModule = (tstTimerModule*)TEPMHA_pvGetModule(enEHIOResource);

		switch (pstTEPMChannelCB->enAction)
		{
			case TEPMAPI_enSetHigh:
			{
				u32ControlWord = pstTimerModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_CMR;

				if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_ACPA_SET;
					u32ControlWord |= TC_CMR_WAVE;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
				}
				else
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_BCPB_SET;
					u32ControlWord |= TC_CMR_WAVE;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
				}
				break;
			}
			case TEPMAPI_enSetLow:
			{
				u32ControlWord = pstTimerModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_CMR;

				if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_WAVE; /* Waveform mode is enabled */
					u32ControlWord |= TC_CMR_ACPA_CLEAR;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				}
				else
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_WAVE; /* Waveform mode is enabled */
					u32ControlWord |= TC_CMR_BCPB_CLEAR;
					u32ControlWord |= TC_CMR_EEVT_XC0;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					//tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, TC_SR_CPBS);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				}
				break;
			}
			case TEPMAPI_enToggle: u32ControlWord = 0; break;
			case TEPMAPI_enCapRising:
			{
			    u32ControlWord = TC_CMR_LDRA_RISING | TC_CMR_LDRB_RISING | TC_CMR_TCCLKS_TIMER_CLOCK4;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, TC_SR_LDRAS | TC_SR_LDRBS);
				IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_3, TEPM_vInterruptHandler, NULL);
				TEPMHA_ptMasterClock = &(pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_CV);
				break;
			}
			case TEPMAPI_enCapFalling: u32ControlWord = 0; break;
			case TEPMAPI_enCapAny://matthew problem here
			{
				//u32ControlWord = TC_CMR_LDRA_RISING | TC_CMR_LDRB_FALLING | TC_CMR_TCCLKS_TIMER_CLOCK4;
			    u32ControlWord = TC_CMR_LDRA_EDGE | TC_CMR_LDRB_EDGE | TC_CMR_TCCLKS_TIMER_CLOCK4;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, TC_SR_LDRAS | TC_SR_LDRBS);
				IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_3, TEPM_vInterruptHandler, NULL);
				TEPMHA_ptMasterClock = &(pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_CV);
				break;
			}
			default:
			{
				break;
			}
			TEPMHA_vSyncModules();
		}
	}
	else if (TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType == TEPMHA_enPWM)
	{
	    /* If this resource is associated with a PWM module */
	    pstPWMModule = (tstPWMModule*)TEPMHA_pvGetModule(enEHIOResource);

		memset(&stPWMChannelCB, 0, sizeof(pwm_channel_t));

		stPWMChannelCB.channel = TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel;
		stPWMChannelCB.polarity = PWM_LOW;
		//stPWMChannelCB.alignment = PWM_ALIGN_CENTER;
		stPWMChannelCB.ul_prescaler = 0x0c;//pstTEPMChannelCB->enPreScalar;

        pwm_channel_init(pstPWMModule, &stPWMChannelCB);         
	}
#endif //BUILD_SAM3X8E
}


SYSAPI_tenSVCResult TEPMHA_vInitTEPMResource(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMResourceCB* pstTEPMResourceCB)
{
	tstTimerModule* pstTimerModule = NULL;
	REGSET_tstReg32Val astTEPMReg32Val[2];
	
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	switch (enEHIOResource)
	{
		case EH_VIO_TPM0:
		{
			pstTimerModule = TIMERPREFIX(0);
			break;
		}
		case EH_VIO_TPM1:
		{
			pstTimerModule = TIMERPREFIX(1);
			break;
		}
		case EH_VIO_TPM2:
		{
			pstTimerModule = TIMERPREFIX(2);
			break;
		}
#if defined(BUILD_MK60) || defined(BUILD_MK64)
		case EH_VIO_FTM3:
		{
			pstTimerModule = TIMERPREFIX(3);
			break;
		}
#endif //defined(BUILD_MK60) || defined(BUILD_MK64)
		default:
		{
			pstTimerModule = NULL;
			break;
		}
	}	

	TEPMHA_xRequestPortClock(pstTimerModule);

	astTEPMReg32Val[0].reg = (vpuint32)((uint32)pstTimerModule + offsetof(tstTimerModule, SC));
#if defined(BUILD_MK60) || defined(BUILD_MK64)
	astTEPMReg32Val[0].val = (uint32)(FTM_SC_PS(pstTEPMResourceCB->enPreScalar) | FTM_SC_CLKS(1) |
			 (pstTEPMResourceCB->enCountType << FTM_SC_CPWMS_SHIFT));
#endif //defined(BUILD_MK60) || defined(BUILD_MK64)

#if defined(BUILD_MKS20)
	astTEPMReg32Val[0].val = (uint32)(TPM_SC_PS(pstTEPMResourceCB->enPreScalar) | TPM_SC_CMOD(1) |
			 (pstTEPMResourceCB->enCountType << TPM_SC_CPWMS_SHIFT));
#endif //defined(BUILD_MKS20)

	astTEPMReg32Val[0].writeMode = REGSET_enOverwrite;	
	astTEPMReg32Val[1].reg = NULL;			
	REGSET_vInitReg32(&astTEPMReg32Val[0]);

#if defined(BUILD_MKS20)
	SIM_vSetReg32(SIM_SOPT2, 1 << SIM_SOPT2_TPMSRC_SHIFT);
#endif //defined(BUILD_MKS20)
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	tstPWMModule* pstPWMModule = NULL;
	pwm_clock_t stPWMClockConfig;

	switch (enEHIOResource)
	{
		case EH_VIO_TC0:
		{
		    pstTimerModule = TC0;
			break;
		}
		case EH_VIO_TC1:
		{
			pstTimerModule = TC1;
			break;
		}
		case EH_VIO_TC2:
		{
			pstTimerModule = TC2;
			break;
		}
		case EH_VIO_PWM:
		{
			pstPWMModule = PWM;
			break;
		}
		default:
		{
		     break;
		}
	}

	if (NULL != pstTimerModule)
	{
	    TEPMHA_xRequestTimerClock(pstTimerModule);
	}
	else if (NULL != pstPWMModule)
	{
		TEPMHA_xRequestPWMClock(pstPWMModule);

		stPWMClockConfig.ul_mck = SYS_FREQ_BUS;
		stPWMClockConfig.ul_clka = SYS_FREQ_BUS / pstTEPMResourceCB->enPreScalar;
		stPWMClockConfig.ul_clkb = SYS_FREQ_BUS / pstTEPMResourceCB->enPreScalar;
        pwm_init(pstPWMModule, &stPWMClockConfig);
		IRQ_vEnableIRQ(PWM_IRQn, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
	}
#endif //BUILD_MK60

	return SYSAPI_enOK;
}


void TEPMHA_vForceQueueTerminate(void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX)
{
	volatile TEPMAPI_ttEventTime tEventTimeScheduled;
	volatile sint32 tEventTimeRemains;
	uint32 u32Temp;
    tstTimerModule* pstTimerModule;
		
	CPU_xEnterCritical();

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
    pstTimerModule = (tstTimerModule*)pvModule;
	u32Temp = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC & ~TIMERPREFIX(_CnSC_CHIE_MASK);
	u32Temp |= TIMERPREFIX(_CnSC_CHF_MASK);
	
	tEventTimeRemains = (0xffff & pstTimerModule->CONTROLS[u32ChannelIDX].CnV) - 
											(0xffff & pstTimerModule->CNT);
	
	if ((0 < tEventTimeRemains) && (0x8000 > tEventTimeRemains))
	{	
		tEventTimeScheduled = pstTimerModule->CNT + 4;
		tEventTimeScheduled &= TEPMHA_nCounterMask;
		pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;			
		
		/* Force it now! */
		//while (pstTimerModule->CONTROLS[u32ChannelIDX].CnV != tEventTimeScheduled)
		//{
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;					
		//}

		/* Wait for it! Dangerous!!!*/
		//while (pstTimerModule->CONTROLS[u32ChannelIDX].CnV != pstTimerModule->CNT){}
	}
	else
	{
		pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
		//pstTimerModule->STATUS = 1 << u32ChannelIDX;
	}
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	volatile u32Stat;
	uint32 u32ControlWord;
    pstTimerModule = (tstTimerModule*)pvModule;
	sint32 s32ExitLoops = 100;

	if (0 == u32SubChannelIDX)
	{
		TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, u32SubChannelIDX, tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 10);
		
		tc_disable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPAS);

		u32Stat = 0;

	    tEventTimeRemains = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
		                    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		while (0 != (0x80000000 &  tEventTimeRemains))
		{
			tEventTimeRemains = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
								tc_read_cv(pstTimerModule, u32ChannelIDX / 2);
		}

	    while ((0 == (u32Stat & TC_SR_CPAS)) && (0 < s32ExitLoops)) 
	    {
		    u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);
			s32ExitLoops--;
	    }


		/*
		u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;
		u32ControlWord &= ~TC_CMR_ACPA_TOGGLE;
		u32ControlWord |= TC_CMR_ACPA_CLEAR;
		pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;

	    u32Stat = 0;
	    tEventTimeRemains = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
	    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		* If already past then force again *
		if ((UINT32_MAX / 2) < tEventTimeRemains)
	    {	
		    tEventTimeScheduled = tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 120;
			tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
	    }

	    * If is not too close then reschedule very close *
	    if (((UINT32_MAX / 2) > tEventTimeRemains) &&
	    (tEventTimeRemains > 50))
	    {
		    tEventTimeScheduled = tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 120;
		    tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
	    }

	    while (0 == (u32Stat & TC_SR_CPAS))
	    {
		    u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);

		    tEventTimeRemains = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
		    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		    if ((UINT32_MAX / 2) < tEventTimeRemains) break;
	    }*/
	}
	else
	{
		TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, u32SubChannelIDX, tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 10);		
		
		tc_disable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPBS);
		
		u32Stat = 0;
	    tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
		                    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);
		
		while (0 == (0x80000000 &  tEventTimeRemains))
		{
			tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
								tc_read_cv(pstTimerModule, u32ChannelIDX / 2);
		}

	    while ((0 == (u32Stat & TC_SR_CPBS)) && (0 < s32ExitLoops)) 
	    {
		    u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);
			s32ExitLoops--;
	    }


		/*
	    u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;
		u32ControlWord &= ~TC_CMR_BCPB_TOGGLE;
		u32ControlWord |= TC_CMR_BCPB_CLEAR;
		pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;

	    u32Stat = 0;
	    tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
		                    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		* If already past then force again *
		if ((UINT32_MAX / 2) < tEventTimeRemains)
	    {	
		    tEventTimeScheduled = tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 120;
			tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
	    }

	    * If is not too close then reschedule very close *
		if (((UINT32_MAX / 2) > tEventTimeRemains) && 
		     (tEventTimeRemains > 50))
	    {	
		    tEventTimeScheduled = tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 120;
			tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
	    }

		while (0 == (u32Stat & TC_SR_CPBS))
		{
			u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);

			tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
			tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

			if ((UINT32_MAX / 2) < tEventTimeRemains) break;
		}*/
	}
#endif //BUILD_SAM3X8E
	
	CPU_xExitCritical();	
}

bool TEPMHA_boCheckFalseAlarm(void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX)
{
    bool boFalseAlarm = FALSE;


#ifdef BUILD_SAM3X8E
 	uint32 u32Delay;
    tstTimerModule* pstTimerModule = (tstTimerModule*)pvModule;

	if (0 == u32SubChannelIDX)
	{
		u32Delay = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
	    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		if ((UINT32_MAX / 2) > u32Delay)
		{
			boFalseAlarm = TRUE;
		}
	}
	else
	{
		u32Delay = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
	    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		if ((UINT32_MAX / 2) > u32Delay)
		{
			boFalseAlarm = TRUE;
		}
	}
#endif

	return boFalseAlarm;
}


void TEPMHA_vCapComAction(TEPMAPI_tenAction enAction, void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX, TEPMAPI_ttEventTime tEventTimeScheduled)
{
	uint32 u32Temp;
	tstTimerModule* pstTimerModule;

	switch (enAction)
	{		
		case TEPMAPI_enSetHigh:
		{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)

#endif //defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)

#if defined(BUILD_MKS20)
		    pstTimerModule = (tstTimerModule*)pvModule;

			/* Disable to change edge mode */
		    pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = 0;
			pstTimerModule->STATUS = 1 << u32ChannelIDX;

			u32Temp = pstTimerModule->CNT;
			u32Temp += 2;
			u32Temp &= TEPMHA_nCounterMask;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = u32Temp;

			/* Wait for the action */
			while (u32Temp != pstTimerModule->CNT){}

			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;

			/* Set to toggle with interrupts */
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC =
					TIMERPREFIX(_CnSC_MSA_MASK) | TIMERPREFIX(_CnSC_ELSA_MASK) | TIMERPREFIX(_CnSC_CHIE_MASK);
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
			uint32 u32ControlWord;

            if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	        {
			    pstTimerModule = (tstTimerModule*)pvModule;

				u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;

				if (0 == u32SubChannelIDX)
				{
					tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
					u32ControlWord &= ~TC_CMR_ACPA_CLEAR;
					u32ControlWord |= TC_CMR_ACPA_SET;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPAS);
					//tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				}
				else if (1 == u32SubChannelIDX)
				{
					tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
					u32ControlWord &= ~TC_CMR_BCPB_CLEAR;
					u32ControlWord |= TC_CMR_BCPB_SET;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPBS);
					//tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				}  
			}          
			else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
			{
				tstPWMModule* pstPWMModule;
			    TEPMAPI_ttEventTime tEventTime;
				uint32 u32Temp;
				pwm_channel_t stPWMChannel;

			    pstPWMModule = (tstPWMModule*)pvModule;
				
				//u32Temp = pwm_channel_get_status(pstPWMModule);
				
				tEventTime = tEventTimeScheduled - *TEPMHA_ptMasterClock;

				memset(&stPWMChannel, 0, sizeof(pwm_channel_t));
				stPWMChannel.channel = u32ChannelIDX;

			    pwm_channel_enable(pstPWMModule,  u32ChannelIDX);

				//pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CPRD = pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CCNT + 2;

				while (pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CCNT > 5)
				{
				    pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CDTY = 1;
				    pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CPRD = 10;
				}


				pwm_channel_update_period(pstPWMModule, &stPWMChannel, 6000u);
				pwm_channel_update_duty(pstPWMModule, &stPWMChannel, 4000u);
				//pwm_channel_update_period(pstPWMModule, &stPWMChannel, 5000u);
				//pwm_channel_update_duty(pstPWMModule, &stPWMChannel, 2000u);

				//pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CDTY = 5;
				//pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CPRD = 10;
				//pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CPRD = 5000;
				//pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CDTY = 1000;

				//if (0 == ((1 << u32ChannelIDX) & u32Temp))
				{

				}
			}
#endif //BUILD_SAM3X8E
            break;
		}
		case TEPMAPI_enSetLow:
		{	
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)

#endif //BUILD_MK6X

#if defined(BUILD_MKS20)

#endif //BUILD_MKS20


#ifdef BUILD_SAM3X8E
            if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	        {
			    pstTimerModule = (tstTimerModule*)pvModule;

				u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;

				if (0 == u32SubChannelIDX)
				{
					u32ControlWord &= ~TC_CMR_ACPA_SET;
					u32ControlWord |= TC_CMR_ACPA_CLEAR;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPAS);
					tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				}
				else if (1 == u32SubChannelIDX)
				{
					u32ControlWord &= ~TC_CMR_BCPB_SET;
					u32ControlWord |= TC_CMR_BCPB_CLEAR;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPBS);
					tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				} 
			}      
			else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
			{

			}  
#endif //BUILD_SAM3X8E	
            break;	
		}			
		case TEPMAPI_enToggle:	
		{		
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
		    pstTimerModule = (tstTimerModule*)pvModule;
		    u32Temp = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC & ~TIMERPREFIX(_CnSC_ELSB_MASK);
		    u32Temp |= (TIMERPREFIX(_CnSC_ELSA_MASK) | TIMERPREFIX(_CnSC_CHIE_MASK));
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;
#endif //BUILD_MK6X

#if 0
		    pstTimerModule = (tstTimerModule*)pvModule;
			pstTimerModule->STATUS = 1 << u32ChannelIDX;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;
#endif //BUILD_MKS20
            break;
		}
		case TEPMAPI_enEndProgram:			
		{		
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)
		    pstTimerModule = (tstTimerModule*)pvModule;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;				
#endif //BUILD_MK6X
            break;
		}
		case TEPMAPI_enImmediatePulse:
		{
#if defined(BUILD_MKS20)
		    pstTimerModule = (tstTimerModule*)pvModule;

			/* Disable to change edge mode */
		    pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = 0;
			pstTimerModule->STATUS = 1 << u32ChannelIDX;

			u32Temp = pstTimerModule->CNT;
			u32Temp += 2;
			u32Temp &= TEPMHA_nCounterMask;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = u32Temp;

			/* Wait for the action */
			while (u32Temp != pstTimerModule->CNT){}

			/* Set to toggle with interrupts */
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC =
					TIMERPREFIX(_CnSC_MSA_MASK) | TIMERPREFIX(_CnSC_ELSA_MASK) | TIMERPREFIX(_CnSC_CHIE_MASK);

			u32Temp = pstTimerModule->CNT;
			u32Temp += 2;
			u32Temp &= TEPMHA_nCounterMask;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = u32Temp;

			/* Wait for the action */
			while (u32Temp != pstTimerModule->CNT){}

			pstTimerModule->STATUS = 1 << u32ChannelIDX;

			tEventTimeScheduled += CEM_tSyncTimeLast;
			tEventTimeScheduled &= TEPMHA_nCounterMask;

			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;
#endif //defined(BUILD_MKS20)
			break;
		}

		default:
		{
			break;
		}
	}				
}


static void* TEPMHA_pvGetModule(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	void* pvModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPMHA_pvGetModuleFromEnum(TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule);
	
	return pvModule;
}


void* TEPMHA_pvGetModuleFromEnum(TEPMHA_tenTimerModule enTimerModule)
{
	void* pvModule;
	
#ifdef BUILD_MK60
	switch (enTimerModule)
	{
		case FTM_enTPM0: pvModule = (tstTimerModule*)TPM0; break;
		case FTM_enTPM1: pvModule = (tstTimerModule*)TPM1; break;
		case FTM_enTPM2: pvModule = (tstTimerModule*)TPM2; break;
		case FTM_enFTM3: pvModule = (tstTimerModule*)FTM3; break;
		default: pvModule = NULL; break;
	}
#endif //BUILD_MK60

#ifdef BUILD_MK64
	switch (enTimerModule)
	{
		case FTM_enTPM0: pvModule = (tstTimerModule*)TPM0; break;
		case FTM_enTPM1: pvModule = (tstTimerModule*)TPM1; break;
		case FTM_enTPM2: pvModule = (tstTimerModule*)TPM2; break;
		case FTM_enFTM3: pvModule = (tstTimerModule*)FTM3; break;
		default: pvModule = NULL; break;
	}
#endif //BUILD_MK64

#ifdef BUILD_MKS20
	switch (enTimerModule)
	{
		case TPM_enTPM0: pvModule = (tstTimerModule*)TPM0; break;
		case TPM_enTPM1: pvModule = (tstTimerModule*)TPM1; break;
		case TPM_enTPM2: pvModule = (tstTimerModule*)TPM2; break;
		default: pvModule = NULL; break;
	}
#endif //BUILD_MKS20

#ifdef BUILD_SAM3X8E
	switch (enTimerModule)
	{
		case TEPMHA_enTC0: pvModule = (void*)TC0; break;
		case TEPMHA_enTC1: pvModule = (void*)TC1; break;
		case TEPMHA_enTC2: pvModule = (void*)TC2; break;
		case TEPMHA_enPWM0: pvModule = (void*)PWM; break;
		default: pvModule = NULL; break;
	}
#endif //BUILD_SAMX3X8E
	
	return pvModule;
}

uint32 TEPMHA_u32GetFTMTableIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32TableIDX = 0;
	
#ifdef BUILD_MK60
	if ((EH_IO_TMR16 >= enEHIOResource) && (EH_IO_TMR1 <= enEHIOResource))
	{
		u32ChannelIDX = enEHIOResource - EH_IO_TMR1;
	}
	else
	{
		switch (enEHIOResource)
		{
			case EH_IO_ADSE4: u32ChannelIDX = 16; break;
			case EH_IO_GPSE9: u32ChannelIDX = 16; break;
			case EH_IO_GPSE8: u32ChannelIDX = 16; break;
			case EH_IO_ADSE5: u32ChannelIDX = 16; break;
			case EH_IO_GPSE7: u32ChannelIDX = 16; break;
			case EH_IO_CAN1T: u32ChannelIDX = 16; break;
			case EH_IO_CAN1R: u32ChannelIDX = 16; break;
		}
	}
#endif //BUILD_MK60

#ifdef BUILD_MK64
	if ((EH_IO_TMR18 >= enEHIOResource) && (EH_IO_TMR1 <= enEHIOResource))
	{
		u32TableIDX = enEHIOResource - EH_IO_TMR1;
	}
	else
	{
		u32TableIDX = ~0;
	}
#endif //BUILD_MK64

#ifdef BUILD_MKS20
	if ((EH_IO_TMR10 >= enEHIOResource) && (EH_IO_TMR1 <= enEHIOResource))
	{
		u32TableIDX = enEHIOResource - EH_IO_TMR1;
	}
	else
	{
		u32TableIDX = ~0;
	}
#endif //BUILD_MKS20

#ifdef BUILD_SAM3X8E
	if ((EH_IO_TMR12 >= enEHIOResource) && (EH_IO_TMR1 <= enEHIOResource))
	{
		u32ChannelIDX = enEHIOResource - EH_IO_TMR1;
	}
	else if (EH_IO_ADD5 == enEHIOResource)
	{
	    u32ChannelIDX = 12;
	}
	else if (EH_IO_ADD7 == enEHIOResource)
	{
		u32ChannelIDX = 13;
	}
	else if (EH_IO_ADD8 == enEHIOResource)
	{
		u32ChannelIDX = 14;
	}
	else if (EH_IO_EXTINT == enEHIOResource)
	{
		u32ChannelIDX = 15;
	}
#endif //BUILD_SAM3X8E
	
	return u32TableIDX;
}



bool TEPMHA_boFlagIsSet(void* pvModule, uint32 u32ChannelIDX, puint32 pu32Flags, uint32 u32Sequence, uint32 u32Prio)
{
	bool boFlagIsSet = false;
	tstTimerModule* pstTimerModule;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
    pstTimerModule = (tstTimerModule*)pvModule;
	boFlagIsSet = (TIMERPREFIX(_CnSC_CHF_MASK) == (TIMERPREFIX(_CnSC_CHF_MASK) & pstTimerModule->CONTROLS[u32ChannelIDX].CnSC));
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
    static uint32 u32Seq[2];
    static uint32 u32FlagsCache[9];   /* Once the flags are read they are cleared so cache them */
	uint32 u32CacheIndex = 0;
	uint32 u32CMR;
	bool boWaveMode;
	tstPWMModule* pstPWMModule;

	if ((TC0 == (tstTimerModule*)pvModule) ||
	    (TC1 == (tstTimerModule*)pvModule) ||
		(TC2 == (tstTimerModule*)pvModule))
	{
	    pstTimerModule = (tstTimerModule*)pvModule;

	    switch ((uint32)pstTimerModule)
		{
			case (uint32)TC0: u32CacheIndex = (u32ChannelIDX / 2); break;
			case (uint32)TC1: u32CacheIndex = 3 + (u32ChannelIDX / 2); break;
			case (uint32)TC2: u32CacheIndex = 6 + (u32ChannelIDX / 2); break;
		}

		if (u32Seq[u32Prio] != u32Sequence)
		{
			u32FlagsCache[u32CacheIndex] = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_SR;
			u32Seq[u32Prio] = u32Sequence;
		}

		u32CMR = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;

		if (0 == u32FlagsCache[u32CacheIndex])
		{
			u32FlagsCache[u32CacheIndex] = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_SR;
		}

		*pu32Flags = u32FlagsCache[u32CacheIndex];
		boWaveMode = TC_CMR_WAVE == (u32CMR & TC_CMR_WAVE) ? TRUE : FALSE;

		if (0 == (u32ChannelIDX & 0x1))
		{
			/* Is channel A? */
			if (FALSE == boWaveMode)
			{
				boFlagIsSet = 0 != (*pu32Flags & (TC_SR_LDRAS | TC_SR_LDRBS)) ?
				TRUE : FALSE;
			}
			else
			{
				boFlagIsSet = 0 != (*pu32Flags & TC_IMR_CPAS) ? TRUE : FALSE;
			}
		}
		else
		{
			/* Is channel B? */
			if (FALSE == boWaveMode)
			{
				/* There is no input capture on the physical B channel */
				boFlagIsSet = FALSE;
			}
			else
			{
				boFlagIsSet = 0 != (*pu32Flags & TC_IMR_CPBS) ? TRUE : FALSE;
			}
		}
	}
	else if (PWM == (tstPWMModule*)pvModule)
	{
	    boFlagIsSet = TRUE;
	}	
#endif //BUILD_SAM3X8E

    return boFlagIsSet;
}

bool TEMPHA_boInterruptEnabled(void* pvModule, uint32 u32ChannelIDX)
{
    bool boEnableSet = false;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
	boEnableSet = (TIMERPREFIX(_CnSC_CHIE_MASK) == (TIMERPREFIX(_CnSC_CHIE_MASK) & ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC));
#endif

#ifdef BUILD_SAM3X8E    
    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	{
	    tstTimerModule* pstModule = (tstTimerModule*)pvModule;
		if (0 == (u32ChannelIDX & 0x01))
		{
			boEnableSet = 0 != (pstModule->TC_CHANNEL[u32ChannelIDX / 2].TC_IMR & (TC_IMR_LDRAS | TC_IMR_CPAS)) ?
			TRUE : FALSE;
		}
		else
		{
			boEnableSet = 0 != (pstModule->TC_CHANNEL[u32ChannelIDX / 2].TC_IMR & (TC_IMR_LDRBS | TC_IMR_CPBS)) ?
			TRUE : FALSE;
		}	
	}
	else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
	{

	}
#endif //BUILD_SAM3X8E

    return boEnableSet;
}

TEPMAPI_ttEventTime TEPMHA_tGetScheduledVal(void* pvModule, uint32 u32ChannelIDX, bool boInputMode, uint32 u32Flags)
{
    TEPMAPI_ttEventTime tEventTime = 0;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
    tEventTime = ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnV;
#endif

#ifdef BUILD_SAM3X8E
    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	{
		if (TRUE == boInputMode)
		{
			if (0 != (u32Flags & TC_SR_LDRAS))
			{
				tEventTime = tc_read_ra((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
			if (0 != (u32Flags & TC_SR_LDRBS))
			{
				tEventTime = tc_read_rb((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
		}
		else
		{
			if (0 == (u32ChannelIDX & 0x01))
			{
				tEventTime = tc_read_ra((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
			else
			{
				tEventTime = tc_read_rb((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
		}	
	}
    else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
    {

	}
#endif //BUILD_SAM3X8E


    return tEventTime;
}

IOAPI_tenTriState TEPMHA_enGetTimerDigitalState(IOAPI_tenEHIOResource enEHIOResource)
{
    IOAPI_tenTriState enTriState = IOAPI_enError;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(MK10) || defined(MKS20)
	enTriState = IO_enGetDIOResourceState(enEHIOResource);
#endif

#ifdef BUILD_SAM3X8E
	uint32 u32ChannelIDX;
	void* pvModule;
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPMHA_pvGetModuleFromEnum(TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule);

    if (((EH_IO_TMR1 <= enEHIOResource) && (EH_IO_TMR12 >= enEHIOResource)) ||
	(EH_IO_ADD5 == enEHIOResource) ||
	(EH_IO_EXTINT == enEHIOResource) ||
	(EH_IO_ADD6 == enEHIOResource) ||
	(EH_IO_ADD7 == enEHIOResource))
	{
	    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
		{
		    tstTimerModule* pstModule = (tstTimerModule*)pvModule;
			if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
			{
				enTriState = 0 != (pstModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_SR & TC_SR_MTIOA) ?
					IOAPI_enHigh : IOAPI_enLow;
			}
			else
			{
				enTriState = 0 != (pstModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_SR & TC_SR_MTIOB) ?
				    IOAPI_enHigh : IOAPI_enLow;
			}
		}
	}
#endif

    return enTriState;
}

void TEMPHA_vResetTimerFlag(void* pvModule, uint32 u32ChannelIDX)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
    ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC |= TIMERPREFIX(_CnSC_CHF_MASK);
#endif //defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)

#if defined(BUILD_MKS20)
    ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC |= TIMERPREFIX(_CnSC_CHF_MASK);
#endif //defined(BUILD_MKS20)
}


uint32 TEPMHA_u32GetFreeVal(void* pvModule, uint32 u32ChannelIDX)
{
    uint32 u32FreeVal;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
	u32FreeVal = ((tstTimerModule*)pvModule)->CNT;
#endif

#ifdef BUILD_SAM3X8E
    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	{
	    u32FreeVal = ((tstTimerModule*)pvModule)->TC_CHANNEL[u32ChannelIDX / 2].TC_CV;
	}
#endif

	return u32FreeVal;
}

void TEPMHA_vDisconnectEnable(void* pvModule, uint32 u32ChannelIDX, bool interrupt, bool HW)
{
	uint32 u32Temp;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)
	u32Temp = ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC;
	u32Temp &= ~TIMERPREFIX(_CnSC_CHIE_MASK);
	((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
#endif //BUILD_MK6X

#if defined(BUILD_MKS20)
	u32Temp = ((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC;

	if (interrupt)
	{
		u32Temp &= ~TIMERPREFIX(_CnSC_CHIE_MASK);
	}

	if (HW)
	{
		u32Temp = 0;
		//u32Temp &= ~TIMERPREFIX(_CnSC_ELSA_MASK);
		//u32Temp &= ~TIMERPREFIX(_CnSC_ELSB_MASK);
	}

	((tstTimerModule*)pvModule)->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
#endif //BUILD_MK6X
}

TEPMAPI_ttEventTime TEPMHA_u32GetTimerVal(IOAPI_tenEHIOResource enEHIOResource)
{
	tstTimerModule* pstTimerModule;
	TEPMAPI_ttEventTime tEventTime = 0;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
	uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pstTimerModule = (tstTimerModule*)TEPMHA_pvGetModuleFromEnum(TEPMHA_rastTEPMChannel[u32TableIDX].enModule);
	tEventTime = pstTimerModule->CONTROLS[TEPMHA_rastTEPMChannel[u32TableIDX].u32Channel].CnSC;
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	pstTimerModule = TEPMHA_pvGetTimerModuleFromVIO(enEHIOResource);
	tEventTime = pstTimerModule->TC_CHANNEL[0].TC_CV;
#endif //BUILD_SAM3X8E

	return tEventTime;
}

void* TEPMHA_pvGetTimerModuleFromVIO(IOAPI_tenEHIOResource enEHIOResource)
{
	void* pvModule;
	
	switch(enEHIOResource)
	{
#if defined(BUILD_MK60) || defined(BUILD_MK64)
		case EH_VIO_FTM0: pvModule = (tstTimerModule*)FTM0; break;
		case EH_VIO_FTM1: pvModule = (tstTimerModule*)FTM1; break;
		case EH_VIO_FTM2: pvModule = (tstTimerModule*)FTM2; break;
		case EH_VIO_FTM3: pvModule = (tstTimerModule*)FTM3; break;
#endif //BUILD_MK60X

#if defined(BUILD_MKS20)
		case EH_VIO_TPM0: pvModule = (tstTimerModule*)TPM0; break;
		case EH_VIO_TPM1: pvModule = (tstTimerModule*)TPM1; break;
		case EH_VIO_TPM2: pvModule = (tstTimerModule*)TPM2; break;
#endif //BUILD_MKS20

#ifdef BUILD_SAM3X8E
        case EH_VIO_TC0: pvModule = (tstTimerModule*)TC0; break;	
        case EH_VIO_TC1: pvModule = (tstTimerModule*)TC1; break;
        case EH_VIO_TC2: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_TCC0: pvModule = (tstTimerModule*)TC0; break;
		case EH_VIO_TCC1: pvModule = (tstTimerModule*)TC0; break;
		case EH_VIO_TCC2: pvModule = (tstTimerModule*)TC0; break;
		case EH_VIO_TCC3: pvModule = (tstTimerModule*)TC1; break;
		case EH_VIO_TCC4: pvModule = (tstTimerModule*)TC1; break;
		case EH_VIO_TCC5: pvModule = (tstTimerModule*)TC1; break;
		case EH_VIO_TCC6: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_TCC7: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_TCC8: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_PWM: pvModule = (tstPWMModule*)PWM; break;
#endif
		default: pvModule = NULL; break;
	}
	
	return pvModule;
}

#ifdef BUILD_SAM3X8E
static uint32 TEPMHA_pstGetFTMChannel(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	return TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel;
}
#endif //BUILD_SAM3X8E

uint32 TEPMHA_u32GetTimerChannelsPerInterruptGroup(void* pvModule)
{
    uint32 u32ChannelCount = 0;
#if defined(BUILD_MK60)
    u32ChannelCount = 8;
#endif //BUILD_MK60

#if defined(BUILD_MK64)
    	if (FTM0 == pvModule) u32ChannelCount = 8;
    	if (FTM1 == pvModule) u32ChannelCount = 8;
    	if (FTM2 == pvModule) u32ChannelCount = 2;
    	if (FTM3 == pvModule) u32ChannelCount = 8;
#endif //BUILD_MK64

#if defined(BUILD_MKS20)
    	if (TPM0 == pvModule) u32ChannelCount = 6;
    	if (TPM1 == pvModule) u32ChannelCount = 2;
    	if (TPM2 == pvModule) u32ChannelCount = 2;
#endif //BUILD_MKS20

#ifdef BUILD_SAM3X8E
    u32ChannelCount = 2;
#endif //BUILD_SAM3X8E
	
	return u32ChannelCount;
}

IOAPI_tenEHIOResource TEPMHA_enGetTimerResourceFromVIOAndIndex(IOAPI_tenEHIOResource enEHIOResource, uint32 u32Channel)
{
	IOAPI_tenEHIOResource enChannelEHIOResource = EH_IO_Invalid;
	uint32 u32Temp;

#if defined(BUILD_MK60)
    TEPMHA_tenTimerModule enModule = TEPMHA_enGetEnumFromVIO(enEHIOResource);

	for (u32ChannelIDX = 0; u32ChannelIDX < TEPMHA_nEventChannels; u32ChannelIDX++)
	{
	    if ((enModule == TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule) &&
		    u32Channel == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel &&
            (TEPMHA_enCapCom == TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType))
		{
		    enChannelEHIOResource = TEPMHA_rastTEPMChannel[u32ChannelIDX].enEHIOResource;
		    break;
		}
	}
#endif


#if defined (BUILD_MK64)
	u32Temp = 8 * TEPMHA_enGetEnumFromVIO(enEHIOResource) + u32Channel;
	enChannelEHIOResource = TEPMHA_rastTEPMReverseChannel[u32Temp].enEHIOResource;
#endif

#if defined (BUILD_MKS20)
	u32Temp = 8 * TEPMHA_enGetEnumFromVIO(enEHIOResource) + u32Channel;
	enChannelEHIOResource = TEPMHA_rastTEPMReverseChannel[u32Temp].enEHIOResource;
#endif


#ifdef BUILD_SAM3X8E
    uint32 u32ChannelIDX;
    TEPMHA_tenTimerModule enModule = TEPMHA_enGetEnumFromVIO(enEHIOResource);

	for (u32ChannelIDX = 0; u32ChannelIDX < TEPMHA_nEventChannels; u32ChannelIDX++)
	{   
	    if ((enModule == TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule) &&
		    (u32Channel == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel + TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel) &&
            (TEPMHA_enCapCom == TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType))
		{
		    enChannelEHIOResource = TEPMHA_rastTEPMChannel[u32ChannelIDX].enEHIOResource;
		    break;
		}
	}
#endif //BUILD_SAM3X8E
	return enChannelEHIOResource;
}


#ifdef BUILD_SAM3X8E
static IOAPI_tenEHIOResource TEPMHA_enGetParentResourceFromVIO(IOAPI_tenEHIOResource enEHIOResource)
{
    IOAPI_tenEHIOResource enParentVIOResource = EH_IO_Invalid;

	switch (enEHIOResource)
	{
		case EH_VIO_TCC0: enParentVIOResource = EH_VIO_TC0; break;
		case EH_VIO_TCC1: enParentVIOResource = EH_VIO_TC0; break;
		case EH_VIO_TCC2: enParentVIOResource = EH_VIO_TC0; break;
		case EH_VIO_TCC3: enParentVIOResource = EH_VIO_TC1; break;
		case EH_VIO_TCC4: enParentVIOResource = EH_VIO_TC1; break;
		case EH_VIO_TCC5: enParentVIOResource = EH_VIO_TC1; break;
		case EH_VIO_TCC6: enParentVIOResource = EH_VIO_TC2; break;
		case EH_VIO_TCC7: enParentVIOResource = EH_VIO_TC2; break;
		case EH_VIO_TCC8: enParentVIOResource = EH_VIO_TC2; break;
		default : enParentVIOResource = EH_IO_Invalid; break;
	}

    return enParentVIOResource;
}
#endif //BUILD_SAM3X8E

uint32 TEPMHA_u32GetTimerStartChannelInterruptGroup(IOAPI_tenEHIOResource enEHIOResource)
{
    uint32 u32StartChannel;

#ifdef BUILD_SAM3X8E
    switch (enEHIOResource)
	{
	    case EH_VIO_TCC0: u32StartChannel = 0; break;
	    case EH_VIO_TCC1: u32StartChannel = 2; break;
	    case EH_VIO_TCC2: u32StartChannel = 4; break;
	    case EH_VIO_TCC3: u32StartChannel = 0; break;
	    case EH_VIO_TCC4: u32StartChannel = 2; break;
	    case EH_VIO_TCC5: u32StartChannel = 4; break;
	    case EH_VIO_TCC6: u32StartChannel = 0; break;
	    case EH_VIO_TCC7: u32StartChannel = 2; break;
	    case EH_VIO_TCC8: u32StartChannel = 4; break;
		default : u32StartChannel = 0; break;
	}
#endif //BUILD_SAM3X8E

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	u32StartChannel = 0;
#endif

    return u32StartChannel;
}

uint32 TEPMHA_u32GetTimerHardwareSubChannel(uint32 u32TableIndex)
{
    uint32 u32ChannelSubIDX = 0;

#ifdef BUILD_SAM3X8E
	u32ChannelSubIDX = TEPMHA_rastTEPMChannel[u32TableIndex].u32SubChannel;
#endif //BUILD_SAM3X8E

    return u32ChannelSubIDX;
}

uint32 TEPMHA_u32GetTimerHardwareChannel(IOAPI_tenEHIOResource enEHIOResource)
{
#if defined(BUILD_MK64) || defined(BUILD_MK60) || defined(BUILD_MKS20)
    uint32 u32ChannelIDX = 0;
    uint32 u32TableIDX = 0;

	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	u32ChannelIDX = TEPMHA_rastTEPMChannel[u32TableIDX].u32Channel;

	return u32ChannelIDX;
#endif //BUILD_MK6X
}


TEPMHA_tenTimerModule TEPMHA_enTimerEnumFromModule(tstTimerModule* pstTimerModule)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10)
	TEPMHA_tenTimerModule enTimerModule = TEPMHA_enTPM0;

	if (FTM1 == pstTimerModule) enTimerModule = TEPMHA_enFTM1;
	if (FTM2 == pstTimerModule) enTimerModule = TEPMHA_enFTM2;
	if (FTM3 == pstTimerModule) enTimerModule = TEPMHA_enFTM3;
#endif

#if defined(BUILD_MKS20)
	TEPMHA_tenTimerModule enTimerModule = TEPMHA_enTPM0;

	if (TPM0 == pstTimerModule) enTimerModule = TEPMHA_enTPM0;
	if (TPM1 == pstTimerModule) enTimerModule = TEPMHA_enTPM1;
	if (TPM2 == pstTimerModule) enTimerModule = TEPMHA_enTPM2;
#endif

#ifdef BUILD_SAM3X8E
	TEPMHA_tenTimerModule enTimerModule = TEPMHA_enTC0;

    if (TC0 == pstTimerModule)
	{
		enTimerModule = TEPMHA_enTC0;
	}
    else if (TC1 == pstTimerModule)
    {
		enTimerModule = TEPMHA_enTC1;
	}
    else if (TC2 == pstTimerModule)
    {
		enTimerModule = TEPMHA_enTC2;
	}
#endif //BUILD_SAM3X8E

	return enTimerModule;
}


#ifdef BUILD_SAM3X8E
static void TEPMHA_vSyncModules(void)
{
	tc_sync_trigger((tstTimerModule*)TC0);
	tc_sync_trigger((tstTimerModule*)TC1);
	tc_sync_trigger((tstTimerModule*)TC2);
}
#endif //BUILD_SAM3X8E


#ifdef BUILD_SAM3X8E
static bool TEPMHA_boModuleIsTimer(void* pvModule)
{
    bool boRetVal = FALSE;


    if ((TC0 == (tstTimerModule*)pvModule) ||
        (TC1 == (tstTimerModule*)pvModule) ||
        (TC2 == (tstTimerModule*)pvModule))
    {
	    boRetVal = TRUE;
    }

    return boRetVal;
}
#endif //BUILD_SAM3X8E

static TEPMHA_tenTimerModule TEPMHA_enGetEnumFromVIO(IOAPI_tenEHIOResource enEHIOResource)
{
    TEPMHA_tenTimerModule enModule;
#ifdef BUILD_SAM3X8E
    switch (enEHIOResource)
	{
	    case EH_VIO_TC0:
		case EH_VIO_TCC0:
		case EH_VIO_TCC1:
		case EH_VIO_TCC2:
		{
		    enModule = TEPMHA_enTC0;
			break;
		}
	    case EH_VIO_TC1:
	    case EH_VIO_TCC3:
	    case EH_VIO_TCC4:
	    case EH_VIO_TCC5:
	    {
		    enModule = TEPMHA_enTC1;
		    break;
	    }
	    case EH_VIO_TC2:
	    case EH_VIO_TCC6:
	    case EH_VIO_TCC7:
	    case EH_VIO_TCC8:
	    {
		    enModule = TEPMHA_enTC2;
		    break;
	    }
		case EH_VIO_PWM:
		{

		    enModule = TEPMHA_enPWM0;
			break;
		}
		default:
		{
		    enModule = TEPMHA_enModuleInvalid;
			break;		 
		}
	}
#endif //BUILD_SAM3X8E

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
    enModule = (uint32)(enEHIOResource - EH_VIO_TPM0);
#endif //BUILD_MK6X

    return enModule;
}
#ifdef BUILD_SAM3X8E
static bool TEPMHA_boModuleIsPWM(void* pvModule)
{
    bool boRetVal = FALSE;


    if (PWM == (tstPWMModule*)pvModule)
    {
	    boRetVal = TRUE;
    }
    return boRetVal;
}
#endif //BUILD_SAM3X8E
uint32 TEPMHA_u32GetModulePhaseCorrect(TEPMHA_tenTimerModule enTimerModule, uint32 u32ChannelIDX)
{
  uint32 u32PhaseCorrect = 0;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MK10) || defined(BUILD_MKS20)
  switch (enTimerModule)
  {
  	  case TEPMHA_enTPM0:
  	  {
  		  u32PhaseCorrect = TPM0->CNT - TPM0->CNT;
  		  break;
  	  }
  	  case TEPMHA_enTPM1:
  	  {
  		  u32PhaseCorrect = TPM1->CNT - TPM0->CNT;
  		  break;
  	  }
  	  case TEPMHA_enTPM2:
  	  {
  		  u32PhaseCorrect = TPM2->CNT - TPM0->CNT;
  		  break;
  	  }
  	  default:
  	  {
  		  u32PhaseCorrect = 0;
  		  break;
  	  }
  }
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
  (uint32)CEM_ttGetModulePhase(3 * enTimerEnumModule + u32ChannelIDX / 2);
#endif

  return u32PhaseCorrect;
}

void TEPMHA_vConfigureMissingToothInterrupt(void)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	vpuint32 vpuFTMReg;
	uint32 u32ControlWord;
	tstTimerModule* pstTimerModule = TPM0;
	vpuFTMReg = (vpuint32)((uint32)pstTimerModule + (uint32)offsetof(tstTimerModule, CONTROLS[TEPMHA_nMissingToothChannel]));

	u32ControlWord = (TIMERPREFIX(_CnSC_MSA_MASK));
	u32ControlWord |= TIMERPREFIX(_CnSC_CHIE_MASK);
	*vpuFTMReg = u32ControlWord;
#endif //BUILD_MK6X
}

uint32 TEPMHA_u32SetNextMissingToothInterrupt(TEPMAPI_ttEventTime tReference, TEPMAPI_ttEventTime tLastGap, uint32 u32Repeats)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	static uint32 u32Gap = 0;
	uint32 u32TimerVal;
	uint32 u32Temp;

	if (0 != u32Repeats)
	{
		u32TimerVal = tReference;
		u32Gap = tLastGap;
		TEPMHA_u32MissingRepeats = u32Repeats;
	}
	else
	{
		u32TimerVal = TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnV;
	}

	if (0 != TEPMHA_u32MissingRepeats)
	{
		u32TimerVal += u32Gap;
		u32TimerVal &= TEPMHA_nCounterMask;

		/* Wait for flag clear */
		while (0 != (TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC & TIMERPREFIX(_CnSC_CHF_MASK)))
		{
			/* Clear the flag */
			TPM0->STATUS = 1 << TEPMHA_nMissingToothChannel;
		}

		u32Temp = TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC | (TIMERPREFIX(_CnSC_CHIE_MASK) | TIMERPREFIX(_CnSC_MSA_MASK));
		u32Temp |= TIMERPREFIX(_CnSC_CHF_MASK);

        if (!u32Repeats)
        {
    		//u32Temp |= TIMERPREFIX(_CnSC_ELSA_MASK);
        }

		TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC = u32Temp;

		while (TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnV != u32TimerVal)
		{
			TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnV = u32TimerVal;
		}

		TEPMHA_u32MissingRepeats--;
	}
	else
	{
		u32TimerVal += 100;
		u32TimerVal &= TEPMHA_nCounterMask;

		// clear the flag
		TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC = TIMERPREFIX(_CnSC_CHF_MASK);
		TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC = 0;

		u32Temp = TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC & ~TIMERPREFIX(_CnSC_CHIE_MASK);
		u32Temp &= ~TPM_CnSC_ELSA_MASK;
		//u32Temp |= TIMERPREFIX(_CnSC_CHF_MASK);
		TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnSC = u32Temp;

		while (TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnV != u32TimerVal)
		{
			TPM0->CONTROLS[TEPMHA_nMissingToothChannel].CnV = u32TimerVal;
		}
	}
#endif //BUILD_MK6X

	return u32Gap;
}

//#pragma GCC optimize ("O1")
