#include "cltu.h"

void cltu_init(cltu_t * cltu) {
    cltu->start_seq = CLTU_START_SEQUENCE;
    cltu->size = 0;
    // TODO: tail_seq
}

void cltu_insert(cltu_t * cltu, uint8_t data, size_t size) {
    
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

