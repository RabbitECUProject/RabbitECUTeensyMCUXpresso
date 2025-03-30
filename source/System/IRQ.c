/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      IRQ                                                    */
/* DESCRIPTION:        This code is interrupt service routines                */
/*                                                                            */
/* FILE NAME:          IRQ.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "BUILD.h"

#ifdef BUILD_MK60
#include "mk60f12.h"
#endif //BUILD_MK60

#ifdef BUILD_MK64
#include "mk64f12.h"
#endif //BUILD_MK64

#ifdef BUILD_MKS20
#include "mks20f12.h"
#endif //BUILD_MKS20

#include <core_cm4.h>
#include <CLIENT.h>
#include <DECLARATIONS.h>
#include <string.h>
#include "PERUART.h"
#include "IOAPI.h"
#include "MSG.h"
#include "irq.h"
#include "OS.h"
#include "dll.h"
#include "PERCAN.h"
#include "PERSPI.h"
#include "math.h"
#include "UDSAL.h"
#include "timer.h"
#include "tqueue.h"
#include "SYSAPI.h"
#include "PERADC.h"
#include "TEPM.h"
#include "IIC.h"

#ifdef BUILD_PBL
#include "ECUHostA_PBL.h"
#endif //BUILD_PBL
	

#ifdef BUILD_MK60
SYSMPU_Type IRQ_stMPU;
IRQRXCallBack IRQ_apfRXCallBack[100];
IRQTXCallBack IRQ_apfTXCallBack[100];
#endif //BUILD_MK60

#ifdef BUILD_MK64
SYSMPU_Type IRQ_stMPU;
IRQRXCallBack IRQ_apfRXCallBack[100];
IRQTXCallBack IRQ_apfTXCallBack[100];
#endif //BUILD_MK64

#ifdef BUILD_MKS20
IRQRXCallBack IRQ_apfRXCallBack[94];
IRQTXCallBack IRQ_apfTXCallBack[94];
#endif //BUILD_MK64

#ifdef BUILD_SAM3X8E
IRQRXCallBack IRQ_apfRXCallBack[PERIPH_COUNT_IRQn];
IRQTXCallBack IRQ_apfTXCallBack[PERIPH_COUNT_IRQn];
#endif //BUILD_SAM3X8E

#ifdef BUILD_KERNEL_OR_KERNEL_APP
extern uint32 OS_u32SysTickInterval;
extern uint32 OS_u32SysRunTimeHigh;
extern uint32 OS_u32SysRunTimeLow;
extern tq_struct* OS_pstBackgroundDispatcherTask;
extern void BlinkyTick(void);
extern void SDHC1_Interrupt(void);
extern void OS_vBackgroundDispatcher(void);
#endif

extern void NVIC_EnableIRQ(IRQn_Type IRQn);
extern void SYS_vAPISVC(void);
DLL_tstRXDLLData CAN_stRXDLLData;
bool IRQ_boEnableRTOS = TRUE;
static void IRQ_vCommonUART(tstUARTModule*, IOAPI_tenEHIOResource, IRQn_Type enIRQType);
static void IRQ_vCommonCAN(tstCANModule*, IOAPI_tenEHIOResource, IRQn_Type enIRQType);

#if defined(BUILD_MK60) || defined(BUILD_MK64)
static void IRQ_vCommonSPI(tstSPIModule*, IOAPI_tenEHIOResource, IRQn_Type enIRQType);
#endif //defined(BUILD_MK60) || defined(BUILD_MK64)

static void SVC_Handler_Main(void);

#if defined(BUILD_MK60) || defined(BUILD_SAM3X8E) || defined(BUILD_MK64) || defined(BUILD_MKS20)
void IRQ_vEnableIRQ(IRQn_Type enIRQType, IRQ_tenPRIO enPRIO, IRQRXCallBack pfRXCallBack, IRQTXCallBack pfTXCallBack)
{
	IRQ_apfRXCallBack[enIRQType] = pfRXCallBack;
	IRQ_apfTXCallBack[enIRQType] = pfTXCallBack;
	NVIC_ClearPendingIRQ(enIRQType);

	if (enIRQType == MK6X_FTM_PRIO_IRQ) enPRIO = 4;

	NVIC_SetPriority(enIRQType, enPRIO);              
	NVIC_EnableIRQ(enIRQType);                       
}
#endif

#if defined(BUILD_MK60) || defined(BUILD_SAM3X8E) || defined (BUILD_MKS20)
void IRQ_vDisableIRQ(IRQn_Type enIRQType)
{
	IRQ_apfRXCallBack[enIRQType] = NULL;
	IRQ_apfTXCallBack[enIRQType] = NULL;
	NVIC_DisableIRQ(enIRQType);
}
#endif

#ifdef BUILD_KERNEL_OR_KERNEL_APP
task_queue IRQ_vSuspendThread(task_queue* stTaskQueueToSuspend)
{
	__asm(".EQU TASK_PC_OFFSET,	22");
	__asm(".EQU TASK_SP_OFFSET,	18");
	__asm(".EQU STACK_FRAME_BASE_OFFSET, 8");
	__asm(".EQU STACK_FRAME_SIZE, 32");
	__asm(".EQU FRAME_R0_OFFSET, 0");
	__asm(".EQU FRAME_R1_OFFSET, 4");
	__asm(".EQU FRAME_R2_OFFSET, 8");
	__asm(".EQU FRAME_R3_OFFSET, 12");
	__asm(".EQU FRAME_R12_OFFSET, 16");
	__asm(".EQU FRAME_LR_OFFSET, 20");
	__asm(".EQU FRAME_PC_OFFSET, 24");
	__asm(".EQU FRAME_XPSR_OFFSET, 28");
	__asm(".EQU MASK_XPSR_THUMB, 16777216");

	//
	//
	//
	// get first task in R1
	__asm("LDR		R1, [R0]");

	// get next task in R2
	__asm("LDR		R2, [R1]");
	__asm("B 		TestQueueLast");
	__asm("FindQueueEndLoop:");
	__asm("MOV		R3, R2");
	__asm("LDR		R2,	[R2]");
	__asm("TestQueueLast:");
	__asm("CMP		R2, #0");
	__asm("BNE		FindQueueEndLoop");
	// queue pointer is in R0, current task R1, end task R3

	// change queue pointer to current task->next
	__asm("LDR		R2, [R1]");
	__asm("CMP		R1, R2");
	__asm("BEQ		ThreadSwapAbort");
	__asm("STR		R2, [R0]");

	//	set end task->next to current task
	__asm("STR		R1, [R3]");
	
	// set current task->next to NULL
	__asm("MOV		R3, #0");
	__asm("STR		R3, [R1]");
	
	// get thread address in R4 MATTHEW
	//		MOV		R4, #255
	
	// store current task program counter in task structure program counter
	//		STR		R4, [R1, #TASK_PC_OFFSET] MATTHEW
	
	// update the task saved stack pointer
	__asm("MRS		R0, PSP");
	// MATTHEW for R4 PUSH		NO!!!!!!!
	//		ADD		R0, #4
	__asm("STR		R0, [R1, #TASK_SP_OFFSET]");
	
	// get background dispatcher task stack pointer
	__asm("LDR		R1, =OS_pstBackgroundDispatcherTask");
	__asm("LDR		R1, [R1]");
	__asm("LDR		R1,	[R1, #TASK_SP_OFFSET]");
	
	// set	background dispatcher stack frame return address to
	// dispatcher entry point always
	__asm("LDR		R2, =OS_vBackgroundDispatcher");
	__asm("ORR		R2, #1");
	__asm("STR		R2,	[R1, #FRAME_PC_OFFSET]");
	
	// set XPSR thumb bit always
	//LDR		R2, [R1, #FRAME_XPSR_OFFSET]
	__asm("MOV		R2, #0");
	__asm("ORR		R2, #MASK_XPSR_THUMB");
	__asm("STR		R2, [R1, #FRAME_XPSR_OFFSET]");
	
	// store dispatcher stack pointer to PSP
	__asm("MSR		PSP, R1");
	
    __asm("ThreadSwapAbort:");
}
#endif //BUILD_KERNEL

void IRQ_vReset(void)
{
	uint32 u32IDX;

	for (u32IDX = 0; u32IDX < (sizeof(IRQ_apfRXCallBack) / sizeof(IRQRXCallBack)); u32IDX++)
	{
		IRQ_apfRXCallBack[u32IDX] = NULL;
		IRQ_apfRXCallBack[u32IDX] = NULL;
	}
}


/*----------------------------------------------------------------------------
  SVC_Handler
 *----------------------------------------------------------------------------*/
#ifdef BUILD_PBL
__asm void SVC_Handler(void) 
{}
#endif	
#ifdef BUILD_SBL
__asm void SVC_Handler(void) 
{}	
#endif	
#if defined(BUILD_PBL) || defined (BUILD_SBL)
void SVC_Handler(void)
{}
#endif
#ifdef BUILD_KERNEL_OR_KERNEL_APP
void SVC_Handler(void)
{
	//IMPORT 	SVC_Handler_Main
	//IMPORT 	OS_stSVCDataStruct
	__asm("TST   lr, #4");
	__asm("ITE   NE");
	__asm("MRSNE r0, PSP");
	__asm("MRSEQ r0, MSP");
	
	// load the link register containing user module return address into R0
	// module return address will be used as module call handle - UID which
	// infers privilege and is also unique COMPILER DEPENDENT STACK FRAME OFFSET???
	__asm("LDR   R0, [R0, #28]");
	__asm("PUSH  {R1}");
	__asm("LDR   R1, =OS_stSVCDataStruct");
	__asm("STR   R0, [R1, #4]");
	__asm("POP   {R1}");
	
	__asm("TST   lr, #4");
	__asm("ITE   NE");
	__asm("MRSNE r0, PSP");
	__asm("MRSEQ r0, MSP");
	SVC_Handler_Main();
}
#endif //BUILD_KERNEL

#ifdef BUILD_KERNEL_OR_KERNEL_APP
static void SVC_Handler_Main(void)
{
	SYS_vAPISVC();
}
#endif

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) 
{
#ifdef BUILD_PBL
	PBL_vCyclicTask();
#endif	
#ifdef BUILD_SBL
	SBL_vCyclicTask();	
#endif
#ifdef BUILD_KERNEL_OR_KERNEL_APP
	/* declare data static because are usually consuming
		 a thread stack here for temp data! */
	static uint32 u32SysRunTimeLowOld;
 	static OS_tenOSState enOSState;		
	static uint32 u32Stat;
	static task_queue* pstTaskQueue;	
	static task_queue* pstTaskQueueToSuspend;
	static task_queue stTaskQueue;	
	
	stTaskQueue = NULL;
	pstTaskQueueToSuspend = NULL;	
	
	OS_u32SysRunTimeLow += OS_u32SysTickInterval;
	OS_u32SysRunTimeHigh = (u32SysRunTimeLowOld > OS_u32SysRunTimeLow) ?
													OS_u32SysRunTimeHigh + 1 : OS_u32SysRunTimeHigh;
	u32SysRunTimeLowOld = OS_u32SysRunTimeLow;
	
	/* call this OS module run function from the interrupt context although
	   normally OS module runs are called in co-operative multitask context */
	TIMER_vRun(&u32Stat);

 	enOSState = OS_enGetState();
	
	if (OS_enOSStateOSStarted == enOSState)
	//if ((OS_enOSStateOSStarted == enOSState) &&
	//	(TRUE == IRQ_boEnableRTOS))
	{		
		/* check if kernel cyclic queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enKERNELQueueCyclic);
			stTaskQueue = *pstTaskQueue;
			if (NULL != stTaskQueue)
			{			
				OS_vOverrunTaskReport(OS_enKERNELQueueCyclic);
				/* call schedule - no thread to swap out we are still co-op tasking */				
				//OS_vSchedule(OS_enOSSchedulerTick);
			}
		}	
		
		/* check if client cyclic queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enCLIENTQueueCyclic);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				OS_vOverrunTaskReport(OS_enCLIENTQueueCyclic);
				/* call schedule - no thread to swap out we are still co-op tasking */				
				//OS_vSchedule(OS_enOSSchedulerTick);
			}
		}			

		/* check if kernel thread queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enKERNELQueueThread);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				/* swap out thread before calling schedule */
				pstTaskQueueToSuspend = pstTaskQueue;
				OS_vSchedule(OS_enOSSchedulerTick);
			}
		}	

		/* check if client thread queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enCLIENTQueueThread);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				/* swap out thread before calling schedule */
				pstTaskQueueToSuspend = pstTaskQueue;
				OS_vSchedule(OS_enOSSchedulerTick);
			}
		}
		
		if(NULL != pstTaskQueueToSuspend)
		{
			stTaskQueue = IRQ_vSuspendThread(pstTaskQueueToSuspend);
		}
		
		stTaskQueue = NULL;		
	}
	else if (OS_enOSStartRequest == enOSState)
	{
		OS_vSetState(OS_enOSStateOSStarted);	
	}
#endif	
}


/*----------------------------------------------------------------------------
  MemManage_Handler
 *----------------------------------------------------------------------------*/
void MemManage_Handler(void)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64)
	memcpy((void*)&IRQ_stMPU, (void*)SYSMPU_BASE, sizeof(SYSMPU_Type));
	while(1);
#endif //defined(BUILD_MK60) || defined(BUILD_MK64)
}

/*----------------------------------------------------------------------------
  ENET_TX_Handlers
 *----------------------------------------------------------------------------*/
void ENET_Transmit_IRQHandler(void)
{
	
}

/*----------------------------------------------------------------------------
  ENET_RX_Handlers
 *----------------------------------------------------------------------------*/
void ENET_Receive_IRQHandler(void)
{
// 	ENET_EIMR_BABT_MASK 	
// 	ENET_EIMR_GRA_MASK			
// 	ENET_EIMR_EBERR_MASK	
// 	ENET_EIMR_LC_MASK			
// 	ENET_EIMR_RL_MASK			
// 	ENET_EIMR_UN_MASK	
#ifdef BUILD_ENET
	if(ENE_xIntActivated(ENET_EIMR_BABR_MASK))
	{		
		ENE_xIntClearFlag(ENET_EIMR_BABR_MASK);
	}
	
	if(ENE_xIntActivated(ENET_EIMR_RXF_MASK))
	{		
		ENE_xIntClearFlag(ENET_EIMR_RXF_MASK);
		ENE_vReceiveFrame();		
	}
#endif
}

/*----------------------------------------------------------------------------
  ENET_ERR_Handlers
 *----------------------------------------------------------------------------*/
void ENET_Error_IRQHandler(void)
{
	
	
}



/*----------------------------------------------------------------------------
  SPI2_Handlers
 *----------------------------------------------------------------------------*/
void SPI2_IRQHandler(void)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64)
	SPI_Type* pstSPI = SPI2;
	IRQ_vCommonSPI(pstSPI, EH_VIO_SPI1, SPI2_IRQn);
#endif //defined(BUILD_MK60) || defined(BUILD_MK64)
}



/*----------------------------------------------------------------------------
  UART0_Handlers
 *----------------------------------------------------------------------------*/
void UART0_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART0;
	IRQ_vCommonUART(pstUART, EH_VIO_UART1, UART0_RX_TX_IRQn);
}

void UART0_ERR_IRQHandler(void){}
	
/*----------------------------------------------------------------------------
  UART1_Handlers
 *----------------------------------------------------------------------------*/
void UART1_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART1;
	IRQ_vCommonUART(pstUART, EH_VIO_UART2, UART1_RX_TX_IRQn);
}

void UART1_ERR_IRQHandler(void){}

/*----------------------------------------------------------------------------
  UART2_Handlers
 *----------------------------------------------------------------------------*/
void UART2_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART2;
	IRQ_vCommonUART(pstUART, EH_VIO_UART3, UART2_RX_TX_IRQn);
}

void UART2_ERR_IRQHandler(void){}
	
/*----------------------------------------------------------------------------
  UART5_Handlers
 *----------------------------------------------------------------------------*/
void UART3_RX_TX_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	UART_Type* pstUART = UART3;
	IRQ_vCommonUART(pstUART, EH_VIO_UART4, UART3_RX_TX_IRQn);
#endif
}

void UART3_ERR_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
#endif
}

/*----------------------------------------------------------------------------
  UART4_Handlers
 *----------------------------------------------------------------------------*/
void UART4_RX_TX_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	UART_Type* pstUART = UART4;
	IRQ_vCommonUART(pstUART, EH_VIO_UART5, UART4_RX_TX_IRQn);
#endif
}

void UART4_ERR_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
#endif
}

/*----------------------------------------------------------------------------
  UART5_Handlers
 *----------------------------------------------------------------------------*/
void UART5_RX_TX_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	UART_Type* pstUART = UART5;
	IRQ_vCommonUART(pstUART, EH_VIO_UART6, UART5_RX_TX_IRQn);
#endif
}

void UART5_ERR_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
#endif
}
	
	
/*----------------------------------------------------------------------------
  SDHC_Handler
 *----------------------------------------------------------------------------*/
void SDHC_IRQHandler(void)
{
#ifdef BUILD_KERNEL_OR_KERNEL_APP
	//SDHC1_Interrupt();
#endif	
}

/*----------------------------------------------------------------------------
  ADCX_Handlers
 *----------------------------------------------------------------------------*/
void ADC0_IRQHandler(void)
{
#ifdef BUILD_KERNEL_OR_KERNEL_APP
	IRQ_apfRXCallBack[ADC0_IRQn](EH_VIO_ADC0, NULL);
#endif	
}

void ADC1_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	IRQ_apfRXCallBack[ADC1_IRQn](EH_VIO_ADC1, NULL);
#endif	
}

void ADC2_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && defined(BUILD_MK60)
	IRQ_apfRXCallBack[ADC2_IRQn](NULL, NULL);
	#endif
}

void ADC3_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && defined(BUILD_MK60)
	IRQ_apfRXCallBack[ADC3_IRQn](NULL, NULL);
#endif	
}


/*----------------------------------------------------------------------------
  FTMX_Handlers
 *----------------------------------------------------------------------------*/
void TPM0_IRQHandler(void)
{
#ifdef BUILD_KERNEL_OR_KERNEL_APP
	uint32 u32Prio = 1;
	IRQ_apfRXCallBack[TPM0_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TPM0, (void*)&u32Prio);
#endif	
}

void TPM1_IRQHandler(void)
{
#ifdef BUILD_KERNEL_OR_KERNEL_APP
	uint32 u32Prio = 1;
	IRQ_apfRXCallBack[TPM1_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TPM1, (void*)&u32Prio);
#endif	
}

void TPM2_IRQHandler(void)
{
#ifdef BUILD_KERNEL_OR_KERNEL_APP
	uint32 u32Prio = 1;
	IRQ_apfRXCallBack[TPM2_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TPM2, (void*)&u32Prio);
#endif	
}

void FTM3_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	uint32 u32Prio = 1;
	IRQ_apfRXCallBack[FTM3_IRQn]((IOAPI_tenEHIOResource)EH_VIO_FTM3, (void*)&u32Prio);
#endif	
}

/*----------------------------------------------------------------------------
  IICX_Handlers
 *----------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	IRQ_apfRXCallBack[I2C0_IRQn](NULL, NULL);	
#endif
}

void I2C1_IRQHandler(void)
{
#if defined(BUILD_KERNEL_OR_KERNEL_APP) && (defined(BUILD_MK60) || defined(BUILD_MK64))
	IRQ_apfRXCallBack[I2C1_IRQn](NULL, NULL);
#endif	
}
	
#if defined(BUILD_MK64) || defined(BUILD_MKS20)
void CAN0_ORed_Message_buffer_IRQHandler(void)
{
	tstCANModule* pstCAN = CAN0;

	IRQ_vCommonCAN(pstCAN, EH_VIO_CAN1, CAN0_ORed_Message_buffer_IRQn);
}
#endif //BUILD_MK64

#ifdef BUILD_MK60
void CAN1_ORed_Message_buffer_IRQHandler(void)
{
	CAN_Type* pstCAN = CAN1;
	
	IRQ_vCommonCAN(pstCAN, EH_VIO_CAN2, CAN0_ORed_Message_buffer_IRQn);
}
#endif

static void IRQ_vCommonCAN(tstCANModule* pstCAN, IOAPI_tenEHIOResource enEHIOResource, IRQn_Type enIRQType)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	CANHA_tstCANMB* pstCANMB;
	uint32 u32IMask = 1;
	uint32 u32MBIDX;
	uint32 u32IMaskMax = MATH_u32IDXToMask(CAN_nCANMailboxCount);
	
	while(u32IMask < u32IMaskMax)
	{
		if((u32IMask & (pstCAN -> IFLAG1)) == u32IMask)
		{
			pstCANMB = (CANHA_tstCANMB*)&pstCAN -> MB[0];
			u32MBIDX = MATH_u32MaskToIDX(u32IMask);
			pstCANMB += u32MBIDX;
			
			while(((pstCANMB -> u32CS & CAN_CS_CODE_MASK)
				>> CAN_CS_CODE_SHIFT) == CAN_nCANMBXBusy);
					
			IRQ_apfRXCallBack[enIRQType](enEHIOResource, (void*)pstCANMB);			
			pstCAN -> IFLAG1 |= u32IMask;
		}
		u32IMask <<= 1;
		/* abort loop if all flags clear */
		if (pstCAN -> IFLAG1 == 0) 
		{
			u32IMask = u32IMaskMax;
		}
	}
#endif
}

void IRQ_vCommonUART(UART_Type* pstUART, IOAPI_tenEHIOResource enEHIOResource, IRQn_Type enIRQType)
{
	DLL_tstRXDLLData stRXDLLData;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	if(((pstUART -> S1) & UART_S1_TDRE_MASK) == UART_S1_TDRE_MASK)
	{
		if(((pstUART -> C2) & UART_C2_TIE_MASK) == UART_C2_TIE_MASK)
		
			IRQ_apfTXCallBack[enIRQType](enEHIOResource, NULL);	
	}
	
	if(((pstUART -> S1) & UART_S1_RDRF_MASK) == UART_S1_RDRF_MASK)
	{
		stRXDLLData.u8Data[0] = UART_u8GetChar(enEHIOResource);
		stRXDLLData.u8DataCount = 1;
		stRXDLLData.u8RXByteIDX = 0;		
		IRQ_apfRXCallBack[enIRQType](enEHIOResource, (void*)&stRXDLLData);
	}

	if(((pstUART -> S1) & UART_S1_FE_MASK) == UART_S1_FE_MASK)
	{
		(void)UART_u8GetChar(enEHIOResource);
	}
#endif //BUILD_MK6X
}

#if defined(BUILD_MK60) || defined(BUILD_MK64)
void IRQ_vCommonSPI(SPI_Type* pstSPI, IOAPI_tenEHIOResource enEHIOResource, IRQn_Type enIRQType)
{

	DLL_tstRXDLLData stRXDLLData;
	IRQ_apfTXCallBack[enIRQType](enEHIOResource, NULL);
}
#endif //BUILD_MK6X

void IRQ_vEnableRTOS(bool boEnable)
{
	IRQ_boEnableRTOS = boEnable;
}




