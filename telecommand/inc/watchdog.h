#ifndef WATCHDOG_H
#define WATCHDOG_H

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
	#include "chip.h"
#else
	#include "board.h"
#endif
#endif

void watchdog_init(void);
void watchdog_reset(void);

#endif // WATCHDOG_H
