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
#include "i2c_11xx.h"

typedef int request_id_t;
typedef enum {
	CLTU_ACCEPTED = 0,
	CLTU_REJECTED,
} request_resp_t;

typedef enum {
	PHY_IDLE = 0,
	PHY_TX_DONE,
	PHY_TX_NEW_CLTU,
	PHY_TX_TRANSMIT
} phy_tx_event_t;

void phy_init(void);
void phy_run(void);

// Used by slave phy
void phy_activate(void);
void phy_deactivate(void);
bool phy_get_state(void);

// Used by master phy
request_resp_t phy_transmit_request(cltu_t * cltu, request_id_t id);


#endif // PHYSICAL_LAYER_H
