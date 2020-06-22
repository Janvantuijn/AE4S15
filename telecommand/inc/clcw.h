#ifndef CLCW_H
#define CLCW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint8_t CLCW_VERSION   : 2; // Official version is 00 but since we modified a lot lets use 01
    uint8_t RESERVED       : 6; // Not used
    uint8_t FRAME_SEQUENCE : 8; // Frame sequence number 
} clcw_t;

void clcw_init(clcw_t * clcw);

#endif // CLCW_H
