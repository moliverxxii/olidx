/*
 * utility.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */
#include <string.h>
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

char* append_str(char** text_pp, const char* appendage_p)
{
    if(strlen(appendage_p) > 0)
    {
        size_t length = strlen(*text_pp) + strlen(appendage_p);
        char* new_text_p = malloc(length + 1);
        strcpy(new_text_p, *text_pp);
        strcat(new_text_p, appendage_p);
        free(*text_pp);
        *text_pp = new_text_p;
    }
    return *text_pp;
}

char* append_counter(char** text_pp, uint8_t counter)
{

    char suffix[5] = {0};
    sprintf(suffix, "_%03hhu", counter);
    return append_str(text_pp, suffix);
}

const char* path_to_file_name(const char* path_p)
{
    char* root_p = strrchr(path_p, '/');
    return (root_p == NULL)? path_p : root_p + 1;
}

char* get_extension(const char* path_p)
{

    char* extension_start_p = strrchr(path_p, '.');
    if(strchr(extension_start_p, '/') != NULL)
    {
        return NULL;
    }
    {
        return extension_start_p;
    }
}


int is_extension_valid(const char* path_p, const char* extension_p)
{
    const char* extension_start_p = get_extension(path_p);
    if(extension_start_p == NULL)
    {
        return 0;
    }
    else
    {
        return strcmp(extension_start_p, extension_p) == 0;
    }
}
