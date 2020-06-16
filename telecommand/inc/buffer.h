#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 0x400

typedef struct {
    uint8_t * array;
    size_t size;
} buffer_t;

void buffer_init(buffer_t * b, uint8_t * arr);
bool buffer_is_empty(const buffer_t * b);
bool buffer_is_full(const buffer_t * b);
void buffer_insert(buffer_t * b, uint8_t data);
void buffer_clear(buffer_t * b);

#endif // BUFFER_H
