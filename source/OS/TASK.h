/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TASK Header File                                       */
/* DESCRIPTION:        This code provides definitions of task structs and     */
/*                     enums                                                  */
/* FILE NAME:          TASK.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TASK_H
#define TASK_H

#include <BUILD.h>
#include <TYPES.h>
#include "TASKAPI.h"

// typedef enum
// {	
// 	enTaskPrMaxAll = 0u,
// 	enTaskPrMaxCyc = 128u,
// 	enTaskPrLowest = 255u,
// }	TASK_tenPriority;

// typedef enum
// {
// 	enTaskCycBad = 0u,
// 	enTask1ms = 1u,
// 	enTask2ms = 2u,
// 	enTask5ms = 5u,
// 	enTask10ms = 10u,
// 	enTask25ms = 25u,
// 	enTask100ms = 100u,
// 	enTask200ms = 200u,
// 	enTaskCycNot = 255u
// } TASK_tenRateMs;

typedef enum {
	enTaskNew, enTaskRunning, enTaskReady, enTaskFinished, enTaskBlocked
} TASK_tenState;

typedef enum {
	enTaskFunction, enTaskCyclic, enTaskThread
} TASK_tenType;

typedef enum {
	enTaskPrivileged = 0, enTaskUnprivileged = 1
} TASK_tenPrivilege;

typedef uint16 TASK_tID;
typedef uint32 TASK_tstTime;
typedef void* TASK_tProgramCounter;
typedef void* TASK_tStackPointer;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

typedef struct
{
	TASK_tID tTaskID;                          //0
	TASK_tenType enTaskType;                   //2
	TASKAPI_tenPriority enTaskPriority;        //3
	TASK_tenState enTaskState;                 //4
	TASKAPI_tenRateMs enTaskRateMS;            //5
	void (*pfTaskFunction)(uint32* const);     //6
	TASK_tStackPointer tTaskStackPointer;      //10
	TASK_tProgramCounter tTaskProgramCounter;  //14
	TASK_tstTime tsTaskQueued;                  //18
	TASK_tstTime tsTaskStarted;                 //22
	TASK_tstTime tsTaskFinished;                //26
	TASK_tstTime tsTaskAccumulated;             //30
	TASK_tenPrivilege enPrivilege;             //34
} BUILD_PACKING TASK_tstTask;

#pragma GCC diagnostic pop

TASK_tstTime TASK_vReportRunTime(TASK_tstTask* const);


#endif // TASK_H

