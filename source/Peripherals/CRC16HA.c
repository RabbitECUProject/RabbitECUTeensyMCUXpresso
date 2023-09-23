/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CRC16                                                  */
/* DESCRIPTION:        This code provides CRC16 functions                     */
/*                                                                            */
/* FILE NAME:          CRC16HA.c                                              */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define CRC16HA_C
#include "sys.h"
#include "CPUAbstract.h"
#include "declarations.h"
#include "SIM.h"


#include "CRC16.h"
#include "types.h"

uint16 CRC16_u16CRC;



#ifdef BUILD_SAM3X8E
static uint16 CRC16HA_u16CalcTableCRC(uint16, puint8, uint16);
static puint16 CRC16HA_pu16CalcTableCRC(uint16, puint8, uint16);
#endif //BUILD_SAM3X8E

void CRC16HA_vStart(uint32* const u32Stat)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_CRC_MASK);
#endif
}

void CRC16HA_vRun(uint32* const u32Stat)
{
	
}

void CRC16HA_vTerminate(uint32* const u32Stat)
{
	
}

uint16 CRC16HA_u16CalcCRC(uint16 u16CRC, uint8* pu8Data, uint16 u16Len)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	uint16 u16IDX;
	CRC_Type* pstCRC0 = (CRC_Type*)CRC_BASE;

	pstCRC0 -> GPOLY_ACCESS16BIT.GPOLYL = CRC_GPOLY_LOW(0x1021);
	pstCRC0 -> CTRL |= CRC_CTRL_WAS_MASK;
	pstCRC0 -> DATA = u16CRC;
	pstCRC0 -> CTRL &= ~CRC_CTRL_WAS_MASK;
	
	for (u16IDX = 0; u16IDX < u16Len; u16IDX++)
	{
		pstCRC0 -> ACCESS8BIT.DATALL = pu8Data[u16IDX];
	}	
	
	return (uint16)pstCRC0 -> ACCESS16BIT.DATAL;
#endif //BUILD_MK6X


#ifdef BUILD_SAM3X8E
return CRC16HA_u16CalcTableCRC(u16CRC, pu8Data, u16Len);
#endif

}	

puint16 CRC16HA_pu16CalcCRC(uint16 u16CRC, uint8* pu8Data, uint16 u16Len)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	uint16 u16IDX;
	CRC_Type* pstCRC0 = (CRC_Type*)CRC_BASE;

	pstCRC0 -> GPOLY_ACCESS16BIT.GPOLYL = CRC_GPOLY_LOW(0x1021);
	pstCRC0 -> CTRL |= CRC_CTRL_WAS_MASK;
	pstCRC0 -> ACCESS16BIT.DATAL = u16CRC;
	pstCRC0 -> CTRL &= ~CRC_CTRL_WAS_MASK;
	
	for (u16IDX = 0; u16IDX < u16Len; u16IDX++)
	{
		pstCRC0 -> ACCESS8BIT.DATALU = pu8Data[u16IDX];
	}	
	
	CRC16_u16CRC = (uint16)pstCRC0 -> ACCESS16BIT.DATAL;
	return (puint16)&CRC16_u16CRC;
#endif

#ifdef BUILD_SAM3X8E
    return CRC16HA_pu16CalcTableCRC(u16CRC, pu8Data, u16Len);
#endif
}	

#ifdef BUILD_SAM3X8E
static uint16 CRC16HA_u16CalcTableCRC(uint16 crc_seed, puint8 pu8Data, uint16 len)
{
	uint16 crc = crc_seed;
	while (len--){
		crc = (crc << 8) ^ CRC16HA_au16CRCTable[((crc >> 8) ^ *pu8Data++)];
	}
	return (crc);
}
#endif //BUILD_SAM3X8E

#ifdef BUILD_SAM3X8E
static puint16 CRC16HA_pu16CalcTableCRC(uint16 crc_seed, puint8 pu8Data, uint16 len)
{
	CRC16_u16CRC = crc_seed;
	while (len--){
		CRC16_u16CRC = (CRC16_u16CRC << 8) ^ CRC16HA_au16CRCTable[((CRC16_u16CRC >> 8) ^ *pu8Data++)];
	}
	return (puint16)&CRC16_u16CRC;
}
#endif //BUILD_SAM3X8E


