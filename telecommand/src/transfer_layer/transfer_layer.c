#include "transfer_layer.h"
#include "coding_layer.h"
#include "frame.h"

static uint8_t frame_sequence_number = 0;

/**
 * @brief Send a message (non-blocking). This function requires you to use "transfer_layer_check_ack()" to check
 *        whether the message was acknowledged.
 * 
 * @param data, data of the message
 * @param size, length is the message
 * @return int16_t, -1 if failed to send the message, else returns the sequence number
 */
int16_t transfer_layer_send_message(const uint8_t * data, const uint8_t length) {
    bool ret = false;
    frame_t frame;

    // Create frame
    frame.length = length + 2;
    frame.sequence_number = frame_sequence_number++;
    frame.data = data;

    // Give frame to the coding layer
	ret = coding_layer_transmit_frame(&frame);
	if (!ret) {
		return -1;
	} else {
		return frame.sequence_number;
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
		if (clcw.FRAME_SEQUENCE == sequence_number && clcw.RETRANSMIT == 0) {
			return CLCW_ACK;
		} else if (clcw.FRAME_SEQUENCE == sequence_number) {
			return CLCW_NOT_UPDATED;
		} else {
			return CLCW_NACK;
		}
	}
}


