#include <stdlib.h>

#ifndef UTILITY_H
#define UTILITY_H

#define GET_AMOUNT(BITS) (2 << BITS)
/**
 * Most Significant Byte first!
 */
typedef struct TwoByte_t
{
    uint8_t msb;
    uint8_t lsb;
} TwoByte_t;


const char* option_handler(int argc, char* argv[]);

int get_checksum(const void* buffer, size_t buffer_size);
uint8_t generate_checksum(const void* buffer, size_t buffze_size);
uint16_t get_payload_size(TwoByte_t byte_count);
TwoByte_t format_payload_size(size_t size);

#endif /* UTILITY_H */
