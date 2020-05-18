#include "code_block.h"

size_t code_block_size(const code_block_t * block) {
    size_t size = block->size;
    size += sizeof(block->error_control_field);
    return size;
}
