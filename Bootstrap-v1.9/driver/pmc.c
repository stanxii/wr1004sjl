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
 * this list of conditions and the disclaiimer below.
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
 * File Name           : pmc.c
 * Object              :
 * Creation            : ODi Apr 24th 2006
 *-----------------------------------------------------------------------------
 */
#include "../include/pmc.h"
#include "../include/part.h"
#include "../include/main.h"

/* Write PMC register */
static inline void write_pmc(unsigned int offset, const unsigned int value)
{
	writel(value, offset + AT91C_BASE_PMC);
}

/* Read PMC registers */
static inline unsigned int read_pmc(unsigned int offset)
{
	return readl(offset + AT91C_BASE_PMC);
}


//*----------------------------------------------------------------------------
//* \fn    pmc_cfg_plla
//* \brief Configure the pll frequency to the corresponding value.
//*----------------------------------------------------------------------------*/
int pmc_cfg_plla(unsigned int pmc_pllar, unsigned int timeout)
{
	write_pmc((unsigned int)PMC_PLLAR, pmc_pllar);

	while ( (timeout--) && !(read_pmc(PMC_SR) & AT91C_PMC_LOCKA) );
	return (timeout) ? 0 : (-1);
}

//*----------------------------------------------------------------------------
//* \fn    pmc_cfg_pllb
//* \brief Configure the pll frequency to the corresponding value.
//*----------------------------------------------------------------------------*/
int pmc_cfg_pllb(unsigned int pmc_pllbr, unsigned int timeout)
{
	write_pmc(PMC_PLLBR, pmc_pllbr);

	while ( (timeout--) && !(read_pmc(PMC_SR) & AT91C_PMC_LOCKB) );
	return (timeout) ? 0 : (-1);
}

//*----------------------------------------------------------------------------
//* \fn    pmc_cfg_mck
//* \brief Configure the main oscillator to the corresponding value.
//*----------------------------------------------------------------------------*/
int pmc_cfg_mck(unsigned int pmc_mckr, unsigned int timeout)
{
	write_pmc(PMC_MCKR,  pmc_mckr);

	while ( (timeout--) && !(read_pmc(PMC_SR) & AT91C_PMC_MCKRDY) );
	return (timeout) ? 0 : (-1);
}

//*----------------------------------------------------------------------------
//* \fn    pmc_cfg_pck
//* \brief Configure the PCK frequency to the corresponding value.
//*----------------------------------------------------------------------------*/
int pmc_cfg_pck(unsigned char x, unsigned int clk_sel, unsigned int prescaler)
{
	write_pmc(PMC_PCKR + x * 4,  clk_sel | prescaler);
	write_pmc(PMC_SCER,  1 << (x + 8));
	while ( !(read_pmc(PMC_SR) & (1 << (x + 8))) );
	return 0;
}
