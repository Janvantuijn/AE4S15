#include "clcw.h"

void clcw_init(clcw_t * clcw) {
    clcw->CLCW_VERSION = 0b01;
    clcw->RESERVED = 0b000000;
    clcw->FRAME_SEQUENCE = 0b11111111;
}
