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

const size_t BULK_DATA_BYTE_COUNT_TABLE[BULK_DATA_FORMAT_COUNT] =
{
    BYTE_COUNT_VOICE_EDIT_BUFFER,
    BYTE_COUNT_SUPPLEMENT_EDIT_BUFFER,
    BYTE_COUNT_PACKED_32_SUPPLEMENT,
    BYTE_COUNT_PACKED_32_VOICE,
    0
};

const size_t UNIVERSAL_BULK_DATA_BYTE_COUNT_TABLE[UNIVERSAL_BULK_DATA_COUNT] =
{
    BYTE_COUNT_PERFORMANCE_EDIT_BUFFER,
    BYTE_COUNT_PACKED_32_PERFORMANCE,
    BYTE_COUNT_SYSTEM_SET_UP,
    BYTE_COUNT_MICRO_TUNING,
    BYTE_COUNT_MICRO_TUNING,
    BYTE_COUNT_MICRO_TUNING,
    BYTE_COUNT_MICRO_TUNING,
    BYTE_COUNT_FRACTIONAL_SCALING,
    BYTE_COUNT_FRACTIONAL_SCALING
};

const size_t UNIVERSAL_BULK_DATA_REPEAT_TABLE[UNIVERSAL_BULK_DATA_COUNT] =
{
    REPEAT_PERFORMANCE_EDIT_BUFFER,
    REPEAT_PACKED_32_PERFORMANCE,
    REPEAT_SYSTEM_SET_UP,
    REPEAT_MICRO_TUNING,
    REPEAT_MICRO_TUNING,
    REPEAT_MICRO_TUNING,
    REPEAT_MICRO_TUNING_CARTRIDGE,
    REPEAT_FRACTIONAL_SCALING_EDIT_BUFFER,
    REPEAT_FRACTIONAL_SCALING_CARTRIDGE
};


const SysexHeader_t SYSEX_HEADER_INITIALISER =
{
    0,
    0,
    0
};

const SysexHeader_t SYSEX_HEADER_INITIALISER_YAMAHA =
{
    0x43,
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

const UniversalBulkDataHeader_t UNIVERSAL_BULK_HEADER_INITIALISER =
{
    {0},
    {0}
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
            printf("Parameter group:   %01hhu%01hhu\n"
                   "Parameter number: %3hhu\n",
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
            if(bulk_type == BULK_DATA_MALFORMED)
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
        const TwoByte_t* byte_count_p = data_p;
        data_p += sizeof(TwoByte_t);
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

uint8_t* format_dx7_sysex(const SysExData_t* sysex_data_p, size_t* length_p, uint8_t device_id)
{
    if(sysex_data_p == NULL)
    {
        return NULL;
    }
    SysexHeader_t header = SYSEX_HEADER_INITIALISER_YAMAHA;
    header.device = device_id;

    BulkDataHeader_t bulk_header;
    ParameterChangeHeader_t parameter_header;
    switch(sysex_data_p->type)
    {
        case SYSEX_TYPE_BULK:
            header.substatus = 0;
            bulk_header.format = BULK_DATA_FORMAT_TABLE[sysex_data_p->bulk_data.type];
        break;
        case SYSEX_TYPE_PARAMETER:
            //TODO: parameters.
            header.substatus = 1;
        break;
        default:
        break;
    }
    return NULL;
}


uint8_t* format_dx7_bulk_payload(const void* data_p,
                                 size_t data_length,
                                 size_t* format_length_p)
{
    size_t format_length = sizeof(TwoByte_t) + data_length + sizeof(uint8_t);
    uint8_t* wrapped_data_p = malloc(format_length);
    if(wrapped_data_p)
    {
        *(TwoByte_t*) wrapped_data_p = format_payload_size(data_length);
        memcpy(wrapped_data_p + sizeof(TwoByte_t), data_p, data_length);
        *(wrapped_data_p + sizeof(TwoByte_t) + data_length) = generate_checksum(data_p, data_length);
        if(format_length_p)
        {
            *format_length_p = format_length;
        }
    }
    return wrapped_data_p;
}

uint8_t* format_dx7_universal_bulk_payload(const void* data_p,
                                           size_t data_length_p,
                                           size_t* format_length_p)
{
    return NULL;
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

PackedVoiceParameters_t pack_voice_parameters(VoiceParameters_t parameters)
{
    PackedVoiceParameters_t packed_parameters;

    for(int operator = OPERATOR_1; operator <= OPERATOR_6; ++operator)
    {
       PackedOperatorParameters_t* packed_operator_p = packed_parameters.Operator + operator;
       OperatorParameters_t*       operator_p        = parameters.Operator        + operator;
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_rate_1);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_rate_2);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_rate_3);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_rate_4);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_level_1);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_level_2);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_level_3);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, eg_level_4);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, break_point);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, left_depth);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, right_depth);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, left_curve);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, right_curve);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, rate_scaling);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, modulation_sensitivity);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, touch_sensitivity);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, total_level);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, frequency_mode);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, frequency_coarse);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, frequency_fine);
       CONVERT_STRUCT_PARAMETER(*operator_p, *packed_operator_p, detune);
    }
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_rate_1);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_rate_2);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_rate_3);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_rate_4);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_level_1);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_level_2);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_level_3);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, peg_level_4);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, algorithm);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, feedback_level);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, oscillator_phase_init);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, lfo_speed);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, lfo_delay_time);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, pitch_modulation_depth);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, amplitude_modulation_depth);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, lfo_key_sync);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, lfo_wave);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, lfo_pitch_modulation_sensitivity);
    CONVERT_STRUCT_PARAMETER(parameters, packed_parameters, transpose);

    return packed_parameters;
}

VoiceParameters_t unpack_voice_parameters(PackedVoiceParameters_t parameters)
{
    VoiceParameters_t unpacked_parameters;
    for(int operator = OPERATOR_1; operator <= OPERATOR_6; ++operator)
    {
       OperatorParameters_t*       unpacked_operator_p = unpacked_parameters.Operator + operator;
       PackedOperatorParameters_t* operator_p          = parameters.Operator          + operator;
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_rate_1);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_rate_2);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_rate_3);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_rate_4);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_level_1);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_level_2);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_level_3);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, eg_level_4);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, break_point);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, left_depth);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, right_depth);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, left_curve);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, right_curve);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, rate_scaling);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, modulation_sensitivity);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, touch_sensitivity);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, total_level);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, frequency_mode);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, frequency_coarse);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, frequency_fine);
       CONVERT_STRUCT_PARAMETER(*operator_p, *unpacked_operator_p, detune);
    }
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_rate_1);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_rate_2);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_rate_3);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_rate_4);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_level_1);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_level_2);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_level_3);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, peg_level_4);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, algorithm);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, feedback_level);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, oscillator_phase_init);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, lfo_speed);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, lfo_delay_time);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, pitch_modulation_depth);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, amplitude_modulation_depth);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, lfo_key_sync);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, lfo_wave);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, lfo_pitch_modulation_sensitivity);
    CONVERT_STRUCT_PARAMETER(parameters, unpacked_parameters, transpose);

    return unpacked_parameters;
}

