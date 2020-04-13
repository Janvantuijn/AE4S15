#include "cltu.h"

void cltu_init(cltu_t * cltu) {
    cltu->start_seq = CLTU_START_SEQUENCE;
}
    
bool cltu_start_seq_check(cltu_t * cltu) {
    if (cltu->start_seq != CLTU_START_SEQUENCE) {
        return false;
    } else {
        return true;
    }
}