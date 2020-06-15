#ifndef TRANSFER_LAYER_H
#define TRANSFER_LAYER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "clcw.h"

typedef enum {
	CLCW_ACK = 0,
	CLCW_NACK,
	CLCW_NOT_UPDATED,
	CLCW_READ_FAIL
} ack_response_t;

void transfer_layer_init(void);
int16_t transfer_layer_send_message(const uint8_t * data, const uint8_t length);
ack_response_t transfer_layer_check_ack(uint8_t sequence_number);

#endif // TRANSFER_LAYER_H

