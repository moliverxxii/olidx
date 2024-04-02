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
#include "help.h"
#include "midi.h"

const char* option_handler(int argc, char* argv[])
{
    int opt;
    int flag_b = 0;
    char* folder_name_p = NULL;
    char* file_name_p = NULL;
    while(-1 != (opt = getopt(argc, argv, ":f:hu:")))
    {
        switch(opt)
        {
            case 'f':
                file_name_p = optarg;
            break;
            case 'h':
                printf("%s", get_help());
            break;
            case 'u':
                if(!flag_b)
                {
                    printf("unpack %s\n", optarg);
                    folder_name_p = malloc(strlen(optarg) + 1);
                    strcpy(folder_name_p, optarg);
                }
                else
                {
                    printf("can't have more than one file to unpack\n");
                }
                flag_b = 1;
            break;
            case ':':
                printf("error %c\n", optopt);
                if(optopt == 'u')
                {
                    if(!flag_b)
                    {
                        folder_name_p = "output";
                    }
                    else
                    {
                        printf("ignoring useless command\n");
                    }
                }
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

    return file_name_p;
}


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
