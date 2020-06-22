#include "coding_layer.h" 

static decoding_state_t state = INACTIVE;
static code_block_t current_code_block;
static uint8_t start_sequence[2];
static uint8_t start_sequence_index = 0;
static uint8_t block_size_count = 0;

static cltu_t cltu;
static uint32_t cltu_counter = 0;

void coding_layer_init(void) {
	cltu_init(&cltu);
}

bool coding_layer_transmit_frame(const frame_t * frame) {
	// Prepare CLTU for new data
	cltu_clear(&cltu);

	// Copy Frame data to CLTU
	cltu_insert_byte(&cltu, frame->length);
	cltu_insert_byte(&cltu, frame->sequence_number);
	cltu_insert_bytes(&cltu, frame->data, frame->length - 2);

	// Calculate the tail sequence
	cltu_calc_tail_sequence(&cltu);

	return phy_transmit_request(&cltu, cltu_counter++);
}

bool coding_layer_receive_ack(clcw_t * clcw) {
	return phy_clcw_request(clcw);
}

void coding_layer_set_clcw(clcw_t clcw) {
	phy_set_clcw(clcw);
}

void coding_layer_run(void) {
	switch(state) {
		case INACTIVE:
			if (phy_is_activated()) {
				state = SEARCH;
				block_size_count = 0;
				start_sequence_index = 0;
			}
			break;
		case SEARCH: {
			if (!phy_is_activated()) {
				state = INACTIVE;
				break;
			}

			uint8_t data;
			if (phy_get_data(&data)) {
				if (start_sequence_index < 2) {
					start_sequence[start_sequence_index++] = data;
				}

				if (start_sequence_index == 2 && start_sequence[0] == CLTU_START_SEQUENCE_MSB && start_sequence[1] == CLTU_START_SEQUENCE_LSB) {
					// We found the start sequence
					state = DECODE;
					break;
				} else if (start_sequence_index == 2) {
					// We've got 2 bytes but they are not the start sequences we set the second byte to the position of the first
					start_sequence[0] = start_sequence[1];
					start_sequence_index--;
				}
			}
			break;
		}
		case DECODE: {
			if (!phy_is_activated()) {
				state = INACTIVE;
				break;
			}

			if (block_size_count == CODE_BLOCK_LENGTH + 2) {
				uint16_t checksum = crc(current_code_block.info_field, CODE_BLOCK_LENGTH);

				if (checksum == current_code_block.error_control_field) {
					// Give a complete code block to the Transfer Layer
					transfer_layer_add_code_block(&current_code_block);
					block_size_count = 0;
				} else {
					// CRC failure of the received block
					block_size_count = 0;
					state = SEARCH;
				}
			} else {
				uint8_t data;
				if (phy_get_data(&data)) {
					if (block_size_count < CODE_BLOCK_LENGTH) {
						// Add information bytes
						current_code_block.info_field[block_size_count++] = data;
					} else {
						// Add error control field bytes
						((uint8_t*)&current_code_block.error_control_field)[block_size_count++ % CODE_BLOCK_LENGTH] = data;
					}
				}
				break;
			}
		}
	}
}

