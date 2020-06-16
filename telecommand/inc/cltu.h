#ifndef CLTU_H
#define CLTU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "code_block.h"

#define CLTU_START_SEQUENCE 0xEB90
#define CLTU_START_SEQUENCE_MSB 0xEB
#define CLTU_START_SEQUENCE_LSB 0x90

#define MAX_CODE_BLOCKS 64

typedef struct {
	uint8_t size; // Number of codes blocks
	uint8_t block_index; // Index of last byte of last code block
	uint16_t start_seq;
	code_block_t code_blocks[MAX_CODE_BLOCKS];
	code_block_t tail_seq;
} cltu_t;

void cltu_init(cltu_t * cltu);
void cltu_clear(cltu_t * cltu);

void cltu_insert_byte(cltu_t * cltu, const uint8_t data);
void cltu_insert_bytes(cltu_t * cltu, const uint8_t * data, const size_t size);
void cltu_calc_tail_sequence(cltu_t * cltu);

bool cltu_start_seq_check(const cltu_t * cltu);
bool cltu_tail_seq_check(const cltu_t * cltu);

#endif // CLTU_H
