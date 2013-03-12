/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support  -  ROUSSET  -
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
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
 * File Name           : dataflash.c
 * Object              : ATMEL DataFlash High level functions
 * Creation            : NLe Jul 12th 2006
 *---------------------------------------------------------------------------
*/

#include "../include/part.h"
#include "../include/main.h"
#include "../include/dataflash.h"
#include <stdlib.h>

#ifdef CFG_DATAFLASH

/* Write SPI register */
static inline void write_spi(unsigned int offset, const unsigned int value)
{
	writel(value, offset + AT91C_BASE_SPI);
}

/* Read SPI registers */
static inline unsigned int read_spi(unsigned int offset)
{
	return readl(offset + AT91C_BASE_SPI);
}

extern div_t	udiv(unsigned int dividend, unsigned int divisor);
/*------------------------------------------------------------------------------*/
/* \fn    df_spi_init								*/
/* \brief Configure SPI								*/
/*------------------------------------------------------------------------------*/
static int df_spi_init(unsigned int pcs, unsigned int spi_csr)
{
	unsigned int ncs = 0;

	/* Open PIO for SPI0 */
	df_hw_init();

	/* Enables the SPI0 Clock */
	writel((1 << AT91C_ID_SPI), PMC_PCER + AT91C_BASE_PMC);

	/* Reset SPI0 */
        write_spi(SPI_CR, AT91C_SPI_SWRST);
#ifdef AT91SAM9263
	/* SAM9263-MRLB SPI needs two software reset */
	write_spi(SPI_CR, AT91C_SPI_SWRST);
#endif	

    	/* Configure SPI0 in Master Mode with No CS selected */
    	write_spi(SPI_MR, AT91C_SPI_MSTR | AT91C_SPI_MODFDIS | AT91C_SPI_PCS);

	switch (pcs)
	{
		case AT91C_SPI_PCS0_DATAFLASH:	ncs = 0;	break;
		case AT91C_SPI_PCS1_DATAFLASH:	ncs = 1;	break;
		case AT91C_SPI_PCS2_DATAFLASH:	ncs = 2;	break;
		case AT91C_SPI_PCS3_DATAFLASH:	ncs = 3;	break;
	}
	/* Configure CSx */
	write_spi(SPI_CSR + 4*ncs, spi_csr);

	/* Choose CSx */
	write_spi(SPI_MR, read_spi(SPI_MR) & 0xFFF0FFFF);
	write_spi(SPI_MR, read_spi(SPI_MR) | ((pcs<<16) & AT91C_SPI_PCS));

	/* SPI_Enable */
	write_spi(SPI_CR, AT91C_SPI_SPIEN);

	return 0;
}

/*------------------------------------------------------------------------------*/
/* \fn    df_is_busy								*/
/* \brief Test if SPI has received a buffer or not				*/
/*------------------------------------------------------------------------------*/
static AT91S_DF_SEM df_is_busy(
	AT91PS_DF pDataFlash)
{
	unsigned int dStatus = read_spi(SPI_SR);

	/* If End of Receive Transfer interrupt occurred */
 	if (( dStatus & AT91C_SPI_RXBUFF))
 	{
	 	write_spi(SPI_PTCR, AT91C_PDC_TXTDIS);	/* PDC Disable Tx */
	 	write_spi(SPI_PTCR, AT91C_PDC_RXTDIS);	/* PDC Disable Rx */

 		/* Release the semaphore */
		pDataFlash->bSemaphore = UNLOCKED;
		return UNLOCKED;
	}
	return  pDataFlash->bSemaphore;
}

/*------------------------------------------------------------------------------*/
/* \fn    df_send_command							*/
/* \brief Generic function to send a command to the dataflash			*/
/*------------------------------------------------------------------------------*/
char df_send_command (
	AT91PS_DF pDataFlash,
	unsigned char bCmd,      /* Command value */
	unsigned char bCmdSize,  /* Command Size */
	char         *pData,     /* Data to be sent */
	unsigned int  dDataSize, /* Data Size */
	unsigned int  dAddress)  /* Dataflash Address */
{
	unsigned int dInternalAdr;
 	div_t result = udiv(dAddress, AT91C_PAGE_SIZE(pDataFlash));

 	/* Try to get the dataflash semaphore */
	if ((pDataFlash->bSemaphore) != UNLOCKED)
		return (char) 0;
	pDataFlash->bSemaphore = LOCKED;

	/* Compute command pattern */
	dInternalAdr = (result.quot << AT91C_PAGE_OFFSET(pDataFlash)) + result.rem;

 	if (AT91C_DF_NB_PAGE(pDataFlash) >= 16384)
	{
		pDataFlash->command[0] = (bCmd & 0x000000FF) | \
	                             ((dInternalAdr & 0x0F000000) >> 16) | \
	                             ((dInternalAdr & 0x00FF0000) >>  0) | \
	                             ((dInternalAdr & 0x0000FF00) << 16);
 		pDataFlash->command[1] =  (dInternalAdr & 0x000000FF);

		if ((bCmd != DB_CONTINUOUS_ARRAY_READ) && (bCmd != DB_PAGE_READ))
			bCmdSize++;
	}
	else
	{
		pDataFlash->command[0] = (bCmd & 0x000000FF) | \
	                             ((dInternalAdr & 0x00FF0000) >> 8) | \
	                             ((dInternalAdr & 0x0000FF00) << 8) | \
	                             ((dInternalAdr & 0x000000FF) << 24);
 		pDataFlash->command[1] = 0;
	}

 	/* Send Command and data through the SPI */
 	write_spi(SPI_PTCR, AT91C_PDC_RXTDIS);				/* PDC Disable Rx*/
 	write_spi(SPI_RPR, (unsigned int) &(pDataFlash->command));	/* PDC Set Rx */
	write_spi(SPI_RCR, bCmdSize);
 	write_spi(SPI_RNPR, (unsigned int) pData);			/* PDC Set Next	Rx */
	write_spi(SPI_RNCR, dDataSize);

 	write_spi(SPI_PTCR, AT91C_PDC_TXTDIS);				/* PDC Disable Tx */
 	write_spi(SPI_TPR, (unsigned int) &(pDataFlash->command));	/* PDC Set Tx */
	write_spi(SPI_TCR, bCmdSize);
 	write_spi(SPI_TNPR, (unsigned int) pData);			/* PDC Set Next Tx */
	write_spi(SPI_TNCR, dDataSize);

 	write_spi(SPI_PTCR, AT91C_PDC_RXTEN);				/* PDC Enable Rx */
 	write_spi(SPI_PTCR, AT91C_PDC_TXTEN);				/* PDC Enable Tx */

    	while (df_is_busy(pDataFlash) == LOCKED);

	return 1;
}

/*------------------------------------------------------------------------------*/
/* \fn    df_wait_ready								*/
/* \brief wait for DataFlash to be ready					*/
/*------------------------------------------------------------------------------*/
static char df_wait_ready(AT91PS_DF pDataFlash)
{
	unsigned int timeout = 0;

	while (timeout++ < AT91C_DF_TIMEOUT)
	{
		if (df_get_status(pDataFlash))
		{
			if (df_is_ready(pDataFlash))
				return 1;
		}
	}

	return 0;
}

/*------------------------------------------------------------------------------*/
/* \fn    df_read								*/
/* \brief Read a block in dataflash						*/
/*------------------------------------------------------------------------------*/
static int df_read(
	AT91PS_DF pDf,
	unsigned int addr,
	unsigned char *buffer,
	unsigned int size)
{
	unsigned int SizeToRead;

	while (size)
	{
		SizeToRead = (size < AT91C_MAX_PDC_SIZE)? size : AT91C_MAX_PDC_SIZE;

		/* wait the dataflash ready status */
		df_wait_ready(pDf);
	    	df_continuous_read(pDf, (char *)buffer, SizeToRead, addr);

		size -= SizeToRead;
		addr += SizeToRead;
		buffer += SizeToRead;
	}

   	return 0;
}

/*----------------------------------------------------------------------*/
/* \fn    df_download							*/
/* \brief load the content of the dataflash				*/
/*----------------------------------------------------------------------*/
static int df_download(AT91PS_DF pDf, unsigned int img_addr, unsigned int img_size, unsigned int img_dest)
{
	/* read bytes in the dataflash */
	df_read(pDf, img_addr,(unsigned char *)img_dest, img_size);

	/* wait the dataflash ready status */
	df_wait_ready(pDf);

    return 0;
}

/*----------------------------------------------------------------------*/
/* \fn    df_probe							*/
/* \brief Returns DataFlash ID						*/
/*----------------------------------------------------------------------*/
static int df_probe(AT91PS_DF pDf)
{
    char *pResult = (char *)(pDf->command);

    df_get_status(pDf);
    return (pResult[1] & 0x3C);
}

/*----------------------------------------------------------------------*/
/* \fn    df_init							*/
/* \brief This function tries to identify the DataFlash connected	*/
/*----------------------------------------------------------------------*/
static int df_init (AT91PS_DF pDf)
{
	int dfcode = 0;
	int status = 1;

	/* Default: AT45DB321B */
	pDf->dfDescription.pages_number = 8192;
	pDf->dfDescription.pages_size = 528;
	pDf->dfDescription.page_offset = 10;

	dfcode = df_probe (pDf);

	switch (dfcode)
	{
/*		case AT45DB011B:
			pDf->dfDescription.pages_number = 512;
			pDf->dfDescription.pages_size = 264;
			pDf->dfDescription.page_offset = 9;
			break;

		case AT45DB021B:
			pDf->dfDescription.pages_number = 1024;
			pDf->dfDescription.pages_size = 264;
			pDf->dfDescription.page_offset = 9;
			break;

		case AT45DB041B:
			pDf->dfDescription.pages_number = 2048;
			pDf->dfDescription.pages_size = 264;
			pDf->dfDescription.page_offset = 9;
			break;

		case AT45DB081B:
			pDf->dfDescription.pages_number = 4096;
			pDf->dfDescription.pages_size = 264;
			pDf->dfDescription.page_offset = 9;
			break;
*/
		case AT45DB161B:
			pDf->dfDescription.pages_number = 4096;
			pDf->dfDescription.pages_size = 528;
			pDf->dfDescription.page_offset = 10;
			break;

		case AT45DB321B:
			pDf->dfDescription.pages_number = 8192;
			pDf->dfDescription.pages_size = 528;
			pDf->dfDescription.page_offset = 10;
			break;

		case AT45DB642:
			pDf->dfDescription.pages_number = 8192;
			pDf->dfDescription.pages_size = 1056;
			pDf->dfDescription.page_offset = 11;
			break;
/*
		case AT45DB1282:
			pDf->dfDescription.pages_number = 16384;
			pDf->dfDescription.pages_size = 1056;
			pDf->dfDescription.page_offset = 11;
			break;

		case AT45DB2562:
			pDf->dfDescription.pages_number = 16384;
			pDf->dfDescription.pages_size = 2112;
			pDf->dfDescription.page_offset = 12;
			break;

		case AT45DB5122:
			pDf->dfDescription.pages_number = 32768;
			pDf->dfDescription.pages_size = 2112;
			pDf->dfDescription.page_offset = 12;
			break;
*/
		default:
		        status = 0;
			break;
	}

	return status;
}

/*------------------------------------------------------------------------------*/
/* \fn    load_df								*/
/* \brief This function loads dataflash content to specified address		*/
/*------------------------------------------------------------------------------*/	
int load_df(unsigned int pcs, unsigned int img_addr, unsigned int img_size, unsigned int img_dest)
{
    	AT91S_DF sDF;
    	AT91PS_DF pDf = (AT91PS_DF)&sDF;
    	unsigned int rxBuffer[128];

    	pDf->bSemaphore = UNLOCKED;

    	df_spi_init(pcs, DF_CS_SETTINGS);

    	if (!df_init(pDf))
        	return -1;

#if defined(AT91SAM9260) || defined(AT91SAM9XE) || defined(AT91SAM9G20)
	/* Test if a button has been pressed or not */
	/* Erase Page 0 to avoid infinite loop */
	df_recovery(pDf);
#endif

    	df_continuous_read(pDf, (char *)rxBuffer, 32, img_addr);
	df_wait_ready(pDf);

	return df_download(pDf, img_addr, img_size, img_dest);
}

#endif /* CFG_DATAFLASH */
