/*
 * dx7.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#include <string.h>

#include "dx7.h"
#include "midi.h"
#include "utility.h"



const char* const SYSEX_TYPE_NAME_TABLE[SYSEX_TYPE_COUNT] =
{
    "Bulk data",
    "Parameter"
};

const char* const BULK_DATA_FORMAT_NAME_TABLE[BULK_DATA_FORMAT_COUNT] =
{
    "Voice edit buffer",
    "Supplement edit buffer",
    "Packed 32 supplement",
    "Packed 32 voice",
    "Universal bulk dump",
    "Bulk error"
};

const char* const UNIVERSAL_BULK_DATA_CLASSIFICATION_NAME = "LM  ";

const char* const UNIVERSAL_BULK_DATA_FORMAT_TABLE[UNIVERSAL_BULK_DATA_COUNT] =
{
    "8973PE", //x1
    "8973PM", //x1
    "8973S ", //x1
    "MCRYE ", //x1
    "MCRYM0", //x1
    "MCRYM1", //x1
    "MCRYC ", //x64
    "FKSYE ", //x1
    "FKSYC "  //x32
};


const char* const UNIVERSAL_BULK_DATA_NAME_TABLE[UNIVERSAL_BULK_DATA_COUNT] =
{
    "Performance edit buffer",
    "Packed 32 performance",
    "System set up",
    "Micro tuning edit_buffer",
    "Micro tuning memory 0",
    "Micro tuning memory 1",
    "Micro tuning cartridge",
    "Fractional scaling edit buffer",
    "Fractional scaling cartridge"
};

const uint8_t BULK_DATA_FORMAT_TABLE[BULK_DATA_FORMAT_COUNT] =
{
    BULK_DATA_FORMAT_VOICE_EDIT_BUFFER,
    BULK_DATA_FORMAT_SUPPLEMENT_EDIT_BUFFER,
    BULK_DATA_FORMAT_PACKED_32_SUPPLEMENT,
    BULK_DATA_FORMAT_PACKED_32_VOICE,
    BULK_DATA_FORMAT_UNIVERSAL_BULK_DUMP
};

const size_t UNIVERSAL_BULK_DATA_CLASSIFICATION_SIZE = 4U;
const size_t UNIVERSAL_BULK_DATA_FORMAT_SIZE         = 6U;

const SysexHeader_t SYSEX_HEADER_INITIALISER =
{
    0,
    0,
    0
};

const ParameterChangeHeader_t PARAMETER_HEADER_INITIALISER =
{
    0,
    0,
    0
};

const BulkDataHeader_t BULK_HEADER_INITIALISER =
{
    0
};

void process_sysex_data(const void* data_p)
{
    const SysexHeader_t* sysex_header_p = data_p;
    data_p += sizeof(SysexHeader_t);
    SysexType_t type = get_header_info(sysex_header_p);

    printf("Sysex type: %s\n", SYSEX_TYPE_NAME_TABLE[type]);

    BulkData_t bulk_type = BULK_DATA_MALFORMED;


    switch(type)
    {
        case SYSEX_TYPE_PARAMETER:
        {
            const ParameterChangeHeader_t* parameter_header_p = data_p;
            data_p += sizeof(ParameterChangeHeader_t);
            printf("\
Parameter group:   %01hhu%01hhu\n\
Parameter number: %3hhu\n",
                   parameter_header_p->group_h,
                   parameter_header_p->group_l,
                   parameter_header_p->parameter);
        }
        break;
        case SYSEX_TYPE_BULK:
        {
            const BulkDataHeader_t* bulk_header_p = data_p;
            data_p += sizeof(BulkDataHeader_t);
            bulk_type = get_bulk_data_header_info(bulk_header_p);
            if(type == BULK_DATA_MALFORMED)
            {
                return;
            }
        }
        break;
        default:
            return;
        break;
    }


    if((type == SYSEX_TYPE_BULK) && (bulk_type != BULK_DATA_UNIVERSAL_BULK_DUMP))
    {
        const ByteCount_t* byte_count_p = data_p;
        data_p += sizeof(ByteCount_t);
        uint16_t payload_size = get_payload_size(*byte_count_p);
        printf("Payload size:   %huB\n",payload_size);

        void* payload_p;
        Packed32Voice_t voices;
        if(bulk_type == BULK_DATA_PACKED_32_VOICE)
        {
            for(int voice = 0; voice < VOICE_COUNT; voice++)
            {
                voices[voice] = *(PackedVoiceParameters_t*) data_p;
                data_p += sizeof(PackedVoiceParameters_t);
                printf("%3$2d: %1$.*2$s\n",
                       voices[voice].voice_name,
                       VOICE_NAME_SIZE,
                       1+voice);
            }
            payload_p = voices;
        }
        else
        {
            payload_p = malloc(payload_size);
            memcpy(payload_p, data_p, payload_size);
            data_p += payload_size;
        }

        int checksum = get_checksum(payload_p, payload_size);
        int byte = *((uint8_t*) data_p);
        ++data_p;

        if(bulk_type != BULK_DATA_PACKED_32_VOICE)
        {
            free(payload_p);
        }
        printf("checksum: %3d + %3d = %3d\n", checksum, byte, checksum + byte);
    }

}


SysexType_t get_header_info(const SysexHeader_t* header_p)
{
    printf("Manufacturer: %#4x\n", header_p->id);
    printf("Substatus:    %#4x\n", header_p->substatus);
    printf("Device id:    %#4x\n", header_p->device);
    return header_p->substatus;
}

BulkData_t get_bulk_data_header_info(const BulkDataHeader_t* header_p)
{
    BulkData_t type;
    switch(header_p->format)
    {
        case BULK_DATA_FORMAT_VOICE_EDIT_BUFFER:
            type = BULK_DATA_VOICE_EDIT_BUFFER;
        break;
        case BULK_DATA_FORMAT_SUPPLEMENT_EDIT_BUFFER:
            type = BULK_DATA_SUPPLEMENT_EDIT_BUFFER;
        break;
        case BULK_DATA_FORMAT_PACKED_32_SUPPLEMENT:
            type = BULK_DATA_PACKED_32_SUPPLEMENT;
        break;
        case BULK_DATA_FORMAT_PACKED_32_VOICE:
            type = BULK_DATA_PACKED_32_VOICE;
        break;
        case BULK_DATA_FORMAT_UNIVERSAL_BULK_DUMP:
            type = BULK_DATA_UNIVERSAL_BULK_DUMP;
        break;
        default:
            type = BULK_DATA_MALFORMED;
        break;
    }
    printf("%s\n", BULK_DATA_FORMAT_NAME_TABLE[type]);
    return type;
}


