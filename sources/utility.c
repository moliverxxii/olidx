/*
 * utility.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */
#include <unistd.h>

#include "utility.h"

const char* option_handler(int argc, char* argv[])
{
    int opt;
    while(-1 != (opt = getopt(argc, argv, "f:")))
    {
        switch(opt)
        {
            case 'f':
                return optarg;
            default:
            break;
        }
    }
    return NULL;
}


int get_checksum(void* buffer, size_t buffer_size)
{
    uint32_t checksum = 0;
    uint8_t* byte_p = buffer;
    size_t position = 0;
    for(position = 0; position < buffer_size; ++position)
    {
        checksum += (uint32_t) byte_p[position];
    }
    checksum = (checksum & 0x7F);
    return checksum;
}

uint8_t generate_checksum(void *buffer, size_t buffze_size)
{
    uint8_t checksum = get_checksum(buffer, buffze_size);
    return 0x7F & ~checksum;
}

uint16_t get_payload_size(ByteCount_t byte_count)
{
    return ((uint16_t) byte_count.msb << 7) + (uint16_t) byte_count.lsb;
}

