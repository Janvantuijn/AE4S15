#include "buffer.h"

void buffer_init(buffer_t * b, uint8_t * arr)
{
	b->array = arr;
	b->front = 0;
	b->size = 0;
	b->rear = -1;
}

uint8_t buffer_peek(const buffer_t * b) {
    return b->array[b->front];
}

bool buffer_is_empty(const buffer_t * b) {
    return b->size == 0;
}

bool buffer_is_full(const buffer_t * b) {
    return b->size == BUFFER_SIZE;
}

void buffer_insert(buffer_t * b, uint8_t data) {
  if (!buffer_is_full(b)) {
      if (b->rear == BUFFER_SIZE - 1) {
        b->rear = -1;
      }

      b->array[++(b->rear)] = data;
      b->size++;
  }
}

uint8_t buffer_remove(buffer_t * b) {
    uint8_t data = b->array[(b->front)++];

    if (b->front == BUFFER_SIZE) {
        b->front = 0;
    }

    b->size--;
    return data;
}

void buffer_clear(buffer_t * b) {
	b->front = 0;
	b->size = 0;
	b->rear = -1;
}
