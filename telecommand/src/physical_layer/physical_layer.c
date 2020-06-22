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
static uint8_t phy_buffer_arr[BUFFER_SIZE];

static buffer_t phy_tx_buffer;
static RINGBUFF_T phy_rx_buffer;

static clcw_t clcw = {0, 0};

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


	Chip_I2C_SetMasterEventHandler(i2c_id, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

#ifdef I2C_SLAVE
	RingBuffer_Init(&phy_rx_buffer, (void*)phy_buffer_arr, sizeof(uint8_t), BUFFER_SIZE);
	i2c_xfer.slaveAddr = (I2C_SLAVE_ADDRESS << 1);
	i2c_xfer.txBuff = (uint8_t*)&clcw;
	i2c_xfer.rxBuff = i2c_buffer;
	i2c_xfer.txSz = sizeof(clcw_t);
	i2c_xfer.rxSz = I2C_BUFFER_SIZE;
	Chip_I2C_SlaveSetup(i2c_id, I2C_SLAVE_0, &i2c_xfer, i2c_slave_phy_events, 0);
#else
	buffer_init(&phy_tx_buffer, phy_buffer_arr);
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
	buffer_clear(&phy_tx_buffer);

	// Write channel number
	buffer_insert(&phy_tx_buffer, CH0);

	// write start sequence
	buffer_insert(&phy_tx_buffer, (cltu->start_seq >> 8) & 0xFF);
	buffer_insert(&phy_tx_buffer, cltu->start_seq & 0xFF);

	// Write code blocks
	for (int i = 0; i < cltu->size * (CODE_BLOCK_LENGTH + 2); i++) {
		buffer_insert(&phy_tx_buffer, ((uint8_t*)cltu->code_blocks)[i]);
	}

	// Write tail sequence
	for (int i = 0; i < CODE_BLOCK_LENGTH + 2; i++) {
		buffer_insert(&phy_tx_buffer, ((uint8_t*)&cltu->tail_seq)[i]);
	}

	Chip_I2C_MasterSend(i2c_id, I2C_SLAVE_ADDRESS, phy_tx_buffer.array, phy_tx_buffer.size);
	return true;
}

bool phy_clcw_request(clcw_t * clcw) {
	return Chip_I2C_MasterRead(i2c_id, I2C_SLAVE_ADDRESS, (uint8_t*)clcw, sizeof(clcw_t));
}

void phy_set_clcw(clcw_t clcw) {

}

bool phy_is_activated(void) {
	return (phy_is_active == 1);
}

bool phy_get_data(uint8_t * data) {
	return (RingBuffer_Pop(&phy_rx_buffer, data) == 1);
}

/* Slave event handler for simulated EEPROM */
static void i2c_slave_phy_events(I2C_ID_T id, I2C_EVENT_T event)
{
	switch (event) {
	case I2C_EVENT_DONE: // Done event that wakes up Wait event
		break;
	case I2C_EVENT_SLAVE_RX: // Slave receive event
		/* Each time we get here the base pointer to our buffer has been increased by one and the size of the our buffer
		 * has been decreased by one.
		 */
#ifdef I2C_SLAVE
		if (Chip_I2C_get_activation_state() == I2C_DEACTIVATED) {
			phy_is_active = 0;
		}

		if (phy_is_active == 1) {
			// Copy data from I2C buffer to PHY buffer
			if (!RingBuffer_IsFull(&phy_rx_buffer)) {
				RingBuffer_Insert(&phy_rx_buffer, (const void *)&(i2c_buffer[0]));
			}

		} else {
			if (i2c_buffer[0] ==  CH0) { // First after activation is always the channel
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
		// Send the CLCW
		if (i2c_xfer.txSz == 0) {
			i2c_xfer.txBuff = (uint8_t*)&clcw;
			i2c_xfer.txSz = sizeof(clcw_t);
		}
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
