#ifndef PHYSICAL_LAYER_H
#define PHYSICAL_LAYER_H

#if defined (__USE_LPCOPEN)
	#if defined(NO_BOARD_LIB)
		#include "chip.h"
	#else
		#include "board.h"
	#endif
#endif

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000
#define I2C_DEFAULT_SPEED    SPEED_100KHZ
#define I2C_FASTPLUS_BIT     0

#if (I2C_DEFAULT_SPEED > SPEED_400KHZ)
#undef  I2C_FASTPLUS_BIT
#define I2C_FASTPLUS_BIT IOCON_FASTI2C_EN
#endif

#define CH0 0x00
#define CH1 0x01

#include <stdbool.h>
#include <stddef.h>
#include "system_config.h"
#include "cltu.h"
#include "clcw.h"
#include "i2c_11xx.h"
#include "ring_buffer.h"

typedef uint32_t request_id_t;

void phy_init(void);
bool phy_transmit_request(cltu_t * cltu, request_id_t id);
bool phy_clcw_request(clcw_t * clcw);
void phy_set_clcw(clcw_t clcw);

bool phy_is_activated(void);
bool phy_get_data(uint8_t * data);

#endif // PHYSICAL_LAYER_H
