#ifndef FRAME_H
#define FRAME_H

typedef struct {
	uint8_t length;
	uint8_t sequence_number;
	const uint8_t * data;
} frame_t ;

#endif // FRAME_H
