#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 1000

typedef struct {
    uint8_t * array;
    size_t size;
    int front;
    int rear;
} buffer_t;

void buffer_init(buffer_t * b, uint8_t * arr);
uint8_t buffer_peek(const buffer_t * b);
bool buffer_is_empty(const buffer_t * b);
bool buffer_is_full(const buffer_t * b);
void buffer_insert(buffer_t * b, uint8_t data);
uint8_t buffer_remove(buffer_t * b);
void buffer_clear(buffer_t * b);

#endif // BUFFER_H
