/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*   License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher    */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      OS Header File                                         */
/* DESCRIPTION:        This code provides macros that describe the OS task    */
/*                     schedule and OS enums                                  */
/* FILE NAME:          OS.h                                                   */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef OS_H
#define OS_H

#include <SYS.h>
#include <TASK.h>
#include <TYPES.h>
#include "SYSAPI.h"
#include "tqueue.h"
#include "TASKAPI.h"

#define OS_MULTI_THREAD_off

/*<AUTOGEN START>*/
#define OS_nTaskStart 																																																																										\
/* enTaskID, 					enType, 		enPriority,     enState,      enRateMS,     		pfFunction,	 tSP,  tPC, tsCreated,  tsStarted, tsFinished, tsAccumulated, tsPrivilege */\
{																																																																																					\
	{ 1u , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CLIENT_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 2u , enTaskFunction	, 5u                    , enTaskReady, TASKAPI_enTaskCycNot	, &RESM_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 3u , enTaskFunction	, 5u                    , enTaskReady, TASKAPI_enTaskCycNot	, &SPREADS_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 4u , enTaskFunction	, 5u                    , enTaskReady, TASKAPI_enTaskCycNot	, &TABLES_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 5u , enTaskFunction	, 5u                    , enTaskReady, TASKAPI_enTaskCycNot	, &UART_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 6u , enTaskFunction	, 15u                   , enTaskReady, TASKAPI_enTaskCycNot	, &UDSAL_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 7u , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &PIM_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 8u , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &FEE_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 9u , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CRC16_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 10u, enTaskFunction	, 200u                  , enTaskReady, TASKAPI_enTaskCycNot	, &ADC_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 11u, enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DAC_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 12u, enTaskFunction	, 100u                  , enTaskReady, TASKAPI_enTaskCycNot	, &PIT_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 13u, enTaskFunction	, 66u                   , enTaskReady, TASKAPI_enTaskCycNot	, &DLL_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 14u, enTaskFunction	, 77u                   , enTaskReady, TASKAPI_enTaskCycNot	, &DSCRIO_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 15u, enTaskFunction	, 77u                   , enTaskReady, TASKAPI_enTaskCycNot	, &TIMER_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 16u, enTaskFunction	, 77u                   , enTaskReady, TASKAPI_enTaskCycNot	, &SDHC_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 17u, enTaskFunction	, 77u                   , enTaskReady, TASKAPI_enTaskCycNot	, &DISK_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 18u, enTaskFunction	, 77u                   , enTaskReady, TASKAPI_enTaskCycNot	, &TIME_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 19u, enTaskFunction	, 78u                   , enTaskReady, TASKAPI_enTaskCycNot	, &CAN_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 20u, enTaskFunction	, 78u                   , enTaskReady, TASKAPI_enTaskCycNot	, &CTRL_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 21u, enTaskFunction	, 78u                   , enTaskReady, TASKAPI_enTaskCycNot	, &TEPM_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 22u, enTaskFunction	, 78u                   , enTaskReady, TASKAPI_enTaskCycNot	, &VCOM_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 23u, enTaskFunction	, 78u                   , enTaskReady, TASKAPI_enTaskCycNot	, &SRLTFR_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	} \
}

#define OS_nTaskCyclic 																																																																										\
/* enTaskID, 					enType, 		enPriority,     enState,      enRateMS, 				pfFunction,	 tSP,  tPC, tsCreated,  tsStarted, tsFinished, tsAccumulated, tsPrivilege */\
{																																																																																					\
	{ 24u    , enTaskCyclic		, TASKAPI_enTaskPrMaxCyc, enTaskReady, 100u					, &UART_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 25u    , enTaskCyclic		, 101u          , enTaskReady, 10u						, &UDSAL_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 26u    , enTaskCyclic		, 180u          , enTaskReady, 20u					, &PIM_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 27u    , enTaskCyclic		, 115u          , enTaskReady, 20u					, &ADC_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 28u    , enTaskCyclic		, 115u          , enTaskReady, 20u					, &DAC_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 29u    , enTaskCyclic		, 110u          , enTaskReady, 20u					, &PIT_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 30u    , enTaskCyclic		, 200u          , enTaskReady, 1u						, &DLL_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 31u    , enTaskCyclic		, 166u          , enTaskReady, 1u						, &FEE_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 32u    , enTaskCyclic		, 166u          , enTaskReady, 250u					, &DSCRIO_vRun			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 33u    , enTaskCyclic		, 166u          , enTaskReady, 250u					, &SDHC_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 34u    , enTaskCyclic		, 166u          , enTaskReady, 250u					, &DISK_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 35u    , enTaskCyclic		, 166u          , enTaskReady, 250u					, &TIME_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 36u    , enTaskCyclic		, 19u           , enTaskReady, 1u						, &CAN_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 37u    , enTaskCyclic		, 19u           , enTaskReady, 1u						, &CTRL_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 38u    , enTaskCyclic		, 19u           , enTaskReady, 1u						, &TEPM_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 39u    , enTaskCyclic		, 19u           , enTaskReady, 1u						, &CRC16_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 40u    , enTaskCyclic		, 19u           , enTaskReady, 1u						, &VCOM_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 41u    , enTaskCyclic		, 19u           , enTaskReady, 5u						, &SRLTFR_vRun			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	} \
}

#define OS_nTaskTerminate																																																																									\
/* enTaskID, 					enType, 		enPriority,     enState,      enRateMS,     		pfFunction,	 tSP,  tPC, tsCreated,  tsStarted, tsFinished, tsAccumulated, tsPrivilege */\
{																																																																																					\
	{ 42u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &UART_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 43u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &UDSAL_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 44u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &PIM_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 45u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &ADC_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 46u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DAC_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 47u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &PIT_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 48u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DLL_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 49u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &FEE_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 50u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DSCRIO_vTerminate, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 51u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &SDHC_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 52u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DISK_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 53u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &TIME_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 54u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &TIMER_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 55u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CAN_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 56u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CRC16_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 57u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &VCOM_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 58u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &SRLTFR_vTerminate, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	} \
}
/*<AUTOGEN END>*/


/*<AUTOGEN START>*/



typedef enum
{
	OS_enOSStateReset,
	OS_enOSStatePreBootloader,
	OS_enOSStatePreOS,
	OS_enOSStartRequest,
	OS_enOSStateOSStarted,
	OS_enOSStateAppStarted,
	OS_enOSStateCount
} OS_tenOSState;
/*<AUTOGEN END>*/

/*<AUTOGEN START>*/
typedef enum
{
	OS_enOSIDXCLIENT,
	OS_enOSIDXUART,
	OS_enOSIDXDIAG,
	OS_enOSIDXPIM,
	OS_enOSIDXADC,
	OS_enOSIDXPIT,
	OS_enOSIDXDLL,
	OS_enOSIDXFEE,
	OS_enOSIDXENE,
	OS_enOSIDXBMX,
	OS_enOSIDXWIFI,
	OS_enOSIDXCAN,
	OS_enOSIDXCount
} OS_tenOSModuleIDX;

typedef enum
{
	OS_enOSModeKernel,
	OS_enOSModeClient
} OS_tenOSKernelMode;

typedef enum
{
	OS_enOSSchedulerTick,
	OS_enOSSchedulerSWI,	
	OS_enOSSchedulerBHRequest
} OS_tenOSScheduleCallContext;

typedef enum
{
	OS_enKERNELQueueStart,
	OS_enKERNELQueueCyclic,
	OS_enKERNELQueueThread,	
	OS_enKERNELQueueTerminate,
	OS_enCLIENTQueueStart,
	OS_enCLIENTQueueCyclic,
	OS_enCLIENTQueueThread,
	OS_enCLIENTQueueTerminate,
	OS_enTaskQueueCount
} OS_tenQueueType;

	
#define OS_xModuleStartOK( x ) 			( (x) = ( (x) << 1 ) | 1 )
#define OS_xModuleStartFAIL( x )		( (x) = (x) << 1 )
#define OS_xModuleRunOK( x ) 				( (x) = ( (x) << 1 ) | 1 )
#define OS_xModuleRunEnd( x ) 			( (x) = ( (x) << 1 ))
#define OS_xModuleRunFAIL( x )			( (x) = ( (x) = 0 )
#define OS_xAddTaskToRunQueue( x ) 	(		
#define OS_nTickCounterTerminal 		4000000000u
#define OS_MODULE_START( x )      	x ( &OS_ucModuleStartStatus );
#define OS_nKernelHandle						0xF0001000

	
/* macro to add any task to the static task queue */
#define OS_xAddTaskToList( x, y )		memcpy( (OS_tstSYSTaskNodePool*)x->stTask[ (OS_tstSYSTaskNodePool*)x->u16Count++ ], \
																		(TASK_tstTask*)y, sizeof( TASK_tstTask ) )

#ifdef SYS_TASK_PRIORITISE		
	#define OS_xPrioritiseQueue( x )	OS_vPrioritiseQueue( x );
#else
	#define OS_xPrioritiseQueue( x )	OS_vPrioritiseQueue( NULL );
#endif

#define OS_nClientTasksMax 35u

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _OS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN SYSAPI_tstSVCDataStruct OS_stSVCDataStruct;
EXTERN uint32 au32SVCResult[10];
EXTERN uint32 OS_u32TickCounter;

void OS_vStart( uint32* const );
void OS_vRun( uint32* const );
void OS_vSchedule( OS_tenOSScheduleCallContext );
void OS_vTerminate( uint32* const );
void OS_vSetCyclicTaskList (TASK_tstTask*);
void OS_vPrioritiseQueue( task_queue* );
void OS_vBackgroundDispatcher( void );
OS_tenOSState OS_enGetState( void );
void OS_vSetState(OS_tenOSState);
task_queue* OS_stGetOSQueue( OS_tenQueueType);
OS_tenOSKernelMode OS_enGetOSKernelMode( void );
void OS_vOverrunTaskReport( OS_tenQueueType );
void OS_vSuspendThread( OS_tenQueueType );
void OS_vSystemIdleThread( uint32* const );
void OS_vInterCoopTasks(void);
SYSAPI_tenSVCResult OS_enAddTaskToQueue(OS_tenQueueType, TASK_tstTask*);

#endif // OS_H
	

