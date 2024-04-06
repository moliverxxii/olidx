#include <stdlib.h>
#include <stdint.h>

#ifndef UTILITY_H
#define UTILITY_H

#define GET_AMOUNT(BITS) (2 << BITS)
#define OK(MESSAGE) printf("ok number " #MESSAGE "!\n")

/**
 * Most Significant Byte first!
 */
typedef struct TwoByte_t
{
    uint8_t msb;
    uint8_t lsb;
} TwoByte_t;

int get_checksum(const void* buffer, size_t buffer_size);
uint8_t generate_checksum(const void* buffer, size_t buffze_size);
uint16_t get_payload_size(TwoByte_t byte_count);
TwoByte_t format_payload_size(size_t size);

char* append_counter(char** text_pp, uint8_t counter);
char* append_str(char** text_pp, const char* appendage_p);
const char* path_to_file_name(const char* path_p);
char* get_extension(const char* path_p);
int is_extension_valid(const char* path_p, const char* extension);


#endif /* UTILITY_H */
