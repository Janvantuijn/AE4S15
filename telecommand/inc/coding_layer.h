#ifndef CODING_LAYER_H
#define CODING_LAYER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "frame.h"
#include "clcw.h"
#include "cltu.h"
#include "physical_layer.h"

void coding_layer_init(void);
bool coding_layer_transmit_frame(const frame_t * frame);
bool coding_layer_receive_ack(clcw_t * clcw);

#endif // CODING_LAYER_H
