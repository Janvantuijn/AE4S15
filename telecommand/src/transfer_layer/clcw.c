#include "clcw.h"

void clcw_init(clcw_t * clcw) {
    clcw->CLCW_VERSION = 0b01;
    clcw->RETRANSMIT = 0b0;
    clcw->RESERVED = 0b00000;
    clcw->FRAME_SEQUENCE = 0b00000000;
}
