/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <DECLARATIONS.h>
#include <MACROS.h>
#include "mks20f12.h"
#include <string.h>
#include <SYS.h>
#include <TYPES.h>

/* include the header files */
#include "SSD_Types.h"
#include "SSD_FTFx.h"
#include "SSD_FTFx_Internal.h"
#include "SSD_FTFx_Device.h"

#include "IOAPI.h"
#include "regset.h"
#include "os.h"
#include "DLL.h"
#include "COMMONNL.h"
#include "FEE.h"
#include "FEEHA.h"

#ifdef BUILD_SAM3X8E
#include "FLASHHA.h"
#endif //BUILD_SAM3X8E

#include "TEPM.h"

tstFTFEConfig FEE_stFlashSSDConfig;

uint32 returnCode;          /* Return code from each SSD function */
uint32 destination;         /* Address of the target location */
uint32 size;
uint32 source;
uint32 protectStatus;       /* Store Protection Status Value of PFLASH */
uint8  protectStatus1;      /* Store Protection Status Value of DFLASH or EEPROM */
uint32 FailAddr;
uint8  FailData;
uint8  marginLevel;
uint16 phraseNumber;        /* Number of phrase to be verified*/
uint8  EEEDataSizeCode;
uint8  DEPartitionCode;
uint32 Sum;
uint32 unsecure_key;
uint32 i;
uint8 u8WriteCount;
uint32 FEE_u32RunCount;
uint32 FEE_u32InhibitNVMCount;

extern FEE_tstWriteControlBlock FEE_stWriteControlBlock;
extern FEE_tenPgmErrCode FEE_enPgmErrCode;
extern FEE_tstWorkingPage FEE_stWorkingPage;

#if	BUILD_SBL
	extern uint8 FEE_au8ProgBuff[FEE_PFLASH_SCTR_BYTES];
#else
	extern uint8 FEE_au8ProgBuff[1];
#endif

const REGSET_tstReg8Val FEEHA_rastFTFEReg8Val[] = FEEHA_nReg8Set;

static bool FEEHA_boPartitionDFlash(void);
static void FEEHA_vInitFlashConfig(void);
static bool FEEHA_boGetSem(bool);
static uint8* FEEHA_pu8GetPartitionAddress(void);
static uint8* FEEHA_pu8GetFreePartitionAddress(void);

void FEEHA_vStartSBL(void)
{
#if BUILD_SBL
	FTFE_Type* pstFTFE = FTFE;	
	
	if (((pstFTFE -> FCNFG) & FTFPREFIX(_FCNFG_RAMRDY_MASK) != FTFPREFIX(_FCNFG_RAMRDY_MASK)
	{
		/* Make FlexRAM available as RAM for programming */
		pSetEEEEnable(&FEE_stFlashSSDConfig, 0xff);
	}
#endif	
}

bool FEEHA_boCheckPartition(void)
{
	bool boPartitionOK = false;
	
#if defined(BUILD_MK60)
	tstFTFEModule* pstFTFE;
    pstFTFE = FTFE;

	if (((pstFTFE -> FCNFG) & FTFE_FCNFG_EEERDY_MASK) != FTFE_FCNFG_EEERDY_MASK)
	/* If DFlash not already partitioned */
	{	
		boPartitionOK = FALSE;
	}
	else
	{
		boPartitionOK = TRUE;
	}	
#endif //BUILD_MK60

#if defined(BUILD_MK64)
    uint32* pu32NVRecordBase = (uint32*)(FEEHA_DFLASH_END - FEEHA_WORK_DATA_MAX + 1);

    while ((uint32*)FEEHA_DFLASH_START <= pu32NVRecordBase)
    {
    	if (~0 != *pu32NVRecordBase)
    	{
    		boPartitionOK = TRUE;
    		break;
    	}

    	pu32NVRecordBase -= (FEEHA_WORK_DATA_MAX / sizeof(uint32));
    }
#endif //BUILD_MK64

#if defined(BUILD_MKS20)
    uint32* pu32NVRecordBase = (uint32*)(FEEHA_PFLASH_END - FEEHA_WORK_DATA_MAX + 1);

    while ((uint32*)FEEHA_FLASH_NVM_RECS <= pu32NVRecordBase)
    {
    	if (~0 != *pu32NVRecordBase)
    	{
    		boPartitionOK = TRUE;
    		break;
    	}

    	pu32NVRecordBase -= (FEEHA_WORK_DATA_MAX / sizeof(uint32));
    }
#endif //BUILD_MKS20

	return boPartitionOK;
}

bool FEEHA_boSetWorkingData(puint8 pu8WorkingAddress, uint16 u16WorkingDataCount)
{
	bool boWorkingDataOK = FALSE;
	
	if (((puint8)FEEHA_WORK_DATA_START <= pu8WorkingAddress) &&
			((puint8)FEEHA_WORK_DATA_END >= pu8WorkingAddress) &&
			((uint16)FEEHA_WORK_DATA_MAX > u16WorkingDataCount) &&
			(2 <= u16WorkingDataCount))
	{
		FEE_stWorkingPage.pu8WorkingData = pu8WorkingAddress;
		FEE_stWorkingPage.s16WorkingDataCount = (sint16)u16WorkingDataCount;		
		boWorkingDataOK = TRUE;		
	}
		
	return boWorkingDataOK;
}



bool FEEHA_boNVMWorkingCopy(bool boNVMToWorking, bool boCheckCRC16MakeCRC16, uint32 u32AddressOverride)
{		
	bool boCopyOK = false;
	puint16 pu16CRC16Computed;
	puint16 pu16CRC16Stored;

#if defined(BUILD_MK60)
	tstFTFEModule* pstFTFE;
	boCopyOK = FEEHA_boCheckPartition();
	uint32 u32RetCode = FTFx_OK;
    pstFTFE = FTFE;
	uint32 u32NVMWordCount;
	uint32 u32NVMWordIDX;
	uint32 u32SourceWord;
	uint32 u32DestinationWord;

	if ((TRUE == boCopyOK) && 
			(0 < FEE_stWorkingPage.s16WorkingDataCount) &&
			(NULL != FEE_stWorkingPage.pu8WorkingData))
	{	
		if (TRUE == boNVMToWorking)
		/* Copy NVM page to working page */
		{
			pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)FEEHA_EEPROM_START,
				FEE_stWorkingPage.s16WorkingDataCount - 2));
			
			pu16CRC16Stored = (puint16)((uint32)FEEHA_EEPROM_START +
				(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);
			
			if (TRUE == boCheckCRC16MakeCRC16)
			{
				if (*pu16CRC16Stored == *pu16CRC16Computed)
				{
					memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
						(void*)FEEHA_EEPROM_START, FEE_stWorkingPage.s16WorkingDataCount);
					boCopyOK = TRUE;
				}
				else
				{
					boCopyOK = FALSE;
				}
			}
			else
			{
				memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
					(void*)FEEHA_EEPROM_START, FEE_stWorkingPage.s16WorkingDataCount);
				boCopyOK = TRUE;
			}	
		}	
		else
		/* Copy working page to NVM page */
		{
			if (TRUE == boCheckCRC16MakeCRC16)
			{
				pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, FEE_stWorkingPage.pu8WorkingData, 
					FEE_stWorkingPage.s16WorkingDataCount - 2));
				
				pu16CRC16Stored = (puint16)((uint32)FEE_stWorkingPage.pu8WorkingData + 
					(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);
				
				/* Copy the computed CRC into the working page */
				*pu16CRC16Stored = *pu16CRC16Computed;
			}
			
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
			{
				pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
			}
			
			u32NVMWordCount = FEE_stWorkingPage.s16WorkingDataCount / 4;
			u32NVMWordCount = (0 == FEE_stWorkingPage.s16WorkingDataCount % 4) ?
										u32NVMWordCount : u32NVMWordCount + 1;			
			boCopyOK = TRUE;
			
			for (u32NVMWordIDX = 0; u32NVMWordIDX < u32NVMWordCount; u32NVMWordIDX++)
			{				
				u32SourceWord = (uint32)FEE_stWorkingPage.pu8WorkingData + 4 * u32NVMWordIDX;
				u32DestinationWord = (uint32)FEEHA_EEPROM_START + 4 * u32NVMWordIDX;
				
				if (*(puint32)u32SourceWord != *(puint32)u32DestinationWord)
				/* Only call write if words are different */
				{
					if (FTFx_OK == u32RetCode)
					{
						u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
													u32DestinationWord,	4, u32SourceWord);
					}
					else
					{
						boCopyOK = false;
					}
				}
			}
		}			
	}
#endif //BUILD_MK60

#if defined(BUILD_MK64)
	tstFTFEModule* pstFTFE;
	uint32 u32RetCode = FTFx_OK;
    pstFTFE = FTFE;
	uint32 u32NVMSectorCount;
	uint32 u32NVMSectorIDX;
	uint32 u32SourceWord;
	uint32 u32DestinationWord;
	uint8* pu8PartitionBase;

	if ((0 < FEE_stWorkingPage.s16WorkingDataCount) &&
		(NULL != FEE_stWorkingPage.pu8WorkingData) &&
		(0 == FEE_u32InhibitNVMCount))
	{
		if (TRUE == boNVMToWorking)
		/* Copy NVM page to working page */
		{
			uint8* pu8PartitionBase = FEEHA_pu8GetPartitionAddress();

			if (((uint8*)~0) != pu8PartitionBase)
			{
				pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)pu8PartitionBase,
					FEE_stWorkingPage.s16WorkingDataCount - 2));

				pu16CRC16Stored = (puint16)((uint32)pu8PartitionBase +
					(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);

				if (TRUE == boCheckCRC16MakeCRC16)
				{
					if (*pu16CRC16Stored == *pu16CRC16Computed)
					{
						memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
							(void*)pu8PartitionBase, FEE_stWorkingPage.s16WorkingDataCount);
						boCopyOK = TRUE;
					}
					else
					{
						boCopyOK = FALSE;
					}
				}
				else
				{
					memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
						(void*)FEEHA_EEPROM_START, FEE_stWorkingPage.s16WorkingDataCount);
					boCopyOK = TRUE;
				}
			}
		}
		else
		/* Copy working page to NVM page */
		{
			/* No copy for another 10 seconds */
			FEE_u32InhibitNVMCount = 10000;

			if (TRUE == boCheckCRC16MakeCRC16)
			{
				pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, FEE_stWorkingPage.pu8WorkingData,
					FEE_stWorkingPage.s16WorkingDataCount - 2));

				pu16CRC16Stored = (puint16)((uint32)FEE_stWorkingPage.pu8WorkingData +
					(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);

				/* Copy the computed CRC into the working page */
				*pu16CRC16Stored = *pu16CRC16Computed;
			}

			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
			{
				pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
			}

			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_FPVIOL_MASK) == FTFE_FSTAT_FPVIOL_MASK)
			{
				pstFTFE -> FSTAT &= FTFE_FSTAT_FPVIOL_MASK;
			}

			u32NVMSectorCount = FEE_stWorkingPage.s16WorkingDataCount / FEEHA_EEPROM_SCTR_BYTES;
			u32NVMSectorCount = (0 == FEE_stWorkingPage.s16WorkingDataCount % FEEHA_EEPROM_SCTR_BYTES) ?
										u32NVMSectorCount : u32NVMSectorCount + 1;
			boCopyOK = TRUE;

			pu8PartitionBase = FEEHA_pu8GetFreePartitionAddress();

			if (((uint8*)~0) != pu8PartitionBase)
			{
				for (u32NVMSectorIDX = 0; u32NVMSectorIDX < u32NVMSectorCount; u32NVMSectorIDX++)
				{
					u32SourceWord = (uint32)FEE_stWorkingPage.pu8WorkingData + FEEHA_EEPROM_SCTR_BYTES * u32NVMSectorIDX;
					u32DestinationWord = (uint32)pu8PartitionBase + FEEHA_EEPROM_SCTR_BYTES * u32NVMSectorIDX;

					if (FTFx_OK == u32RetCode)
					{
						u32RetCode = FlashProgramPhrase(&FEE_stFlashSSDConfig,
													u32DestinationWord,	FEEHA_EEPROM_SCTR_BYTES, u32SourceWord, &FlashCommandSequence);
					}
					else
					{
						boCopyOK = false;
					}

				}
			}
		}
	}
#endif //BUILD_MK60

#if defined(BUILD_MKS20)
	tstFTFEModule* pstFTFE;
	uint32 u32RetCode = FTFx_OK;
    pstFTFE = FTFMODULE;
	uint32 u32NVMSectorCount;
	uint32 u32NVMSectorIDX;
	uint32 u32SourceWord;
	uint32 u32DestinationWord;
	uint8* pu8PartitionBase;

	if ((0 < FEE_stWorkingPage.s16WorkingDataCount) &&
		(NULL != FEE_stWorkingPage.pu8WorkingData) &&
		(0 == FEE_u32InhibitNVMCount))
	{
		if (TRUE == boNVMToWorking)
		/* Copy NVM page to working page */
		{
			uint8* pu8PartitionBase = FEEHA_pu8GetPartitionAddress();

			if (((uint8*)~0) != pu8PartitionBase)
			{
				pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)pu8PartitionBase,
					FEE_stWorkingPage.s16WorkingDataCount - 2));

				pu16CRC16Stored = (puint16)((uint32)pu8PartitionBase +
					(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);

				if (TRUE == boCheckCRC16MakeCRC16)
				{
					if (*pu16CRC16Stored == *pu16CRC16Computed)
					{
						memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
							(void*)pu8PartitionBase, FEE_stWorkingPage.s16WorkingDataCount);
						boCopyOK = TRUE;
					}
					else
					{
						boCopyOK = FALSE;
					}
				}
				else
				{
					//memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
					//	(void*)FEEHA_EEPROM_START, FEE_stWorkingPage.s16WorkingDataCount);
					//boCopyOK = TRUE;
				}
			}
		}
		else
		/* Copy working page to NVM page */
		{
			if (TRUE == boCheckCRC16MakeCRC16)
			{
				/* No copy for another 20 seconds */
				FEE_u32InhibitNVMCount = 20000;

				pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, FEE_stWorkingPage.pu8WorkingData,
					FEE_stWorkingPage.s16WorkingDataCount - 2));

				pu16CRC16Stored = (puint16)((uint32)FEE_stWorkingPage.pu8WorkingData +
					(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);

				/* Copy the computed CRC into the working page */
				*pu16CRC16Stored = *pu16CRC16Computed;
			}
			else
			{
				/* No copy for another 30 seconds */
				FEE_u32InhibitNVMCount = 30000;
			}

			if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_ACCERR_MASK)) == FTFPREFIX(_FSTAT_ACCERR_MASK))
			{
				pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_ACCERR_MASK);
			}

			if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_FPVIOL_MASK)) == FTFPREFIX(_FSTAT_FPVIOL_MASK))
			{
				pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_FPVIOL_MASK);
			}

			u32NVMSectorCount = FEE_stWorkingPage.s16WorkingDataCount / FEEHA_PFLASH_SCTR_BYTES;
			u32NVMSectorCount = (0 == FEE_stWorkingPage.s16WorkingDataCount % FEEHA_PFLASH_SCTR_BYTES) ?
										u32NVMSectorCount : u32NVMSectorCount + 1;
			boCopyOK = TRUE;

			if (FEEHA_PFLASH_END < u32AddressOverride)
			{
				pu8PartitionBase = FEEHA_pu8GetFreePartitionAddress();
			}
			else
			{
				pu8PartitionBase = (uint8*)u32AddressOverride;
			}

			if ((((uint8*)~0) != pu8PartitionBase) && (0xff == *pu8PartitionBase))
			{
				for (u32NVMSectorIDX = 0; u32NVMSectorIDX < u32NVMSectorCount; u32NVMSectorIDX++)
				{
					u32SourceWord = (uint32)FEE_stWorkingPage.pu8WorkingData + FEEHA_PFLASH_SCTR_BYTES * u32NVMSectorIDX;
					u32DestinationWord = (uint32)pu8PartitionBase + FEEHA_PFLASH_SCTR_BYTES * u32NVMSectorIDX;

					if (FTFx_OK == u32RetCode)
					{
						u32RetCode = FlashProgramLongword(&FEE_stFlashSSDConfig,
													u32DestinationWord,	FEEHA_PFLASH_SCTR_BYTES, u32SourceWord, &FlashCommandSequence);
					}
					else
					{
						boCopyOK = false;
						break;
					}

				}
			}
		}
	}
#endif //BUILD_MKS20


#ifdef BUILD_SAM3X8E
	if (TRUE == boNVMToWorking)
	/* Copy NVM page to working page */
	{
		if (TRUE == boCheckCRC16MakeCRC16)
		{
			pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)FEEHA_EEPROM_START,
			FEE_stWorkingPage.s16WorkingDataCount - 2));
			
			pu16CRC16Stored = (puint16)((uint32)FEEHA_EEPROM_START +
			(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);
				
			if ((NULL != FEE_stWorkingPage.pu8WorkingData) && (*pu16CRC16Stored == *pu16CRC16Computed))
			{
				memcpy(FEE_stWorkingPage.pu8WorkingData, 0x000c0000, FEE_stWorkingPage.s16WorkingDataCount);
				boCopyOK = true;
			}
		}
	}
	else
	/* Copy working page to NVM page */
	{
		if ((NULL != FEE_stWorkingPage.pu8WorkingData) && (10000 < (FEE_u32RunCount - u32LastWriteCount)))
		{
			pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, FEE_stWorkingPage.pu8WorkingData,
			FEE_stWorkingPage.s16WorkingDataCount - 2));
			
			pu16CRC16Stored = (puint16)((uint32)FEE_stWorkingPage.pu8WorkingData +
			(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);

			/* Copy the computed CRC into the working page */
			*pu16CRC16Stored = *pu16CRC16Computed;

			uint32_t u32LoopDelay = 0x1800000;

			TEPM_vEnableSequences(FALSE);
			IRQ_vEnableRTOS(FALSE);

			while (0 != u32LoopDelay)
			{
				u32LoopDelay--;
			}

			/* Erase flash */
			flash_erase_all(0x00c0000);

			/* Write working page to flash */
			flash_write(0x00c0000, FEE_stWorkingPage.pu8WorkingData, FEE_stWorkingPage.s16WorkingDataCount , 1u);
			u32LastWriteCount = FEE_u32RunCount;

			u32LoopDelay = ~0;

			while (0 != u32LoopDelay)
			{
				u32LoopDelay--;
			}
		}
	}
#endif //BUILD_SAM3X8E

	return boCopyOK;
}

bool FEEHA_boWriteNVM(puint8 pu8SourceData, puint8 pu8DestData, uint32 u32DataByteCount)
{
	tstFTFEModule* pstFTFE;	
	bool boCopyOK = TRUE;		
	uint32 u32SourceWord = (uint32)pu8SourceData;
	uint32 u32DestinationWord = (uint32)pu8DestData;	
	uint32 u32RetCode;


#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
    pstFTFE = FTFMODULE;
	u32RetCode = FTFx_OK;

	if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_ACCERR_MASK)) == FTFPREFIX(_FSTAT_ACCERR_MASK))
	{
		pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_ACCERR_MASK);
	}	
	
	if (TRUE == FEEHA_boGetSem(TRUE))
	{
		while (4 < u32DataByteCount)
		{
			if (*(puint32)u32SourceWord != *(puint32)u32DestinationWord)
			/* Only call write if words are different */
			{
				if (FTFx_OK == u32RetCode)
				{
					u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
												u32DestinationWord,	4, u32SourceWord);
				}
				else
				{
					boCopyOK = false;
				}
			}
			
			u32DestinationWord += sizeof(uint32);
			u32SourceWord += sizeof(uint32);
			u32DataByteCount -= sizeof(uint32);		
		}
		
		if (0 < u32DataByteCount)
		{
			if (0 != memcmp((void*)u32SourceWord, (void*)u32DestinationWord, u32DataByteCount))
			/* Only call write if bytes are different */
			{
				if (FTFx_OK == u32RetCode)
				{
					u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
												u32DestinationWord,	u32DataByteCount, u32SourceWord);
				}
				else
				{
					boCopyOK = FALSE;
				}
			}
		}

		FEEHA_boGetSem(FALSE);
	}
	else
	{
		boCopyOK = FALSE;
	}
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	/* Write working page to flash */
	u32RetCode = flash_write(0x00c0000 + (uint32)pu8DestData, (uint32)pu8SourceData, u32DataByteCount, 0u);

	boCopyOK = 0 == u32RetCode;
#endif
	
	return boCopyOK;
}	


bool FEEHA_boNVMClear(void)
{
	bool boClearOK = false;

#if defined(BUILD_MK60)
	tstFTFEModule* pstFTFE;		

	uint32 u32NVMWordCount;
	uint32 u32NVMWordIDX;
	uint32 u32SourceWord = 0;
	uint32 u32DestinationWord;
	uint32 u32RetCode;
    u32RetCode = FTFx_OK;

	boClearOK = FEE_boCheckPartition();

	if ((TRUE == boClearOK) &&
			(0 < FEE_stWorkingPage.s16WorkingDataCount) &&
			(NULL != FEE_stWorkingPage.pu8WorkingData))
	{
		/* Clear NVM page */
		{
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
			{
				pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
			}

			u32NVMWordCount = FEE_stWorkingPage.s16WorkingDataCount / 4;
			u32NVMWordCount = (0 == FEE_stWorkingPage.s16WorkingDataCount % 4) ?
										u32NVMWordCount : u32NVMWordCount + 1;
			boClearOK = TRUE;

			for (u32NVMWordIDX = 0; u32NVMWordIDX < u32NVMWordCount; u32NVMWordIDX++)
			{
				u32DestinationWord = (uint32)FEEHA_EEPROM_START + 4 * u32NVMWordIDX;

				if (FTFx_OK == u32RetCode)
				{
					u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig,
												u32DestinationWord,	4, (uint32)&u32SourceWord);
				}
				else
				{
					boClearOK = false;
				}
			}
		}
	}
#endif //BUILD_MK60

#if defined(BUILD_MK64)
	bool boEraseErr = TRUE;

	if (0 == FEE_u32InhibitNVMCount)
	{
		boEraseErr = FEEHA_boEraseForDownload((uint8*)FEEHA_DFLASH_START, FEEHA_DFLASH_END - FEEHA_DFLASH_START + 1);
	}

	boClearOK = false == boEraseErr ? TRUE : FALSE;
#endif //BUILD_MK64

#if defined(BUILD_MKS20)
	bool boEraseErr = TRUE;

	if (0 == FEE_u32InhibitNVMCount)
	{
		boEraseErr = FEEHA_boEraseForDownload((uint8*)FEEHA_FLASH_NVM_RECS, FEEHA_PFLASH_END - FEEHA_FLASH_NVM_RECS + 1);
	}

	boClearOK = false == boEraseErr ? TRUE : FALSE;
#endif //BUILD_MK64

	return boClearOK;
}

bool FEEHA_boCheckUpdaterCRC16(void)
{
	uint16* pu16CRC16Computed;
	uint16* pu16CRC16Stored;

	pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)FEEHA_FLASH_NVM_RECS,
			FEEHA_PFLASH_END - FEEHA_FLASH_NVM_RECS - 1));

	pu16CRC16Stored = (puint16)(FEEHA_PFLASH_END - 1);

	return (*pu16CRC16Computed == *pu16CRC16Stored);
}

bool FEEHA_boEraseForDownload(puint8 pu8TargetAddress, uint32 u32EraseCount)
{
	tstFTFEModule* pstFTFE;
	uint32 u32ReturnCode;
	uint16 u16WordCount;
	uint16 u16SectorEraseCount;
	uint32* pu32SectorWord;	
	puint32 pu32TargetAddress;
	bool boEraseErr = false;
	FEE_enPgmErrCode = enErrNone;
	
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)

	pstFTFE = FTFMODULE;

	if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_CCIF_MASK)) == FTFPREFIX(_FSTAT_CCIF_MASK))
	{
		if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_ACCERR_MASK)) == FTFPREFIX(_FSTAT_ACCERR_MASK))
		{
			pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_ACCERR_MASK);
		}

		if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_FPVIOL_MASK)) == FTFPREFIX(_FSTAT_FPVIOL_MASK))
		{
			pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_FPVIOL_MASK);
		}	

		if (0 == (uint32)pu8TargetAddress % 4)
		{
			pu32TargetAddress = (puint32)(uint32)pu8TargetAddress;
		}
		else
		{
			pu32TargetAddress = (puint32)0xffffffff;
		}		
			
		/* PFLASH erase */
#if (0 < FEE_PFLASH_START)		
		if (((uint32)pu32TargetAddress >= FEE_PFLASH_START) &&
				(((uint32)pu32TargetAddress + u32EraseCount <= FEE_PFLASH_END)))
#else
		if ((uint32)pu32TargetAddress + u32EraseCount <= (FEEHA_PFLASH_END + 1))
#endif		
		{
			u16WordCount = u32EraseCount / FEEHA_PFLASH_BYTES_WORD;
			
			if (((uint32)pu32TargetAddress % FEEHA_PFLASH_BYTES_WORD) != 0)
			{	
				/* abort on non-word boundary */
				boEraseErr = true;
			}				
			else
			{
				/* calculate target sector count */
				if (u16WordCount > FEEHA_PFLASH_SCTR_WORDS)
				{
					if ((u16WordCount % FEEHA_PFLASH_SCTR_WORDS) == 0)
					{
						u16SectorEraseCount = u16WordCount / FEEHA_PFLASH_SCTR_WORDS;
					}
					else
					{
						u16SectorEraseCount = u16WordCount / FEEHA_PFLASH_SCTR_WORDS + 1;
					}
				}
				else
				{
					u16SectorEraseCount = 1;
				}
										
				pu32SectorWord = (uint32*)((uint32)pu32TargetAddress - ((uint32)pu32TargetAddress % FEEHA_PFLASH_SCTR_WORDS));
				
				while(u16SectorEraseCount-- > 0)
				/* erase any target sectors that are not erased */
				{
					u32ReturnCode = FlashVerifySection(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, FEEHA_PFLASH_SCTR_WORDS, enMarginNormal, pFlashCommandSequence);
					
					if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_MGSTAT0_MASK)) == FTFPREFIX(_FSTAT_MGSTAT0_MASK))
					/* erase sector not blank */
					{
						u32ReturnCode = FlashEraseSector(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, FTFx_PSECTOR_SIZE, pFlashCommandSequence);
						boEraseErr = 0 != u32ReturnCode	? true : boEraseErr;	
					}
					
					/* update the sector base pointer to next sector */
					pu32SectorWord = (uint32*)((uint32)pu32SectorWord + FEEHA_PFLASH_SCTR_BYTES);
				}	
			}
		}
#if defined(BUILD_MK20) || defined(BUILD_MK64)
		/* DFLASH erase */
		else if (((uint32)pu32TargetAddress >= FEEHA_DFLASH_START) &&
				(((uint32)pu32TargetAddress + u32EraseCount <= (FEEHA_DFLASH_END + 1))))
		{
			if ((FEEHA_DFLASH_END - FEEHA_DFLASH_START + 1) == u32EraseCount)
			{
				u32ReturnCode = FlashEraseBlock (&FEE_stFlashSSDConfig,	(uint32)pu8TargetAddress, pFlashCommandSequence);
				boEraseErr = 0 == u32ReturnCode ? FALSE : TRUE;
			}
		}
		else
		{
			boEraseErr = true;
		}
#endif //defined(BUILD_MK64) || defined(BUILD_MK60)
	}

#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E


#endif //BUILD_SAM3X8E
	
	return boEraseErr;
}

static bool FEEHA_boPartitionDFlash(void)
{
	tstFTFEModule* pstFTFE;
	uint32* pu32SectorWord;

	bool boWriteErr = false;
	
	FEE_enPgmErrCode = enErrNone;

#if defined(BUILD_MK60) || defined(BUILD_MK64)
	pstFTFE = FTFE;
	
	if (((pstFTFE -> FSTAT) & FTFE_FSTAT_CCIF_MASK) == FTFE_FSTAT_CCIF_MASK)
	{
		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
		}

		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_FPVIOL_MASK) == 0)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_FPVIOL_MASK;			
		}					
			
		if (((pstFTFE -> FCNFG) & FTFE_FCNFG_EEERDY_MASK) != FTFE_FCNFG_EEERDY_MASK)
		/* If DFlash not already partitioned */
		{		
			pu32SectorWord = (uint32*)FEEHA_DFLASH_START;
			returnCode = FlashVerifyBlock(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, enMarginNormal, pFlashCommandSequence);
					
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_MGSTAT0_MASK) == FTFE_FSTAT_MGSTAT0_MASK)
			/* Erase sector not blank */
			{
				FEE_boEraseForDownload((puint8)FEEHA_DFLASH_START, FEEHA_DFLASH_END - FEEHA_DFLASH_START + 1);
			}
							
			/* Partition DFlash */
			returnCode = pDEFlashPartition(&FEE_stFlashSSDConfig, 0x00, 0x08, pFlashCommandSequence);

			while(FEEHA_xWaitCmdToComplete()) {}
				
			//pSetEEEEnable(&FEE_stFlashSSDConfig, 0x00);
		}
	}	
#endif //BUILD_MK6X
	
	return boWriteErr;
}

bool FEEHA_boWriteSector(void)
{
	tstFTFEModule* pstFTFE;
	uint32* pu32ProgramWord;
	uint32* pu32SourceWord;
	uint32* pu32SectorWord;
	uint16 u16WordCount;
	uint16 u16ByteCount;	
	bool boWriteErr = false;
	
	FEE_enPgmErrCode = enErrNone;
	
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	pstFTFE = FTFMODULE;
	if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_CCIF_MASK)) == FTFPREFIX(_FSTAT_CCIF_MASK))
	{
		if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_ACCERR_MASK)) == FTFPREFIX(_FSTAT_ACCERR_MASK))
		{
			pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_ACCERR_MASK);
		}

		if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_FPVIOL_MASK)) == 0)
		{
			pstFTFE -> FSTAT &= FTFPREFIX(_FSTAT_FPVIOL_MASK);
		}		
			
		/* PFLASH program */
#if (0 < FEE_PFLASH_START)		
		if (((uint32)FEE_stWriteControlBlock.pu8TargetAddress >= FEE_PFLASH_START) &&
				(((uint32)FEE_stWriteControlBlock.pu8TargetAddress +
					(FEE_PFLASH_SCTR_BYTES - 1) <= (uint32)FEE_PFLASH_END)))
#else
		if (((uint32)FEE_stWriteControlBlock.pu8TargetAddress +
					(FEEHA_PFLASH_SCTR_BYTES - 1) <= (uint32)FEEHA_PFLASH_END))
#endif
		{
			if (FEEHA_PFLASH_SCTR_BYTES < FEE_stWriteControlBlock.u32WriteCount)
			{
				u16WordCount = FEEHA_PFLASH_SCTR_BYTES / FEEHA_PFLASH_BYTES_WORD;
				u16ByteCount = FEEHA_PFLASH_SCTR_BYTES;
			}
			else
			{
				if (0 == (FEE_stWriteControlBlock.u32WriteCount % FEEHA_PFLASH_BYTES_WORD))
				{
					u16WordCount = FEE_stWriteControlBlock.u32WriteCount / FEEHA_PFLASH_BYTES_WORD;
				}
				else
				{
					u16WordCount = (FEE_stWriteControlBlock.u32WriteCount / FEEHA_PFLASH_BYTES_WORD) + 1;
				}
				
				u16ByteCount = u16WordCount * FEEHA_PFLASH_BYTES_WORD;
			}
			
			pu32ProgramWord = (uint32*)FEE_stWriteControlBlock.pu8TargetAddress;
			pu32SourceWord = (uint32*)&FEE_au8ProgBuff[0];
			
			if (((uint32)pu32ProgramWord % FEEHA_PFLASH_BYTES_WORD) != 0)
			{	
				/* abort on non-word boundary */
				u16WordCount = 0;
			}
			
			pu32SectorWord = (uint32*)((uint32)pu32ProgramWord - ((uint32)pu32ProgramWord % FEEHA_PFLASH_SCTR_WORDS));
			returnCode = FlashVerifySection(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, u16WordCount, enMarginNormal, pFlashCommandSequence);
				
			if (((pstFTFE -> FSTAT) & FTFPREFIX(_FSTAT_MGSTAT0_MASK)) == FTFPREFIX(_FSTAT_MGSTAT0_MASK))
			/* erase sector not blank */
			{
				boWriteErr = true;
				u16WordCount = 0;
				u8WriteCount = 0x34;
			}
						
			/* program PFLASH words */
			if (u16WordCount > 0)
			{
				/* copy large diag buffer to RAM accelaration buffer */
				memcpy((void*)EERAM_BLOCK_BASE, (void*)pu32SourceWord, (uint32)u16ByteCount);

				/* program the values to PFLASH */
				if (0 == (u16ByteCount % FEEHA_PFLASH_BYTES_PHRS))
				{
					phraseNumber = u16ByteCount / FEEHA_PFLASH_BYTES_PHRS;
				}
				else
				{
					phraseNumber = u16ByteCount / FEEHA_PFLASH_BYTES_PHRS + 1;
				}
				
				destination = (uint32)pu32ProgramWord;
				returnCode = pFlashProgramSection(&FEE_stFlashSSDConfig, destination, phraseNumber, pFlashCommandSequence);
				if (FTFx_OK != returnCode)
				{
						u16WordCount = 0;
				}
				
				u8WriteCount++;

				/* update write address and decrement write count */
				FEE_stWriteControlBlock.pu8TargetAddress += FEEHA_PFLASH_SCTR_BYTES;
				
				if (FEEHA_PFLASH_SCTR_BYTES < FEE_stWriteControlBlock.u32WriteCount)
				{
					FEE_stWriteControlBlock.u32WriteCount -= FEEHA_PFLASH_SCTR_BYTES;
				}
				else
				{
					FEE_stWriteControlBlock.u32WriteCount = 0;
					FEE_stWriteControlBlock.boProgramming = false;				
				}
			}
		}
		else
		{
			boWriteErr = true;
			u8WriteCount = 0x55;
		}
		
		while(FEEHA_xWaitCmdToComplete()) {}
	}	
#endif //BUILD_MK6X
	
	return boWriteErr;
}


bool FEEHA_boStart(uint32* const u32Stat)
{
    bool boStartOK = false;

#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	u8WriteCount = 0;
	unsecure_key = 0xFFFFFFFE;

	REGSET_vInitReg8(&FEEHA_rastFTFEReg8Val[0]);		
	FEEHA_vInitFlashConfig();

	returnCode = pFlashInit(&FEE_stFlashSSDConfig);
	
	if (FTFx_OK == returnCode)
	{
		FEE_stWriteControlBlock.boProgramming = false;
		FEE_stWriteControlBlock.boProgErr = false;
		boStartOK = true;
	}
#endif  //BUILD_MK6X

#ifdef BUILD_SAM3X8E
    uint32 u32Err;

    u32Err = flash_init(128u, 10u);

	if (FLASH_RC_OK == u32Err)
	{
		FEE_stWriteControlBlock.boProgramming = false;
		FEE_stWriteControlBlock.boProgErr = false;
		boStartOK = true;
	}
#endif

    return boStartOK;
}

void FEEHA_vRun(uint32* const u32Stat)
{	
	FEE_u32RunCount++;

	if (0 < FEE_u32InhibitNVMCount)
	{
		FEE_u32InhibitNVMCount--;
	}
}

void FEEHA_vTerminate(uint32* const u32Stat)
{

}

bool FEEHA_boPartition(void)
{
	bool boRetVal;
	
	boRetVal = FEEHA_boPartitionDFlash();
	
	return boRetVal;
}

bool FEEHA_boWriteControlBlock(COMMONNL_tstRXLargeBuffer* const pstParamSourceBuffer,
															 uint8* const pu8ParamTargetAddress, 
															 uint32 u32ParamWriteCount)
{
	if ((false == FEE_stWriteControlBlock.boProgramming) &&
			(false == FEE_stWriteControlBlock.boProgErr))
	{
		FEE_stWriteControlBlock.pstSourceBuffer = pstParamSourceBuffer;
		FEE_stWriteControlBlock.pu8TargetAddress = pu8ParamTargetAddress;
		FEE_stWriteControlBlock.u32WriteCount = u32ParamWriteCount;
		FEE_stWriteControlBlock.u32AccumulateCount = 0;		
		FEE_stWriteControlBlock.boProgramming = true;
	}
	else
	{
		FEE_stWriteControlBlock.boProgramming = false;
	}
	
	return FEE_stWriteControlBlock.boProgramming;
}

bool FEEHA_boUpdateControlBlock(uint32 u32BlockWriteCount)
{		
	puint8 pu8SourceData;
	puint8 pu8BufferData;
	
	if (FEE_stWriteControlBlock.boProgramming == true)
	{
		if ((FEEHA_PFLASH_SCTR_BYTES >=
				(u32BlockWriteCount + FEE_stWriteControlBlock.u32AccumulateCount)) &&
				(0 != u32BlockWriteCount))
		{			
			pu8SourceData = (puint8)&FEE_stWriteControlBlock.pstSourceBuffer->u8Data[0];
			pu8BufferData = (puint8)((uint32)&FEE_au8ProgBuff + FEE_stWriteControlBlock.u32AccumulateCount);
			
			memcpy((void*)pu8BufferData, (void*)pu8SourceData, u32BlockWriteCount);				
		
			FEE_stWriteControlBlock.u32AccumulateCount += u32BlockWriteCount;
			
			if (FEEHA_PFLASH_SCTR_BYTES == FEE_stWriteControlBlock.u32AccumulateCount)
			{
				FEE_stWriteControlBlock.u32AccumulateCount = 0;
				/* TODO FEE_boWriteSector(); */
			}
			else if (FEEHA_PFLASH_SCTR_BYTES < FEE_stWriteControlBlock.u32AccumulateCount)
			{
				/* Uh oh too many bytes received - ran past the sector size */
				/* matthew deal with this */
				FEE_stWriteControlBlock.boProgramming = false;	
			}					
			else if (FEE_stWriteControlBlock.u32WriteCount <= FEE_stWriteControlBlock.u32AccumulateCount)
			{
				FEE_stWriteControlBlock.u32AccumulateCount = 0;
				/* TODO FEE_boWriteSector(); */
			}
		}
		else
		{
			FEE_stWriteControlBlock.boProgramming = false;
		}
	}
	
	return FEE_stWriteControlBlock.boProgramming;	
}


static bool FEEHA_boGetSem(bool boGet)
{
	static bool FEE_boSem = false;
	bool boRetVal = false;
	
	if (TRUE == boGet)
	{
		if (false == FEE_boSem)
		{
			FEE_boSem = TRUE;
			boRetVal = TRUE;
		}
	}
	else
	{
		FEE_boSem = FALSE;
		boRetVal = TRUE;		
	}
	
	return boRetVal;
}

static void FEEHA_vInitFlashConfig(void)
{
#if defined(BUILD_MK60) || defined(BUILD_MK64) || defined(BUILD_MKS20)
	FEE_stFlashSSDConfig.ftfxRegBase = FTFx_REG_BASE;
	FEE_stFlashSSDConfig.PFlashBlockBase = PFLASH_BLOCK_BASE;
	FEE_stFlashSSDConfig.PFlashBlockSize = PBLOCK_SIZE;
	FEE_stFlashSSDConfig.DFlashBlockBase = DEFLASH_BLOCK_BASE;
	FEE_stFlashSSDConfig.DFlashBlockSize = 0;
	FEE_stFlashSSDConfig.EERAMBlockBase = EERAM_BLOCK_BASE;
	FEE_stFlashSSDConfig.EERAMBlockSize = EERAM_BLOCK_SIZE;
	FEE_stFlashSSDConfig.EEEBlockSize = 0;
	FEE_stFlashSSDConfig.DebugEnable = DEBUGENABLE;
	FEE_stFlashSSDConfig.CallBack = NULL_CALLBACK;
#endif //BUILD_MK6X
}

static uint8* FEEHA_pu8GetPartitionAddress(void)
{
	uint8* pu8NVRecordBase = (uint8*)~0;
#if defined(BUILD_MK64)
    pu8NVRecordBase = (uint8*)(FEEHA_DFLASH_END - FEEHA_WORK_DATA_MAX + 1);

    while (((uint8*)FEEHA_DFLASH_START) <= pu8NVRecordBase)
    {
    	if (0xff != *pu8NVRecordBase) //caveat!!!!
    	{
    		break;
    	}

    	pu8NVRecordBase -= FEEHA_WORK_DATA_MAX;
    }
#endif //BUILD_MK64

#if defined(BUILD_MKS20)
    pu8NVRecordBase = (uint8*)(FEEHA_PFLASH_END - FEEHA_WORK_DATA_MAX + 1);

    while (((uint8*)FEEHA_PFLASH_START) <= pu8NVRecordBase)
    {
    	if (0xff != *pu8NVRecordBase) //caveat!!!!
    	{
    		break;
    	}

    	pu8NVRecordBase -= FEEHA_WORK_DATA_MAX;
    }
#endif //BUILD_MKS20


	return pu8NVRecordBase;
}

static uint8* FEEHA_pu8GetFreePartitionAddress(void)
{
	uint8* pu8NVFreeBase;
	uint8* pu8RetVal = (uint8*)~0;
#if defined(BUILD_MK64)
    pu8NVFreeBase = (uint8*)FEEHA_DFLASH_START;
    bool boEraseErr;

    while (((uint8*)FEEHA_DFLASH_END) > pu8NVFreeBase)
    {
    	if (0xff == *pu8NVFreeBase) //caveat!!!!
    	{
    		pu8RetVal = pu8NVFreeBase;
    		break;
    	}

    	pu8NVFreeBase += FEEHA_WORK_DATA_MAX;
    }


    if (((uint8*)~0) == pu8RetVal)
    {
    	boEraseErr = FEEHA_boEraseForDownload((uint8*)FEEHA_DFLASH_START, FEEHA_DFLASH_END - FEEHA_DFLASH_START + 1);

    	if (FALSE == boEraseErr)
    	{
    		pu8RetVal = (uint8*)FEEHA_DFLASH_START;
    	}
    }
#endif //BUILD_MK64

#if defined(BUILD_MKS20)
    pu8NVFreeBase = (uint8*)FEEHA_FLASH_NVM_RECS;
    bool boEraseErr;

    while (((uint8*)FEEHA_PFLASH_END) > pu8NVFreeBase)
    {
    	if (0xff == *pu8NVFreeBase) //caveat!!!!
    	{
    		pu8RetVal = pu8NVFreeBase;
    		break;
    	}

    	pu8NVFreeBase += FEEHA_WORK_DATA_MAX;
    }


    if (((uint8*)~0) == pu8RetVal)
    {
    	boEraseErr = FEEHA_boEraseForDownload((uint8*)FEEHA_FLASH_NVM_RECS, FEEHA_PFLASH_END - FEEHA_FLASH_NVM_RECS + 1);

    	if (FALSE == boEraseErr)
    	{
    		pu8RetVal = (uint8*)FEEHA_FLASH_NVM_RECS;
    	}
    }
#endif //BUILD_MKS20

	return pu8RetVal;
}


