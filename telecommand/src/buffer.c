#include "buffer.h"

void buffer_init(buffer_t * b, uint8_t * arr)
{
	b->array = arr;
	b->size = 0;
}

uint8_t buffer_peek(const buffer_t * b) {
    return b->array[0];
}

bool buffer_is_empty(const buffer_t * b) {
    return b->size == 0;
}

bool buffer_is_full(const buffer_t * b) {
    return b->size == BUFFER_SIZE;
}

void buffer_insert(buffer_t * b, uint8_t data) {
	if (!buffer_is_full(b)) {
	    b->array[b->size++] = data;
    }
}

uint8_t buffer_remove(buffer_t * b) {
    if (!buffer_is_empty(b)) {
    	b->size--;
    }
}

void buffer_clear(buffer_t * b) {
	b->size = 0;
}
