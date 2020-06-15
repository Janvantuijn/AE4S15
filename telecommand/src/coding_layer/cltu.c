#include "cltu.h"
#include "crc.h"

/**
 * @brief Initializes the cltu block
 * 
 * @param cltu, pointer to cltu block that will be initialized
 */
void cltu_init(cltu_t * cltu) {
    cltu->start_seq = CLTU_START_SEQUENCE;
    cltu->size = 0;
    cltu->block_index = 0;
}

/**
 * @brief Clears the content of the cltu block
 * 
 * @param cltu, pointer to cltu block that will be cleared
 */
void cltu_clear(cltu_t * cltu) {
    cltu->size = 0;
    cltu->block_index = 0;

    for (int i = 0; i < CODE_BLOCK_LENGTH; i++) {
    	cltu->tail_seq.info_field[i] = 0;
    }
}

void cltu_insert_byte(cltu_t * cltu, const uint8_t data) {
	if (cltu->size == 0) {
		cltu->size = 1;
	} else if (cltu->block_index == CODE_BLOCK_LENGTH) {
		cltu->size++;
		cltu->block_index = 0;
	}

	code_block_t * current_block = &(cltu->code_blocks[cltu->size - 1]);
	current_block->info_field[cltu->block_index++] = data;

	// Update checksum of this block
	current_block->error_control_field = crc(current_block->info_field, CODE_BLOCK_LENGTH);
}

/**
 * @brief Copies the given data array to the given cltu 
 * 
 * @param cltu, pointer to cltu block that will be given data
 * @param data, pointer to data array that will be copied to cltu 
 * @param size, size of of the data array
 */
void cltu_insert_bytes(cltu_t * cltu, const uint8_t * data, const size_t size) {
    for (int i = 0; i < size; i++) {
    	cltu_insert_byte(cltu, data[i]);
    }
}

void cltu_calc_tail_sequence(cltu_t * cltu) {
	code_block_t * tail_sequence = &(cltu->tail_seq);
	tail_sequence->error_control_field = crc(tail_sequence->info_field, CODE_BLOCK_LENGTH);

	// Intentionally insert faulty bits in the tail sequence
	for (int i = 0; i < CODE_BLOCK_LENGTH; i++) {
		tail_sequence->info_field[i] += 1;
	}
}

/**
 * @brief Checks if the start sequence of the cltu is correct
 * 
 * @param cltu, pointer to the cltu whose start sequence needs to be checked
 * @return true, returned when the start sequence is correct
 * @return false, returned when the start sequence is incorrect
 */
bool cltu_start_seq_check(const cltu_t * cltu) {
    if (cltu->start_seq != CLTU_START_SEQUENCE) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief Checks if the tail sequence of the cltu is correct
 * 
 * @param cltu, pointer to the cltu whose tail sequence needs to be checked
 * @return true, returned when the tail sequence is correct
 * @return false, returned when the tail sequence is incorrect.
 */
bool cltu_tail_seq_check(const cltu_t * cltu) {
    return false;
}
