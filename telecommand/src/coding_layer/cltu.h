#ifndef CLTU_H
#define CLTU_H

#include <stdint.h>
#include <stdbool.h>
#include "code_block.h"

#define CLTU_START_SEQUENCE 0xEB90

typedef struct {
	uint16_t start_seq;
	code_block_t code_block;
	uint8_t tail_seq;
} cltu_t;

void cltu_init(cltu_t * cltu);
bool cltu_start_seq_check(cltu_t * cltu);

#endif // CLTU_H
