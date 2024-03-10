/*
 * midi.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#include "midi.h"
#include "dx7.h"
#include "utility.h"

uint8_t* get_sysex_payload(FILE* file_p, int* size_p)
{
    int counter = 0;
    int byte    = getc(file_p);
    counter    += (byte != EOF);
    uint8_t* buffer_p = NULL;

    if(byte == MIDI_SYSTEM_EXCLUSIVE)
    {
        long int position_start = ftell(file_p);

        for(byte = getc(file_p); byte != MIDI_EOX; byte = getc(file_p))
        {
            if(byte != EOF)
            {
                ++counter;
            }
            else
            {
                counter = EOF;
                break;
            }
        }

        long int position_end = ftell(file_p);
        size_t payload_size = position_end - position_start - 1;
        buffer_p = malloc(payload_size);
        fseek(file_p, position_start, SEEK_SET);
        fread(buffer_p, sizeof(uint8_t), payload_size, file_p);

        fseek(file_p, position_end, SEEK_SET);
    }

    if(size_p)
    {
        *size_p = counter;
    }

    return buffer_p;
}

