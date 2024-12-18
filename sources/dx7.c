/*
 * dx7.c
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#include <string.h>
#include <ctype.h>

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

const ParameterChangeHeader_t PARAMETER_CHANGE_GROUP_TABLE[PARAMETER_CHANGE_COUNT] =
{
    {1,0,28}, //PARAMETER_CHANGE_VOICE = 0,
    {0,6,73}, //PARAMETER_CHANGE_SUPPLEMENT,
    {0,6,126},//PARAMETER_CHANGE_MICRO_TUNING,
    {0,6,127}, //PARAMETER_CHANGE_FRACTIONAL_SCALING,
    {1,6,52}, //PARAMETER_CHANGE_PERFORMANCE,
    {1,6,83} //PARAMETER_CHANGE_SYSTEM_SET_UP,
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

const size_t PARAMETER_CHANGE_BYTE_COUNT_TABLE[PARAMETER_CHANGE_COUNT] =
{
    SIZE_OF_FIELD(ParameterPayload_t, data), //PARAMETER_CHANGE_VOICE = 0,
    SIZE_OF_FIELD(ParameterPayload_t, data), //PARAMETER_CHANGE_SUPPLEMENT,
    SIZE_OF_FIELD(ParameterPayload_t, micro_tuning), //PARAMETER_CHANGE_MICRO_TUNING,
    SIZE_OF_FIELD(ParameterPayload_t, fractional_scaling_data), //PARAMETER_CHANGE_FRACTIONAL_SCALING,
    SIZE_OF_FIELD(ParameterPayload_t, data), //PARAMETER_CHANGE_PERFORMANCE,
    SIZE_OF_FIELD(ParameterPayload_t, data), //PARAMETER_CHANGE_SYSTEM_SET_UP,
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

const SysExData_t SYSEX_DATA_INITIALISER =
{
    SYSEX_TYPE_BULK,
    {
        {BULK_DATA_MALFORMED, {NULL}}
    }
};

uint8_t* dx7_format_sysex(const SysExData_t* sysex_data_p,
                          size_t* length_p,
                          uint8_t device_id)
{
    if(sysex_data_p == NULL)
    {
        return NULL;
    }
    SysexHeader_t header = SYSEX_HEADER_INITIALISER_YAMAHA;
    header.device = device_id;

    BulkDataHeader_t bulk_header;
    ParameterChangeHeader_t parameter_header;

    void* header_data_p = NULL;
    size_t header_data_length = 0;
    void* payload_p = NULL;
    size_t payload_length = 0;

    switch(sysex_data_p->type)
    {
        case SYSEX_TYPE_BULK:
            header_data_p = &bulk_header;
            header_data_length = sizeof(BulkDataHeader_t);
            header.substatus = 0;
            bulk_header.format = BULK_DATA_FORMAT_TABLE[sysex_data_p->bulk_data.type];
            payload_p = dx7_format_bulk_payload(&sysex_data_p->bulk_data,
                                                &payload_length);
            break;
        case SYSEX_TYPE_PARAMETER:
            parameter_header = dx7_get_parameter_header(&sysex_data_p->parameter_change);
            payload_p = dx7_format_parameter_payload(&sysex_data_p->parameter_change,
                                                     &payload_length);
            header_data_p = &parameter_header;
            header_data_length = sizeof(ParameterChangeHeader_t);

            //TODO: parameters.
            header.substatus = 1;
        break;
        default:
        break;
    }
    size_t sysex_message_length = sizeof(SysexHeader_t)
                                + header_data_length
                                + payload_length;
    uint8_t* sysex_message_p = malloc(sysex_message_length);

    *(SysexHeader_t*) sysex_message_p = header;
    memcpy(sysex_message_p + sizeof(SysexHeader_t),
           header_data_p,
           header_data_length);
    memcpy(sysex_message_p + sizeof(SysexHeader_t) + header_data_length,
           payload_p,
           payload_length);
    free(payload_p);
    if(length_p != NULL)
    {
        *length_p = sysex_message_length;
    }
    return sysex_message_p;
}

SysExData_t* dx7_get_sysex(const uint8_t* payload_p, size_t length)
{
    const uint8_t* head_p = payload_p;
    SysexHeader_t header;
    header = *(SysexHeader_t*) head_p;
    head_p += sizeof(SysexHeader_t);
    SysExData_t* data_p = malloc(sizeof(SysExData_t));
    *data_p = SYSEX_DATA_INITIALISER;
    data_p->type = dx7_get_header(&header);
    printf("Sysex type: %s\n", SYSEX_TYPE_NAME_TABLE[data_p->type]);
    switch(data_p->type)
    {
        case SYSEX_TYPE_PARAMETER:
        {
            data_p->parameter_change = dx7_get_sysex_parameter(head_p);
        }
        break;
        case SYSEX_TYPE_BULK:
        {
            data_p->bulk_data = dx7_get_sysex_bulk_data(head_p);
        }
        break;
        default:
        break;
    }
    return data_p;

}

static uint32_t dx7_get_key(ParameterChangeHeader_t header)
{
    return (((header.group_g * 10) + header.group_h) * 1000) + header.parameter;
}

ParameterPayload_t dx7_get_sysex_parameter(const uint8_t* payload_p)
{
    const uint8_t* head_p = payload_p;
    ParameterChangeHeader_t parameter_header = *(const ParameterChangeHeader_t*) (head_p);
    head_p += sizeof(ParameterChangeHeader_t);
    int parameter_type = PARAMETER_CHANGE_VOICE;
    uint32_t key = dx7_get_key(parameter_header);
    ParameterPayload_t parameter;
    parameter.parameter = PARAMETER_CHANGE_COUNT;
    for(;parameter_type<PARAMETER_CHANGE_COUNT;parameter_type++)
    {
        if(key <= dx7_get_key(PARAMETER_CHANGE_GROUP_TABLE[parameter_type]))
        {
            parameter.parameter = parameter_type;
            memcpy(&parameter.data, head_p, PARAMETER_CHANGE_BYTE_COUNT_TABLE[parameter_type]);
            break;
        }
    }
    printf("Parameter group:   %01hhu,%01hhu\n"
           "Parameter number: %3hhu\n"
           "Key %05d\n",
           parameter_header.group_g,
           parameter_header.group_h,
           parameter_header.parameter,
           key);

    return parameter;
}


BulkDataPayload_t dx7_get_sysex_bulk_data(const uint8_t* payload_p)
{
    BulkDataPayload_t bulk_data;

    const uint8_t* head_p = payload_p;
    const BulkDataHeader_t* bulk_header_p = (const BulkDataHeader_t*) head_p;
    head_p += sizeof(BulkDataHeader_t);

    bulk_data.type = dx7_get_bulk_data_header(bulk_header_p);

    const TwoByte_t* byte_count_p = (const TwoByte_t*) (head_p);
    head_p += sizeof(TwoByte_t);
    uint16_t payload_size = get_payload_size(*byte_count_p);
    printf("Payload size:   %huB\n", payload_size);

    void* bulk_payload_p;
    Packed32Voice_t voices;
    if(bulk_data.type == BULK_DATA_PACKED_32_VOICE)
    {
        for(int voice = 0; voice < VOICE_COUNT; voice++)
        {
            voices[voice] = *(PackedVoiceParameters_t*) head_p;
            head_p += sizeof(PackedVoiceParameters_t);
            printf("%3$2d: %1$.*2$s\n", voices[voice].voice_name,
            VOICE_NAME_SIZE, 1 + voice);
        }
        bulk_payload_p = voices;
    }
    else
    {
        bulk_payload_p = malloc(payload_size);
        memcpy(bulk_payload_p, head_p, payload_size);
        head_p += payload_size;
    }
    bulk_data.payload_p = malloc(payload_size);
    memcpy(bulk_data.payload_p, bulk_payload_p, payload_size);

    int checksum = get_checksum(bulk_payload_p, payload_size);
    int byte = *((uint8_t*) head_p);
    ++head_p;

    if(bulk_data.type != BULK_DATA_PACKED_32_VOICE)
    {
        free(bulk_payload_p);
    }
    printf("checksum: %3d + %3d = %3d\n", checksum, byte, checksum + byte);
    return bulk_data;
}


uint8_t* dx7_format_bulk_payload(const BulkDataPayload_t* bulk_data_p,
                                 size_t* length_p)
{
    void* payload_p = NULL;
    const void* data_p = NULL;
    switch(bulk_data_p->type)
    {
    case BULK_DATA_VOICE_EDIT_BUFFER:
    case BULK_DATA_SUPPLEMENT_EDIT_BUFFER:
        data_p = bulk_data_p->payload_p;
        break;
    case BULK_DATA_PACKED_32_SUPPLEMENT:
    case BULK_DATA_PACKED_32_VOICE:
        data_p = *(void**) bulk_data_p->payload_p;
        break;
    case BULK_DATA_UNIVERSAL_BULK_DUMP:
        payload_p = dx7_format_universal_bulk_payload(&bulk_data_p->universal,
                                                      length_p);
    break;
    default:
        return NULL;
        break;
    }
    if(bulk_data_p->type != BULK_DATA_UNIVERSAL_BULK_DUMP)
    {
        payload_p = dx7_wrap_bulk_payload(data_p,
                                          BULK_DATA_BYTE_COUNT_TABLE[bulk_data_p->type],
                                          length_p);

    }
    return payload_p;
}

uint8_t* dx7_format_universal_bulk_payload(const UniversalBulkDataPayload_t* data_p,
                                           size_t* data_length_p)
{
    uint8_t* payload_p = NULL;
    size_t payload_length = 0;
    int repeat;
    uint8_t** block_payload_pp = malloc(UNIVERSAL_BULK_DATA_REPEAT_TABLE[data_p->type] * sizeof(uint8_t*));
    int repeat_count = UNIVERSAL_BULK_DATA_REPEAT_TABLE[data_p->type];
    for(repeat = 0;
        repeat < repeat_count;
        ++repeat)
    {
        UniversalBulkDataHeader_t universal_bulk_header;
        strncpy(universal_bulk_header.classification,
                UNIVERSAL_BULK_DATA_CLASSIFICATION_NAME,
                UNIVERSAL_BULK_DATA_CLASSIFICATION_SIZE);
        strncpy(universal_bulk_header.format,
                UNIVERSAL_BULK_DATA_FORMAT_TABLE[data_p->type],
                UNIVERSAL_BULK_DATA_FORMAT_SIZE);
        size_t format_length = UNIVERSAL_BULK_DATA_BYTE_COUNT_TABLE[data_p->type] + sizeof(UniversalBulkDataHeader_t);
        uint8_t* block_payload_p = malloc(format_length);
        memcpy(block_payload_p,
               &universal_bulk_header,
               sizeof(UniversalBulkDataHeader_t));
        const void* block_p = (repeat_count > 1) //si il y a plusieurs repetitions, nous avons un pointeur vers un tableau.
                             ? * (void**) data_p->payload_p + repeat * UNIVERSAL_BULK_DATA_BYTE_COUNT_TABLE[data_p->type]
                             : data_p->payload_p;
        memcpy(block_payload_p, block_p, UNIVERSAL_BULK_DATA_BYTE_COUNT_TABLE[data_p->type]);
        size_t length = 0;
        block_payload_pp[repeat] = dx7_wrap_bulk_payload(block_payload_p,
                                                         format_length,
                                                         &length);
        payload_length += length;
        free(block_payload_p);
    }
    payload_p = malloc(payload_length);
    size_t block_length = payload_length/repeat_count;
    uint8_t* head_p;
    for(repeat = 0, head_p = payload_p;
        repeat < repeat_count;
        repeat++, head_p += block_length)
    {
        memcpy(head_p, block_payload_pp[repeat], block_length);
        free(block_payload_pp[repeat]);
    }
    free(block_payload_pp);
    return payload_p;
}

uint8_t* dx7_wrap_bulk_payload(const void* data_p,
                               size_t data_length,
                               size_t* format_length_p)
{
    size_t format_length = sizeof(TwoByte_t) + data_length + sizeof(uint8_t);
    uint8_t* wrapped_data_p = malloc(format_length);
    if(wrapped_data_p)
    {
        *(TwoByte_t*) wrapped_data_p = format_payload_size(data_length);
        //TODO: PROBLÈME ICI!!!
        memcpy(wrapped_data_p + sizeof(TwoByte_t), data_p, data_length);
        *(wrapped_data_p + sizeof(TwoByte_t) + data_length) = generate_checksum(data_p, data_length);
        if(format_length_p)
        {
            *format_length_p = format_length;
        }
    }
    return wrapped_data_p;
}

uint8_t* dx7_format_parameter_payload(const ParameterPayload_t* parameter_p,
                                      size_t* length_p)
{
    //TODO: convert.
    if(length_p != NULL)
    {
        *length_p = 0;
    }
    return NULL;
}

ParameterChangeHeader_t dx7_get_parameter_header(const ParameterPayload_t* parameter_p)
{
    //TODO: convert.
    return PARAMETER_HEADER_INITIALISER;
}


SysexType_t dx7_get_header(const SysexHeader_t* header_p)
{
    printf("Manufacturer: %#4x\n", header_p->id);
    printf("Substatus:    %#4x\n", header_p->substatus);
    printf("Device number:%4u\n",  header_p->device + 1);
    return header_p->substatus;
}

BulkData_t dx7_get_bulk_data_header(const BulkDataHeader_t* header_p)
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

PackedVoiceParameters_t dx7_pack_voice_parameters(VoiceParameters_t parameters)
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
    memcpy(packed_parameters.voice_name, parameters.voice_name, VOICE_NAME_SIZE);

    return packed_parameters;
}

VoiceParameters_t dx7_unpack_voice_parameters(PackedVoiceParameters_t parameters)
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

    memcpy(unpacked_parameters.voice_name, parameters.voice_name, VOICE_NAME_SIZE);

    return unpacked_parameters;
}

char*
dx7_copy_patch_name(VoiceParameters_t parameters)
{
    char* voice_name_p = malloc(sizeof(parameters.voice_name) + 1);
    strncpy(voice_name_p, parameters.voice_name, sizeof(parameters.voice_name));
    char* character_p = voice_name_p + sizeof(parameters.voice_name);
    do
    {
        *character_p = 0;
        character_p--;
    } while(isspace(*character_p));
    for(;character_p>=voice_name_p; --character_p)
    {
        if(!is_valid_byte(*character_p))
        {
            *character_p = '_';
        }
    }

    return voice_name_p;
}


