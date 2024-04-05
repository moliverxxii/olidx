/*
 * utility.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */
#include "utility.h"
#include "midi.h"

int get_checksum(const void* buffer, size_t buffer_size)
{
    uint32_t checksum = 0;
    const uint8_t* byte_p = buffer;
    size_t position = 0;
    for(position = 0; position < buffer_size; ++position)
    {
        checksum += (uint32_t) byte_p[position];
    }
    checksum = (checksum & 0x7F);
    return checksum;
}

uint8_t generate_checksum(const void* buffer, size_t buffze_size)
{
    uint8_t checksum = get_checksum(buffer, buffze_size);
    return 0x7F & ~checksum;
}

uint16_t get_payload_size(TwoByte_t byte_count)
{
    return ((uint16_t) byte_count.msb << 7) + (uint16_t) byte_count.lsb;
}

TwoByte_t format_payload_size(size_t size)
{
    TwoByte_t byte_count = {0, 0};
    byte_count.lsb = size & MIDI_DATA_MASK;
    byte_count.msb = size >> MIDI_DATA_BITS;
    return byte_count;
}
