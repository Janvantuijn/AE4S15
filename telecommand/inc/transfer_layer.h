#ifndef TRANSFER_LAYER_H
#define TRANSFER_LAYER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "clcw.h"
#include "code_block.h"
#include "ring_buffer.h"

typedef enum {
	CLCW_ACK = 0,
	CLCW_NACK,
	CLCW_NOT_UPDATED,
	CLCW_READ_FAIL
} ack_response_t;

typedef enum {
	FRAME_HEADER = 0,
	FRAME_DATA
} transfer_decoding_state_t;

void transfer_layer_init(void);
void transfer_layer_run(void);

// Master interfaces
int16_t transfer_layer_send_message(uint8_t * data, const uint8_t length);
ack_response_t transfer_layer_check_ack(uint8_t sequence_number);

// Slave interfaces
int16_t transfer_layer_receive_message(uint8_t * data, uint8_t * length);
void transfer_layer_add_code_block(const code_block_t * block);

#endif // TRANSFER_LAYER_H

