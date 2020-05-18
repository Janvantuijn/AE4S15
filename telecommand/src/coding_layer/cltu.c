#include "cltu.h"

void cltu_init(cltu_t * cltu) {
    cltu->start_seq = CLTU_START_SEQUENCE;
    cltu->size = 0;
    for (int i = 0; i < MAX_CODE_BLOCKS; i++) {
        cltu->code_blocks[i].size = 0;
    }
    // TODO: tail_seq
}

void cltu_clear(cltu_t * cltu) {
    cltu->size = 0;
    for (int i = 0; i < MAX_CODE_BLOCKS; i++) {
        cltu->code_blocks[i].size = 0;
    }
}

void cltu_insert(cltu_t * cltu, uint8_t * data, size_t size) {
    if (size == 0) {
        return;
    }

    for (int j = 0; j < size / CODE_BLOCK_LENGTH + 1; j++) {
    	code_block_t * block = &cltu->code_blocks[j];
    	for (int i = 0; i < CODE_BLOCK_LENGTH && j * CODE_BLOCK_LENGTH + i < size; i++) {
    		block->info_field[i] = data[j * CODE_BLOCK_LENGTH + i];
    		block->size++;
    	}
    	cltu->size++;
    }
}

bool cltu_get_data(const cltu_t * cltu, size_t index, uint8_t * data) {
	int j = index / CODE_BLOCK_LENGTH;
    if (j >= cltu->size) {return false;}

	int i = index - j * CODE_BLOCK_LENGTH;
    if (i >= cltu->code_blocks[j].size) {return false;}

	*data = cltu->code_blocks[j].info_field[i];
    return true;
}
    
bool cltu_start_seq_check(const cltu_t * cltu) {
    if (cltu->start_seq != CLTU_START_SEQUENCE) {
        return false;
    } else {
        return true;
    }
}

bool cltu_tail_seq_check(const cltu_t * cltu) {
    return false;
}

/* Total size of this CLTU in bytes */
size_t cltu_size(const cltu_t * cltu) {
    size_t size = sizeof(cltu->start_seq);
    size += cltu_code_block_size(cltu);
    size += sizeof(cltu->tail_seq);
    return size;
}

/* Size of all code blocks of this CLTU in bytes */
size_t cltu_code_block_size(const cltu_t * cltu) {
    size_t size = 0;
    for (int i = 0; i < cltu->size; i++) {
        size += code_block_size(&cltu->code_blocks[i]);
    }
    return size;
}

