/*
 * utility.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "utility.h"

const char* option_handler(int argc, char* argv[])
{
    int opt;
    int flag_b = 0;
    char* folder_name_p = NULL;
    while(-1 != (opt = getopt(argc, argv, ":f:u:")))
    {
        switch(opt)
        {
            case 'f':
                return optarg;
            case 'u':
                printf("unpack %s\n", optarg);
                folder_name_p = malloc(strlen(optarg) + 1);
                strcpy(folder_name_p, optarg);
                flag_b = 1;
                break;
            case ':':
                printf("error %c\n", optopt);
                folder_name_p = "output";
                break;
            default:
            break;
        }
    }
    mkdir(folder_name_p, S_IRWXU | S_IRWXG | S_IRWXO);
    if(flag_b)
    {
        free(folder_name_p);
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

