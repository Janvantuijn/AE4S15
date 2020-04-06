#ifndef CLTU_H
#define CLTU_H

#include <stdint.h>
#include "code_block.h"

#define START_SEQ 0xEB90

typedef struct {
	uint16_t start_seq;
	code_block_t code_block;
	uint8_t tail_seq;
} cltu_t;

#endif // CLTU_H
