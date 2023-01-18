/*----------------------------------------------------------------------------
 * Name:    DisplayMain.c
 * Purpose: Freescale MK60X256VMD100 LED Flasher
 *----------------------------------------------------------------------------*/
#include <DECLARATIONS.h>
#include "TQUEUE.h"
#include <TASK.h>
#include "mks20f12.h"
#include "OS.h"
#include "CPUAbstract.h"
#include "fsl_clock.h"

bool boGlobalTickFlag;
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
extern uint32 OS_u32SysTickInterval;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int ECUHostMain(void){
	uint32 u32Stat;
	puint32 pu32PBLWord = (puint32)0x1ffffffc;

	if (0x55AA0001 != *pu32PBLWord)
	{	
		SystemCoreClockUpdate();                      /* Get Core Clock Frequency */
		SysTick_Config(SystemCoreClock / 1000);       /* Generate interrupt each 1 ms */
	}
	else
	{
		CPU_xExitCritical();
	}		
	
	CLOCK_SetXtal0Freq(8000000);
	OS_u32SysTickInterval = SystemCoreClock / 1000;
	OS_vStart(&u32Stat);

	*pu32PBLWord = 0xaa550001;
	
	while(1)
	{
		OS_vRun((uint32*)&u32Stat);
	}
}

