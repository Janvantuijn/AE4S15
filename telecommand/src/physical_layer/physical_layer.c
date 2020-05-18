/*
 * physical_layer.c
 *
 *  Created on: May 14, 2020
 *      Author: jan
 */

#include "physical_layer.h"
#include "buffer.h"

#define I2C_SLAVE_ADDRESS 0x67
#define I2C_BUFFER_SIZE 10

static int phy_is_active = false;
static phy_tx_event_t next_event = PHY_IDLE;

// I2C structs and buffers
const static I2C_ID_T i2c_id = I2C0;
static I2C_XFER_T i2c_xfer;
static uint8_t i2c_buffer[I2C_BUFFER_SIZE];

// PHY structs and buffers
static uint8_t phy_buffer_arr[BUFFER_SIZE];
static buffer_t phy_buffer;

static cltu_t * phy_cltu_transmitting = NULL;
static cltu_t * phy_cltu_pending = NULL;
static request_id_t phy_id_transmitting = -1;
static request_id_t phy_id_pending = -1;

static phy_tx_event_t phy_tx_event_handler(phy_tx_event_t event);
static void i2c_slave_phy_events(I2C_ID_T id, I2C_EVENT_T event);
static void Init_I2C_PinMux(void);
static void i2c_state_handling(I2C_ID_T id);

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

void phy_run(void) {
	next_event = phy_tx_event_handler(next_event);
}

void phy_activate(void) {
	phy_is_active = 1;
}

void phy_deactivate(void) {
	phy_is_active = 0;
}

bool phy_get_state(void) {
	return (phy_is_active == 1);
}

static phy_tx_event_t phy_tx_event_handler(phy_tx_event_t event)
{
	int ret = 0;
	int i = 0;
	switch (event)
	{
		case PHY_TX_DONE:
			if (phy_cltu_pending != NULL) {
				// Copy pending CLTU to transmitting CLTU
				phy_cltu_transmitting = phy_cltu_pending;
				phy_cltu_pending = NULL;
				phy_id_transmitting = phy_id_pending;
				phy_id_pending = -1;
				return PHY_TX_NEW_CLTU;
			} else {
				phy_cltu_transmitting = NULL;
				phy_id_transmitting = -1;
				return PHY_IDLE;
			}
			break;
		case PHY_TX_NEW_CLTU: {
			cltu_t * cltu = phy_cltu_transmitting;
			// Copy CLTU data to PHY buffer 
			buffer_insert(&phy_buffer, CH0);
			buffer_insert(&phy_buffer, (cltu->start_seq >> 8) & 0xFF);
			buffer_insert(&phy_buffer, cltu->start_seq & 0xFF);
			// for (i = 0; i < cltu_size(cltu); i++) {
			// 	buffer_insert(&phy_buffer, cltu->code_blocks[i]);
			// } 
			buffer_insert(&phy_buffer, cltu->tail_seq);
			break; 
		} 
		case PHY_TX_TRANSMIT:
			// TODO: sizeof cltu can be bigger than uint8
			ret = Chip_I2C_MasterSend(i2c_id, I2C_SLAVE_ADDRESS, phy_buffer.array, cltu_size(phy_cltu_transmitting));		
			return PHY_TX_DONE;
		default:
		break;
	}

	return PHY_IDLE;
}

request_resp_t phy_transmit_request(cltu_t * cltu, request_id_t id) {

	if (phy_cltu_pending != NULL) {
		return CLTU_REJECTED;
	} else if (phy_cltu_transmitting == NULL)  {
		phy_cltu_transmitting = cltu;
		phy_id_transmitting = id;
		next_event = PHY_TX_NEW_CLTU;
		return CLTU_ACCEPTED;
	} else if (phy_cltu_pending == NULL) {
		phy_cltu_pending = cltu;
		phy_id_pending = id;
		return CLTU_ACCEPTED;
	}

	return CLTU_REJECTED;
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
			phy_deactivate();
		}

		if (phy_get_state()) {
			// Copy data from I2C buffer to PHY buffer
			if (!buffer_is_full(&phy_buffer)) {
				buffer_insert(&phy_buffer, i2c_buffer[0]);
			}

		} else {
			if (i2c_buffer[0] ==  1) { // First after activation is always the channel
				phy_activate();
			} else {
				phy_deactivate();
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
		//seep_data[0]++;
		//seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
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
