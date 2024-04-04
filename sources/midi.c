/*
 * midi.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#include "midi.h"

uint8_t* get_next_sysex_payload(FILE* file_p, int* size_p)
{
    int counter = 0;
    int byte;
    do
    {
        byte = getc(file_p);
        if(byte == EOF)
        {
            break;
        }
        counter++;
    } while(byte != MIDI_SYSTEM_EXCLUSIVE);

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

int write_sysex_payload(FILE* file_p,
                        const uint8_t* payload_p,
                        size_t payload_length)
{
    size_t count = 0;
    uint8_t byte = MIDI_SYSTEM_EXCLUSIVE;
    count += fwrite(&byte, sizeof(uint8_t), 1, file_p);
    count += fwrite(payload_p, sizeof(uint8_t), payload_length, file_p);
    byte = MIDI_EOX;
    count += fwrite(&byte, sizeof(uint8_t), 1, file_p);
    return count;
}

