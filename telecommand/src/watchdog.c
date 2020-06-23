#include "watchdog.h"

/**
 * @brief	Watchdog Timer Interrupt Handler
 * @return	Nothing
 * @note	Handles watchdog timer warning and timeout events
 */
void WDT_IRQHandler(void)
{
	uint32_t wdtStatus = Chip_WWDT_GetStatus(LPC_WWDT);

#if defined(CHIP_LPC11CXX)
	if (wdtStatus & WWDT_WDMOD_WDTOF) {
		Board_LED_Toggle(0);
		while(Chip_WWDT_GetStatus(LPC_WWDT) & WWDT_WDMOD_WDTOF) {
			Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
		}
		Chip_WWDT_Start(LPC_WWDT);	/* Needs restart */
	}
#else
	Board_LED_Toggle(0);

	/* The chip will reset before this happens, but if the WDT doesn't
	   have WWDT_WDMOD_WDRESET enabled, this will hit once */
	if (wdtStatus & WWDT_WDMOD_WDTOF) {
		/* A watchdog feed didn't occur prior to window timeout */
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
		Chip_WWDT_Start(LPC_WWDT);	/* Needs restart */
	}

	/* Handle warning interrupt */
	if (wdtStatus & WWDT_WDMOD_WDINT) {
		/* A watchdog feed didn't occur prior to warning timeout */
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDINT);
		Chip_WWDT_Feed(LPC_WWDT);
	}
#endif
}

/**
 * @brief	SysTick Interrupt Handler
 * @return	Nothing
 * @note	Systick interrupt handler feeds WWDT
 */
void SysTick_Handler(void)
{
#if !defined(DISABLE_WDT_TIMEOUT)
	Chip_WWDT_Feed(LPC_WWDT);
#endif
}

void watchdog_init(void) {
	uint32_t wdtFreq;

	/* Initialize WWDT (also enables WWDT clock) */
	Chip_WWDT_Init(LPC_WWDT);

	/* Prior to initializing the watchdog driver, the clocking for the
	   watchdog must be enabled. This example uses the watchdog oscillator
	   set at a 50KHz (1Mhz / 20) clock rate. */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_WDTOSC_PD);
	Chip_Clock_SetWDTOSC(WDTLFO_OSC_1_05, 20);

	/* The WDT divides the input frequency into it by 4 */
	wdtFreq = Chip_Clock_GetWDTOSCRate() / 4;

	/* LPC1102/4, LPC11XXLV, and LPC11CXX devices select the watchdog
	   clock source from the SYSCLK block, while LPC11AXX, LPC11EXX, and
	   LPC11UXX devices select the clock as part of the watchdog block. */
	/* Select watchdog oscillator for WDT clock source */
	Chip_Clock_SetWDTClockSource(SYSCTL_WDTCLKSRC_WDTOSC, 1);

	Board_LED_Set(0, false);

	/* Set watchdog feed time constant to approximately 2s
	   Set watchdog warning time to 512 ticks after feed time constant
	   Set watchdog window time to 3s */
	Chip_WWDT_SetTimeOut(LPC_WWDT, wdtFreq * 2);
#if !defined(CHIP_LPC11CXX)
	Chip_WWDT_SetWarning(LPC_WWDT, 512);
	Chip_WWDT_SetWindow(LPC_WWDT, wdtFreq * 3);
#endif

#if !defined(CHIP_LPC11CXX)
	/* Configure WWDT to reset on timeout */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);
#endif

	/* Setup Systick to feed the watchdog timer. This needs to be done
	 * at a rate faster than the WDT warning. */
	SysTick_Config(Chip_Clock_GetSystemClockRate() / 50);

	/* Clear watchdog warning and timeout interrupts */
#if !defined(CHIP_LPC11CXX)	
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);
#else
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
#endif	

	/* Clear and enable watchdog interrupt */
	NVIC_ClearPendingIRQ(WDT_IRQn);
	NVIC_EnableIRQ(WDT_IRQn);

	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);
}

void watchdog_reset() {
	Chip_WWDT_Feed(LPC_WWDT);
}
