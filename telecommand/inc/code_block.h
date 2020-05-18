#ifndef CODE_BLOCK_H
#define CODE_BLOCK_H

#include <stdint.h>
#include <stddef.h>

#define CODE_BLOCK_LENGTH 4

typedef struct {
	uint8_t info_field[CODE_BLOCK_LENGTH];
	uint8_t error_control_field;
	size_t size;
} code_block_t;

size_t code_block_size(const code_block_t * block);

#endif // CODE_BLOCK_H
