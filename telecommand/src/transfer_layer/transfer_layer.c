#include "transfer_layer.h"
#include "coding_layer.h"
#include "frame.h"

// Final frame is stored in here
static frame_t latest_frame;
static uint8_t frame_index;
static uint8_t message_array[255];
static bool frame_complete = false;

// Incoming code blocks are stored in here
static RINGBUFF_T code_block_buffer;
static uint8_t code_block_array[512];

static transfer_decoding_state_t state = FRAME_HEADER;

static uint8_t tx_sequence_number = 0;
static uint8_t rx_sequence_number = 0;

void transfer_layer_init(void) {

	RingBuffer_Init(&code_block_buffer, (void*)code_block_array, sizeof(code_block_t), 512);

	latest_frame.length = 0;
	latest_frame.sequence_number = 0;
	latest_frame.data = message_array;
}

/**
 * @brief Send a message (non-blocking). This function requires you to use "transfer_layer_check_ack()" to check
 *        whether the message was acknowledged.
 * 
 * @param data, data of the message
 * @param size, length is the message
 * @return int16_t, -1 if failed to send the message, else returns the sequence number
 */
int16_t transfer_layer_send_message(uint8_t * data, const uint8_t length) {
    bool ret = false;
    frame_t frame;

    // Create frame
    frame.length = length + 2;
    frame.sequence_number = tx_sequence_number++;
    frame.data = data;

    // Give frame to the coding layer
	ret = coding_layer_transmit_frame(&frame);
	if (!ret) {
		return -1;
	} else {
		return frame.sequence_number;
	}
}

int16_t transfer_layer_receive_message(uint8_t * data, uint8_t * length) {
	if (frame_complete && *length < latest_frame.length - 2) {
		// Frame is longer than the length of the array were we need to store the data
		*length = latest_frame.length - 2;
		for (int i = 0; i < *length; i++) {
			data[i] = latest_frame.data[i];
		}
		frame_complete = false;
		return latest_frame.sequence_number;
	} else if (frame_complete) {
		for (int i = 0; i < *length; i++) {
			data[i] = latest_frame.data[i];
		}
		frame_complete = false;
		return latest_frame.sequence_number;
	} else {
		*length = 0;
		return -1;
	}
}

/**
 * @brief Checks whether the frame with the given sequence number was acknowledged by the slave
 * 
 * @param sequence_number, frame number of which we want to know whether it was acknowledged
 * @return ack_response_t, READ_FAIL when the master failed to read the slave, CLCW_ACK when frame was acknowledged
 *         CLCW_NACK when the frame wasn't acknowledged
 */
ack_response_t transfer_layer_check_ack(uint8_t sequence_number) {
	bool ret = false;
	clcw_t clcw;

	clcw_init(&clcw);

	// Read the CLCW from the slave
	ret = coding_layer_receive_ack(&clcw);

	if (!ret) {
		return CLCW_READ_FAIL;
	} else {
		if (clcw.FRAME_SEQUENCE == sequence_number) {
			return CLCW_ACK;
		} else {
			return CLCW_NACK;
		}
	}
}

void transfer_layer_add_code_block(const code_block_t * block) {
	if (!RingBuffer_IsFull(&code_block_buffer)) {
		RingBuffer_Insert(&code_block_buffer, block);
	}
}

void transfer_layer_run(void) {
	switch(state) {
		case FRAME_HEADER: {
			code_block_t code_block;
			if (RingBuffer_Pop(&code_block_buffer, &code_block)) {
				if (frame_index == 0){
					latest_frame.length = code_block.info_field[0];
					latest_frame.sequence_number = code_block.info_field[1];
					for (int i = 2; i < CODE_BLOCK_LENGTH; i++) {
						latest_frame.data[i - 2] = code_block.info_field[i];
					}
					frame_index = CODE_BLOCK_LENGTH;
					state = FRAME_DATA;
				}
			}
		}
		break;
		case FRAME_DATA: {

			if (frame_index >= latest_frame.length) {
				frame_complete = true;
				rx_sequence_number = latest_frame.sequence_number;
				clcw_t clcw;
				clcw.CLCW_VERSION = 0b01;
				clcw.FRAME_SEQUENCE = rx_sequence_number;
				coding_layer_set_clcw(clcw);
			} else {
				code_block_t code_block;
				if (RingBuffer_Pop(&code_block_buffer, &code_block)) {
					for (int i = 0; i < CODE_BLOCK_LENGTH; i++) {
						latest_frame.data[frame_index++ - 2] = code_block.info_field[i];
					}
					// This also add filler bytes from the code block but since we
					// return the length of the frame these should already be ignored by the user
				}
			}
		}
		break;
	}
}
