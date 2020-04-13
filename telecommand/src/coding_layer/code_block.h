#ifndef CODE_BLOCK_H
#define CODE_BLOCK_H

#include <stdint.h>

#define CODE_BLOCK_LENGTH 4

typedef struct {
	uint8_t info_field[CODE_BLOCK_LENGTH];
	uint8_t error_control_field;
} code_block_t;


#endif // CODE_BLOCK_H