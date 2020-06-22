#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>

#if defined (__USE_LPCOPEN)
	#if defined(NO_BOARD_LIB)
		#include "chip.h"
	#else
		#include "board.h"
	#endif
#endif

#include "ring_buffer.h"

void monitor_init(void);
void monitor_send(const uint8_t * data, uint8_t length);

#endif // MONITOR_H
