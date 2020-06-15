#include "coding_layer.h" 

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
	return phy_clcw_request(&clcw);
}


