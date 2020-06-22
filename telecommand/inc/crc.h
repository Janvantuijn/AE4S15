#include <stdint.h>
#include <stddef.h>

uint16_t crc(uint8_t * buf, size_t len);
void crc_set_offset(uint16_t offset);
