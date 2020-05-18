#ifndef CLTU_H
#define CLTU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "code_block.h"

#define CLTU_START_SEQUENCE 0xEB90
#define MAX_CODE_BLOCKS 10

typedef struct {
	uint16_t start_seq;
	code_block_t code_blocks[MAX_CODE_BLOCKS];
	size_t size;
	uint8_t tail_seq;
} cltu_t;

void cltu_init(cltu_t * cltu);
void cltu_clear(cltu_t * cltu);
void cltu_insert(cltu_t * cltu, uint8_t * data, size_t size);
bool cltu_get_data(const cltu_t * cltu, size_t index, uint8_t * data);
bool cltu_start_seq_check(const cltu_t * cltu);
bool cltu_tail_seq_check(const cltu_t * cltu);
size_t cltu_size(const cltu_t * cltu);
size_t cltu_code_block_size(const cltu_t * cltu);

#endif // CLTU_H
