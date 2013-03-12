/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support  -  ROUSSET  -
 * ----------------------------------------------------------------------------
 * Copyright (c) 2006, Atmel Corporation

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the disclaimer below in the documentation and/or
 * other materials provided with the distribution.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 * File Name           : nandflash.c
 * Object              :
 * Creation            : NLe Sep 28th 2006
 *-----------------------------------------------------------------------------
 */
#include "../include/part.h"
#include "../include/main.h"
#include "../include/debug.h"

#ifdef CFG_NANDFLASH

#include "../include/nandflash.h"
#include "../include/nand_ids.h"

/*----------------------------------------------------------------------------*/
/* NAND Commands							      */
/*----------------------------------------------------------------------------*/
/* 8 bits devices */
#define WRITE_NAND_COMMAND(d) do{ *(volatile unsigned char *)((unsigned long)AT91C_SMARTMEDIA_BASE | AT91_SMART_MEDIA_CLE) = (unsigned char)(d); } while(0)
#define WRITE_NAND_ADDRESS(d) do{ *(volatile unsigned char *)((unsigned long)AT91C_SMARTMEDIA_BASE | AT91_SMART_MEDIA_ALE) = (unsigned char)(d); } while(0)
#define WRITE_NAND(d) do{ *(volatile unsigned char *)((unsigned long)AT91C_SMARTMEDIA_BASE) = (unsigned char)d; } while(0)
#define READ_NAND() ((unsigned char)(*(volatile unsigned char *)(unsigned long)AT91C_SMARTMEDIA_BASE))

/* 16 bits devices */
#define WRITE_NAND_COMMAND16(d) do{ *(volatile unsigned short *)((unsigned long)AT91C_SMARTMEDIA_BASE | AT91_SMART_MEDIA_CLE) = (unsigned short)(d); } while(0)
#define WRITE_NAND_ADDRESS16(d) do{ *(volatile unsigned short *)((unsigned long)AT91C_SMARTMEDIA_BASE | AT91_SMART_MEDIA_ALE) = (unsigned short)(d); } while(0)

#define WRITE_NAND16(d) do{ *(volatile unsigned short *)((unsigned long)AT91C_SMARTMEDIA_BASE) = (unsigned short)d; } while(0)
#define READ_NAND16() ((volatile unsigned short)(*(volatile unsigned short *)(unsigned long)AT91C_SMARTMEDIA_BASE))

/*------------------------------------------------------------------------------*/
/* \fn    AT91F_NandInit							*/
/* \brief Initialize NandFlash informations					*/
/*------------------------------------------------------------------------------*/
static void AT91F_NandInit(PSNandInfo pNandInfo, PSNandInitInfo pNandInitInfo)
{
	unsigned int uSectorSize, i=0;

	pNandInfo->uNbBlocks 	  = pNandInitInfo->uNandNbBlocks;	/* Nb of blocks in device */
	pNandInfo->uBlockNbData	  = pNandInitInfo->uNandBlockSize;	/* Nb of DataBytes in a block */
	pNandInfo->uDataNbBytes	  = pNandInitInfo->uNandSectorSize;	/* Nb of bytes in data section */
	pNandInfo->uSpareNbBytes  = pNandInitInfo->uNandSpareSize;	/* Nb of bytes in spare section */
	pNandInfo->uSectorNbBytes = pNandInfo->uDataNbBytes +
								pNandInfo->uSpareNbBytes;	/* Total nb of bytes in a sector */

	pNandInfo->uBlockNbSectors = pNandInfo->uBlockNbData / pNandInfo->uDataNbBytes;		/* Nb of sector in a block */
	pNandInfo->uBlockNbSpares = pNandInfo->uSpareNbBytes * pNandInfo->uBlockNbSectors;	/* Nb of SpareBytes in a block */
	pNandInfo->uBlockNbBytes = pNandInfo->uSectorNbBytes * pNandInfo->uBlockNbSectors;	/* Total nb of bytes in a block */

	pNandInfo->uNbSectors = pNandInfo->uBlockNbSectors * pNandInfo->uNbBlocks;	/* Total nb of sectors in device */
	pNandInfo->uNbData = pNandInfo->uBlockNbBytes * pNandInfo->uNbBlocks;		/* Nb of DataBytes in device */
	pNandInfo->uNbSpares = pNandInfo->uBlockNbSpares * pNandInfo->uNbBlocks;	/* Nb of SpareBytes in device */
	pNandInfo->uNbBytes	= pNandInfo->uNbData + pNandInfo->uNbSpares;		/* Total nb of bytes in device */

	pNandInfo->uDataBusWidth = pNandInitInfo->uNandBusWidth;			/* Data Bus Width (8/16 bits) */
	
	
	uSectorSize = pNandInfo->uDataNbBytes - 1;
	pNandInfo->uOffset = 0;

	while (uSectorSize >> i)
	{
		pNandInfo->uOffset++;
		i++;
	}

    if (pNandInfo->uDataBusWidth)
    {
        pNandInfo->uBadBlockInfoOffset = 2 * BAD_BLOCK_INFO_OFFSET;
    }
    else
    {
        pNandInfo->uBadBlockInfoOffset = BAD_BLOCK_INFO_OFFSET;
    }
}

/*------------------------------------------------------------------------------*/
/* \fn    AT91F_NandReadID							*/
/* \brief Read Nand ID								*/
/*------------------------------------------------------------------------------*/
static PSNandInitInfo AT91F_NandReadID(void)
{
	unsigned int uChipID, i=0;
	unsigned char bManufacturerID, bDeviceID;
	
	/* Enable chipset */
	NAND_ENABLE_CE();

	/* Ask the Nand its IDs */
    WRITE_NAND_COMMAND(CMD_READID);
    WRITE_NAND_ADDRESS(0x00);

    /* Read answer */
    bManufacturerID = READ_NAND();
    bDeviceID       = READ_NAND();

	/* Disable chipset before returning */
	NAND_DISABLE_CE();

	uChipID = (bManufacturerID << 8) | bDeviceID;
	
	/* Search in NandFlash_InitInfo[] */
	while (NandFlash_InitInfo[i].uNandID != 0)
	{
		if (NandFlash_InitInfo[i].uNandID == uChipID)
			return &NandFlash_InitInfo[i];	
	
		i++;
	}
	
	return 0;
}

/*------------------------------------------------------------------------------*/
/* \fn    AT91F_NandEraseBlock0							*/
/* \brief Erase Block 0								*/
/*------------------------------------------------------------------------------*/
BOOL AT91F_NandEraseBlock0(void)
{
	unsigned int uPhySecNb = 0;
	BOOL bRet = TRUE;

	/* Chip enable */
	NAND_ENABLE_CE();

	/* Push Erase_1 command */
	WRITE_NAND_COMMAND(CMD_ERASE_1);

	/* Push sector address in three cycles */
	WRITE_NAND_ADDRESS((uPhySecNb >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uPhySecNb >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uPhySecNb >> 16) & 0xFF);

	/* Push Erase_2 command */
	WRITE_NAND_COMMAND(CMD_ERASE_2);

	/* Wait for nand to be ready */
	NAND_WAIT_READY();
	NAND_WAIT_READY();
	
	/* Check status bit for error notification */
	WRITE_NAND_COMMAND(CMD_STATUS);
	NAND_WAIT_READY();
	if (READ_NAND() & STATUS_ERROR)
	{
		/* Error during block erasing */
		bRet = FALSE;
		goto exit;	
	}

exit:
	/* Chip disable */
	NAND_DISABLE_CE();

	return bRet;
}


#ifdef NANDFLASH_SMALL_BLOCKS
/*------------------------------------------------------------------------------*/
/* \fn    AT91F_NandReadSector							*/
/* \brief Read a Sector								*/
/*------------------------------------------------------------------------------*/
BOOL AT91F_NandReadSector(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pOutBuffer, unsigned int fZone)
{
	BOOL		bRet = TRUE;
	unsigned int	uBytesToRead, i;
	unsigned char   Cmd;

	/* WARNING : During a read procedure you can't call the ReadStatus flash cmd */
	/* The ReadStatus fill the read register with 0xC0 and then corrupt the read.*/

	/* Push offset address */
	switch(fZone)
	{
		case ZONE_DATA:
			uBytesToRead = pNandInfo->uDataNbBytes;
			Cmd = CMD_READ_A0;
			break;
		case ZONE_INFO:
			uBytesToRead = pNandInfo->uSpareNbBytes;
			pOutBuffer += pNandInfo->uDataNbBytes;
			Cmd = CMD_READ_C;
			break;
		case ZONE_DATA | ZONE_INFO:
			uBytesToRead = pNandInfo->uSectorNbBytes;
			Cmd = CMD_READ_A0;
			break;
		default:
			bRet = FALSE;
			goto exit;
	}

	/* Enable the chip */
	NAND_ENABLE_CE();

	/* Write specific command, Read from start */
	if (pNandInfo->uDataBusWidth)
	{	/* 16 bits */
        WRITE_NAND_COMMAND16(Cmd);
    }
    else {
        WRITE_NAND_COMMAND(Cmd);
    }

	/* Push sector address */
	uSectorAddr >>= pNandInfo->uOffset;
		
	if (pNandInfo->uDataBusWidth)
	{	/* 16 bits */
        WRITE_NAND_ADDRESS16(0x00);
        WRITE_NAND_ADDRESS16((uSectorAddr >>  0) & 0xFF);
        WRITE_NAND_ADDRESS16((uSectorAddr >>  8) & 0xFF);
        WRITE_NAND_ADDRESS16((uSectorAddr >> 16) & 0xFF);
    }
    else {
        WRITE_NAND_ADDRESS(0x00);
        WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
        WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
        WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);
    }

	/* Wait for flash to be ready (can't pool on status, read upper WARNING) */
	NAND_WAIT_READY();
	NAND_WAIT_READY();	/* Need to be done twice, READY detected too early the first time? */
	
    /* Read loop */
    if (pNandInfo->uDataBusWidth)
    {	/* 16 bits */
        for(i=0; i<uBytesToRead/2; i++) // Div2 because of 16bits
        {
            *((short*)pOutBuffer) = READ_NAND16();
            pOutBuffer += 2;
        }
    } else {
        if (Cmd == CMD_READ_C) {
            for(i=0; (i<uBytesToRead) && (i); i++)
            {
                *pOutBuffer = READ_NAND();
                pOutBuffer++;
            }
        }
        else {
            for(i = 0; i < (uBytesToRead / 2); i++)
            {
                *pOutBuffer = READ_NAND();
                pOutBuffer++;
            }

            Cmd = CMD_READ_A1;
            WRITE_NAND_COMMAND(Cmd);
            WRITE_NAND_ADDRESS(0x00);
            WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
            WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
            WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

            /* Wait for flash to be ready (can't pool on status, read upper WARNING) */
            NAND_WAIT_READY();
            NAND_WAIT_READY();	/* Need to be done twice, READY detected too early the first time? */
        
            for (i = 0; i < (uBytesToRead / 2); i++)
            {
                *pOutBuffer = READ_NAND();
                pOutBuffer++;
            }
        }
    }

exit:
	/* Disable the chip */
	NAND_DISABLE_CE();

	return bRet;
}

#else /* NANDFLASH_LARGE_BLOCKS */

/*------------------------------------------------------------------------------*/
/* \fn    AT91F_NandReadSector							*/
/* \brief Read a Sector								*/
/*------------------------------------------------------------------------------*/
static BOOL AT91F_NandReadSector(PSNandInfo pNandInfo, unsigned int uSectorAddr, char *pOutBuffer, unsigned int fZone)
{
	BOOL		bRet = TRUE;
	unsigned int	uBytesToRead, i;

	/* WARNING : During a read procedure you can't call the ReadStatus flash cmd */
	/* The ReadStatus fill the read register with 0xC0 and then corrupt the read.*/

	/* Enable the chip */
	NAND_ENABLE_CE();

	/* Write specific command, Read from start */
	WRITE_NAND_COMMAND(CMD_READ_1);

	/* Push offset address */
	switch(fZone)
	{
		case ZONE_DATA:
			uBytesToRead = pNandInfo->uDataNbBytes;
			WRITE_NAND_ADDRESS(0x00);
			WRITE_NAND_ADDRESS(0x00);
			break;
		case ZONE_INFO:
			uBytesToRead = pNandInfo->uSpareNbBytes;
			pOutBuffer += pNandInfo->uDataNbBytes;
			if (pNandInfo->uDataBusWidth)
			{	/* 16 bits */
				WRITE_NAND_ADDRESS(((pNandInfo->uDataNbBytes/2) >>  0) & 0xFF); /* Div 2 is because we address in word and not
				in byte */
				WRITE_NAND_ADDRESS(((pNandInfo->uDataNbBytes/2) >>  8) & 0xFF);
			} else { /* 8 bits */
				WRITE_NAND_ADDRESS((pNandInfo->uDataNbBytes >>  0) & 0xFF);
				WRITE_NAND_ADDRESS((pNandInfo->uDataNbBytes >>  8) & 0xFF);			
			}
			break;
		case ZONE_DATA | ZONE_INFO:
			uBytesToRead = pNandInfo->uSectorNbBytes;
			WRITE_NAND_ADDRESS(0x00);
			WRITE_NAND_ADDRESS(0x00);
			break;
		default:
			bRet = FALSE;
			goto exit;
	}

	/* Push sector address */
	uSectorAddr >>= pNandInfo->uOffset;
		
	WRITE_NAND_ADDRESS((uSectorAddr >>  0) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >>  8) & 0xFF);
	WRITE_NAND_ADDRESS((uSectorAddr >> 16) & 0xFF);

	WRITE_NAND_COMMAND(CMD_READ_2);

	/* Wait for flash to be ready (can't pool on status, read upper WARNING) */
	NAND_WAIT_READY();
	NAND_WAIT_READY();	/* Need to be done twice, READY detected too early the first time? */
	
	/* Read loop */
	if (pNandInfo->uDataBusWidth)
	{	/* 16 bits */
		for(i=0; i<uBytesToRead/2; i++) /* Div2 because of 16bits */
		{
			*((short*)pOutBuffer) = READ_NAND16();
			pOutBuffer+=2;
		}
	} else {
		for(i=0; i<uBytesToRead; i++)
		{
			*pOutBuffer++ = READ_NAND();
		}
	}

exit:
	/* Disable the chip */
	NAND_DISABLE_CE();

	return bRet;
}
#endif

//*----------------------------------------------------------------------------
//* \fn    CheckBlock
//* \brief Check if block is marked Bad
//*----------------------------------------------------------------------------
BOOL CheckBlock(PSNandInfo pNandInfo, unsigned int uBlockNb, char *pOutBuffer)
{
	unsigned int i = 0;
	PSSectorInfo pSectorInfo;
	unsigned int uSectorAddr = uBlockNb * pNandInfo->uBlockNbData;

	// Read first page and second page spare zone to detect if block is bad
	for (i = 0; i < 2; i++)
    {
		AT91F_NandReadSector(pNandInfo, (uSectorAddr + i * pNandInfo->uDataNbBytes), pOutBuffer, ZONE_INFO);
		pSectorInfo = (PSSectorInfo)&pOutBuffer[pNandInfo->uDataNbBytes];
		if (pSectorInfo->spare[pNandInfo->uBadBlockInfoOffset] != BAD_BLOCK_TAG)
		{
			return FALSE;
		}
    }

	return TRUE;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NandRead
//* \brief Read Sector Algorithm
//*----------------------------------------------------------------------------
BOOL AT91F_NandRead(PSNandInfo pNandInfo, unsigned int uBlockNb, unsigned int uSectorNb, unsigned int uSpareValue, char *pOutBuffer)
{
	unsigned int uSectorAddr = uBlockNb * pNandInfo->uBlockNbData + uSectorNb * pNandInfo->uDataNbBytes;

	if (CheckBlock(pNandInfo, uBlockNb, pOutBuffer) == FALSE)
	{
		return FALSE;
	}

	return AT91F_NandReadSector(pNandInfo, uSectorAddr, pOutBuffer, ZONE_DATA);
}

/*------------------------------------------------------------------------------*/
/* \fn    load_nandflash							*/
/* \brief load from nandflash 							*/
/*------------------------------------------------------------------------------*/
int load_nandflash(unsigned int img_addr, unsigned int img_size, unsigned int img_dest)
{
	SNandInfo sNandInfo;
	PSNandInitInfo pNandInitInfo;
	char *pOutBuffer = (char*)img_dest;
	unsigned int blockIdx, badBlock, blockRead, length, sizeToRead, nbSector, newBlock, sectorIdx, blockError, sectorSize;

	nandflash_hw_init();
	
	/* Read Nand Chip ID */
    pNandInitInfo = AT91F_NandReadID();

	if (!pNandInitInfo)
 	{
#ifdef CFG_DEBUG	
	   	dbg_print("\n\r-E- No NandFlash detected !!!\n\r");
#endif
		return -1;
    	}

	/* Initialize NandInfo Structure */
	AT91F_NandInit(&sNandInfo, pNandInitInfo);

	if (!sNandInfo.uDataBusWidth)
		nandflash_cfg_8bits_dbw_init();

    	/* Initialize the block offset */
    	blockIdx = img_addr / sNandInfo.uBlockNbData;
	/* Initialize the number of bad blocks */
    	badBlock = 0;
	blockRead = 0;
    
	length = img_size;
    
	while (length > 0)
	{
        	/* Read a buffer corresponding to a block in the origin file */
		if (length < sNandInfo.uBlockNbData)
		{
			sizeToRead = length;
		}
		else
		{
			sizeToRead = sNandInfo.uBlockNbData;
		}

		/* Adjust the number of sectors to read */
        	nbSector = sizeToRead / sNandInfo.uDataNbBytes;
        	if (sizeToRead % sNandInfo.uDataNbBytes)
		{
            		nbSector++;
        	}

        	newBlock = 1;
		/* Loop until a valid block has been read */
		while (newBlock == 1)
		{
			/* Reset the error flag */
			blockError = 0;
            
			/* Read the sectors */
			for (sectorIdx=0; (sectorIdx < nbSector) && (blockError == 0); sectorIdx++)
			{
				sectorSize = sizeToRead - (sectorIdx * sNandInfo.uDataNbBytes);
				if (sectorSize < sNandInfo.uDataNbBytes)
				{
					sectorSize = sizeToRead - (sectorIdx * sNandInfo.uDataNbBytes);
				}
				else
				{
					sectorSize = sNandInfo.uDataNbBytes;
				}

	                	/* Read the sector */
        	        	if (AT91F_NandRead(&sNandInfo, blockIdx, sectorIdx, ZONE_DATA, pOutBuffer) == FALSE)
				{
					blockError = 1;
				}
				else
				{
					pOutBuffer+=sNandInfo.uDataNbBytes;
				}
			}
            
			if (blockError == 0)
			{
                		/* If the block is valid exit */
	                	newBlock = 0;
        	    	}
			blockIdx++;
		}

        	/* Decrement length */
	        length -= sizeToRead;
		blockRead++;
	}

	return 0;
}
#endif
