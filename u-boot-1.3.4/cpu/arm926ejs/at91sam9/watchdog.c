/*
 * watchdog.c - driver for AT91SAM on-chip watchdog
 *
 * Developed by Giulio Benetti <giulio.bene...@micronovasrl.com>
 * at Micronova srl <i...@micronovasrl.com>
 *
 * !!!ON AT91BOOTSTRAP DELETE WDT SETUP!!!
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <watchdog.h>

#include <asm/arch/hardware.h>
#include <asm/arch/io.h>
#include <asm/arch/at91_wdt.h>

#ifdef CONFIG_HW_WATCHDOG

#define WDT_HEARTBEAT 15

void hw_watchdog_reset(void)
{
	at91_sys_write(AT91_WDT_CR,AT91_WDT_WDRSTT | AT91_WDT_KEY);
}

void hw_watchdog_enable(void)
{
	unsigned int reg;
	unsigned int mr;

	/* Check if disabled */
	mr = at91_sys_read(AT91_WDT_MR);
	if (mr & AT91_WDT_WDDIS)
	{
		printf("hw_watchdog_enable: sorry, wdt is disabled already\n");
		return;
	}

	/*
	 * All counting occurs at SLOW_CLOCK / 128 = 256 Hz
	 *
	 * Since WDV is a 12-bit counter, the maximum period is
	 * 4096 / 256 = 16 seconds.
	 */	
	reg = AT91_WDT_WDRSTEN	/* causes watchdog reset */
		//| AT91_WDT_WDRPROC	/*causes processor reset only */
		| AT91_WDT_WDDBGHLT	/* disabled in debug mode */
		| AT91_WDT_WDD		/* restart at any time */
		| (((WDT_HEARTBEAT * 256) -1) & AT91_WDT_WDV);  /* timer value */
	at91_sys_write(AT91_WDT_MR, reg);

	/* Check if watchdog could be programmed */
	mr = at91_sys_read(AT91_WDT_MR);
	if (mr != reg)
	{
		printf("hw_watchdog_enable: Watchdog register already programmed.\n");
		return;
	}

	hw_watchdog_reset();
	printf("at91_wdt: enabled (heartbeat=%d sec)\n", WDT_HEARTBEAT);
}

void hw_watchdog_disable(void)
{
	at91_sys_write(AT91_WDT_MR, AT91_WDT_WDDIS);
	printf("at91_wdt: hw_watchdog_disabled\n");
}
#endif
