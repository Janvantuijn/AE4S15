/*
 * physical_layer.c
 *
 * Created on: May 14, 2020
 * Author: jan
 */

#include "physical_layer.h"
#include "buffer.h"

#define I2C_SLAVE_ADDRESS 0x67
#define I2C_BUFFER_SIZE 10

static int phy_is_active = 0;

// I2C structs and buffers
const static I2C_ID_T i2c_id = I2C0;
static I2C_XFER_T i2c_xfer;
static uint8_t i2c_buffer[I2C_BUFFER_SIZE];

// PHY structs and buffers
static volatile uint8_t phy_buffer_arr[BUFFER_SIZE];
static volatile buffer_t phy_buffer;

static void Init_I2C_PinMux(void);
static void i2c_slave_phy_events(I2C_ID_T id, I2C_EVENT_T event);
static void i2c_state_handling(I2C_ID_T id);

/* ------------------------- PUBLIC FUNCTIONS ------------------------------------ */

/**
 * @brief Initializes the Physical layer
 * 
 */
void phy_init(void) {
	Init_I2C_PinMux();

	/* Initialize I2C */
	Chip_I2C_Init(i2c_id);
	Chip_I2C_SetClockRate(i2c_id, I2C_DEFAULT_SPEED);

	buffer_init(&phy_buffer, phy_buffer_arr);

	Chip_I2C_SetMasterEventHandler(i2c_id, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

#ifdef I2C_SLAVE
	i2c_xfer.slaveAddr = (I2C_SLAVE_ADDRESS << 1);
	i2c_xfer.txBuff = NULL;
	i2c_xfer.rxBuff = i2c_buffer;
	i2c_xfer.txSz = 0;
	i2c_xfer.rxSz = I2C_BUFFER_SIZE;
	Chip_I2C_SlaveSetup(i2c_id, I2C_SLAVE_0, &i2c_xfer, i2c_slave_phy_events, 0);
#endif
}

/**
 * @brief Transmits the given cltu
 * 
 * @param cltu, cltu that needs to be transmitted
 * @param id, cltu identifier
 * @return request_resp_t, CLTU_REJECT if rejected (transmission already in process), else CLTU_ACCPTED
 */
bool phy_transmit_request(cltu_t * cltu, request_id_t id) {
	buffer_clear(&phy_buffer);

	// Write channel number
	buffer_insert(&phy_buffer, CH0);

	// write start sequence
	buffer_insert(&phy_buffer, (cltu->start_seq >> 8) & 0xFF);
	buffer_insert(&phy_buffer, cltu->start_seq & 0xFF);

	// Write code blocks
	for (int i = 0; i < cltu->size * (CODE_BLOCK_LENGTH + 2); i++) {
		buffer_insert(&phy_buffer, ((uint8_t*)cltu->code_blocks)[i]);
	}

	// Write tail sequence
	for (int i = 0; i < CODE_BLOCK_LENGTH + 2; i++) {
		buffer_insert(&phy_buffer, ((uint8_t*)&cltu->tail_seq)[i]);
	}

	Chip_I2C_MasterSend(i2c_id, I2C_SLAVE_ADDRESS, phy_buffer.array, phy_buffer.size);
	return true;
}

bool phy_clcw_request(clcw_t * clcw) {
	return Chip_I2C_MasterRead(i2c_id, I2C_SLAVE_ADDRESS, (uint8_t*)clcw, sizeof(clcw_t));
}

/* Slave event handler for simulated EEPROM */
static void i2c_slave_phy_events(I2C_ID_T id, I2C_EVENT_T event)
{
	switch (event) {
	case I2C_EVENT_DONE: // Done event that wakes up Wait event
		// Reset buffer pointer and buffer size to original
		i2c_xfer.rxBuff = i2c_buffer;
		i2c_xfer.rxSz = I2C_BUFFER_SIZE;
#ifdef I2C_SLAVE

#else
		//TODO
		//i2c_xfer.txBuff = i2c_buffer;
		//i2c_xfer.txSz++;
#endif
		break;

	case I2C_EVENT_SLAVE_RX: // Slave receive event
		/* Each time we get here the base pointer to our buffer has been increased by one and the size of the our buffer
		 * has been decreased by one.
		 */
#ifdef I2C_SLAVE
		if (Chip_I2C_get_activation_state() == I2C_DEACTIVATED) {
			phy_activated = 0;
		}

		if (phy_is_active == 1) {
			// Copy data from I2C buffer to PHY buffer
			if (!buffer_is_full(&phy_buffer)) {
				buffer_insert(&phy_buffer, i2c_buffer[0]);
			}

		} else {
			if (i2c_buffer[0] ==  1) { // First after activation is always the channel
				phy_is_active = 1;
			} else {
				phy_is_active = 0;
			}
		}
#endif
		// Reset buffer pointer and buffer size to original
		i2c_xfer.rxBuff = i2c_buffer;
		i2c_xfer.rxSz = I2C_BUFFER_SIZE;
		break;

//
//		if (is_addr) {
//			is_addr = 0;
//			//seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);	/* Correct addr if required */
//			//i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
//			break;
//		}
	case I2C_EVENT_SLAVE_TX: // Slave transmit event
#ifdef I2C_MASTER

#endif
		break;
		if (i2c_xfer.txSz == 1) {
			//i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;
	}
}

static void Init_I2C_PinMux(void)
{
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4, IOCON_FUNC1 | I2C_FASTPLUS_BIT);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5, IOCON_FUNC1 | I2C_FASTPLUS_BIT);
#elif (defined(BOARD_NXP_XPRESSO_11C24) || defined(BOARD_MCORE48_1125))
	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1 | I2C_FASTPLUS_BIT);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1 | I2C_FASTPLUS_BIT);
#else
	#error "No Pin Muxing defined for I2C operation"
#endif
}


void I2C_IRQHandler(void)
{
	i2c_state_handling(i2c_id);
}

static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	}
	else {
		Chip_I2C_SlaveStateHandler(id);
	}
}
