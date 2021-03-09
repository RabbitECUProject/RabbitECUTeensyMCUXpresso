/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      DISK                                                   */
/* DESCRIPTION:        This code provides functions to support NVM disk       */
/*                     functionality with a FATFS                             */	
/* FILE NAME:          DISK.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <SDHCHA.h>
#include "types.h"
#include "CPUAbstract.h"
#include "regset.h"
#include "declarations.h"
#include "OS.h"
#include "disk.h"
#include "diskio.h"
#include "types.h"
#include "sdhc.h"

#ifdef BUILD_MK60
#include "sdhc1.h"
#endif //BUILD_MK60



void DISK_vStart(uint32* const u32Stat)
{
	/* TODO */
}

void DISK_vRun(uint32* const u32Stat)
{
	/* TODO */
}

void DISK_vTerminate(uint32* const u32Stat)
{

}

DSTATUS disk_initialize(BYTE pdrv)
{
	return 0;
}

DSTATUS disk_status(BYTE pdrv)
{
	return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	return SDHC_u8DiskRead(buff, sector, count);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	return SDHC_u8DiskWrite((uint8* const)buff, sector, count);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	return 0;	
}

