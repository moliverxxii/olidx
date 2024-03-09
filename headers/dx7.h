/*
 * dx7.h
 *
 *  Created on: 1 mars 2024
 *      Author: moliver
 */

#ifndef HEADERS_DX7_H_
#define HEADERS_DX7_H_

#include <stdlib.h>
#include <stdint.h>

/* macro */
#define VOICE_NAME_SIZE 10
#define VOICE_COUNT     32

#define CONVERT_STRUCT_PARAMETER(SOURCE_STRUCT, DESTINATION_STRUCT, PARAMETER)\
    ((DESTINATION_STRUCT).PARAMETER = (SOURCE_STRUCT).PARAMETER)

/* enum */
typedef enum Operator_t
{
    OPERATOR_1 = 0,
    OPERATOR_2 = 1,
    OPERATOR_3 = 2,
    OPERATOR_4 = 3,
    OPERATOR_5 = 4,
    OPERATOR_6 = 5,
    OPERATOR_COUNT,
} Operator_t;

typedef enum SysexType_t
{
    SYSEX_TYPE_BULK      = 0,
    SYSEX_TYPE_PARAMETER = 1,
    SYSEX_TYPE_COUNT     = 2
} SysexType_t;

typedef enum BulkData_t
{
    BULK_DATA_VOICE_EDIT_BUFFER      = 0,
    BULK_DATA_SUPPLEMENT_EDIT_BUFFER,
    BULK_DATA_PACKED_32_SUPPLEMENT,
    BULK_DATA_PACKED_32_VOICE,
    BULK_DATA_UNIVERSAL_BULK_DUMP,
    BULK_DATA_MALFORMED,
    BULK_DATA_FORMAT_COUNT
} BulkData_t;

typedef enum BulkDataFormat_t
{
    BULK_DATA_FORMAT_VOICE_EDIT_BUFFER      = 0x00,
    BULK_DATA_FORMAT_SUPPLEMENT_EDIT_BUFFER = 0x05,
    BULK_DATA_FORMAT_PACKED_32_SUPPLEMENT   = 0x06,
    BULK_DATA_FORMAT_PACKED_32_VOICE        = 0X09,
    BULK_DATA_FORMAT_UNIVERSAL_BULK_DUMP    = 0x7E
} BulkDataFormat_t;

typedef enum UniversalBulkData_t
{
    UNIVERSAL_BULK_DATA_PERFORMANCE_EDIT_BUFFER = 0,
    UNIVERSAL_BULK_DATA_PACKED_32_PERFORMANCE,
    UNIVERSAL_BULK_DATA_SYSTEM_SET_UP,
    UNIVERSAL_BULK_DATA_MICRO_TUNING_EDIT_BUFFER,
    UNIVERSAL_BULK_DATA_MICRO_TUNING_MEMORY_0,
    UNIVERSAL_BULK_DATA_MICRO_TUNING_MEMORY_1,
    UNIVERSAL_BULK_DATA_MICRO_TUNING_CARTRIDGE,
    UNIVERSAL_BULK_DATA_FRACTIONAL_SCALING_EDIT_BUFFER,
    UNIVERSAL_BULK_DATA_FRACTIONAL_SCALING_CARTRIDGE,
    UNIVERSAL_BULK_DATA_COUNT,
    UNIVERSAL_BULK_DATA_ERROR = -1
} UniversalBulkData_t;

typedef enum BulkDataByteCount_t
{
    BYTE_COUNT_VOICE_EDIT_BUFFER      = 155,
    BYTE_COUNT_SUPPLEMENT_EDIT_BUFFER = 49,
    BYTE_COUNT_PACKED_32_SUPPLEMENT   = 1120,
    BYTE_COUNT_PACKED_32_VOICE        = 4096
} BulkDataByteCount_t;

typedef enum ParameterChange_t
{
    PARAMETER_CHANGE_VOICE = 0,
    PARAMETER_CHANGE_SUPPLEMENT,
    PARAMETER_CHANGE_PERFORMANCE,
    PARAMETER_CHANGE_SYSTEM_SET_UP,
    PARAMETER_CHANGE_MICRO_TUNING,
    PARAMETER_CHANGE_FRACTIONAL_SCALING,
    PARAMETER_CHANGE_COUNT
} ParameterChange_t;

/* struct */
typedef struct OperatorParameters_t
{
    uint8_t eg_rate_1;              //0 - 99
    uint8_t eg_rate_2;
    uint8_t eg_rate_3;
    uint8_t eg_rate_4;
    uint8_t eg_level_1;
    uint8_t eg_level_2;
    uint8_t eg_level_3;
    uint8_t eg_level_4;
    uint8_t break_point;
    uint8_t left_depth;
    uint8_t right_depth;
    uint8_t left_curve;             //0 - 3
    uint8_t right_curve;
    uint8_t rate_scaling;           //0 - 7
    uint8_t modulation_sensitivity; //0 - 3
    uint8_t touch_sensitivity;      //0 - 3
    uint8_t total_level;            //0 - 99
    uint8_t frequency_mode;         //0 - 1
    uint8_t frequency_coarse;       //0 - 31
    uint8_t frequency_fine;         //0 - 99
    uint8_t detune;                 //0 - 14
} OperatorParameters_t;

typedef struct VoiceParameters_t
{
    OperatorParameters_t Operator[OPERATOR_COUNT];
    uint8_t peg_rate_1;                       //0 - 99
    uint8_t peg_rate_2;
    uint8_t peg_rate_3;
    uint8_t peg_rate_4;
    uint8_t peg_level_1;                      //0 - 99
    uint8_t peg_level_2;
    uint8_t peg_level_3;
    uint8_t peg_level_4;
    uint8_t algorithm;                        //0 - 31
    uint8_t feedback_level;                   //0 - 7
    uint8_t oscillator_phase_init;            //0 - 1
    uint8_t lfo_speed;                        //0 - 99
    uint8_t lfo_delay_time;                   //0 - 99
    uint8_t pitch_modulation_depth;           //0 - 99
    uint8_t amplitude_modulation_depth;       //0 - 99
    uint8_t lfo_key_sync;                     //0 - 1
    uint8_t lfo_wave;                         //0 - 5
    uint8_t lfo_pitch_modulation_sensitivity; //0 - 7
    uint8_t transpose;                        //0 - 48
    char    voice_name[VOICE_NAME_SIZE];      //ASCII
} VoiceParameters_t;

typedef struct PackedOperatorParameters_t
{
    uint8_t eg_rate_1;              //0 - 99
    uint8_t eg_rate_2;
    uint8_t eg_rate_3;
    uint8_t eg_rate_4;
    uint8_t eg_level_1;
    uint8_t eg_level_2;
    uint8_t eg_level_3;
    uint8_t eg_level_4;
    uint8_t break_point;
    uint8_t left_depth;
    uint8_t right_depth;
    uint8_t left_curve             : 2; //0 - 3
    uint8_t right_curve            : 2;
    uint8_t                        : 4;

    uint8_t rate_scaling           : 3; //0 - 7
    uint8_t detune                 : 4; //0 - 14
    uint8_t                        : 1;

    uint8_t modulation_sensitivity : 2; //0 - 3
    uint8_t touch_sensitivity      : 3; //0 - 3
    uint8_t                        : 3;
    uint8_t total_level;                //0 - 99
    uint8_t frequency_mode         : 1; //0 - 1
    uint8_t frequency_coarse       : 5; //0 - 31
    uint8_t                        : 1;

    uint8_t frequency_fine;             //0 - 99
} PackedOperatorParameters_t;

typedef struct PackedVoiceParameters_t
{
    PackedOperatorParameters_t Operator[OPERATOR_COUNT];
    uint8_t peg_rate_1;                           //0 - 99
    uint8_t peg_rate_2;
    uint8_t peg_rate_3;
    uint8_t peg_rate_4;
    uint8_t peg_level_1;                          //0 - 99
    uint8_t peg_level_2;
    uint8_t peg_level_3;
    uint8_t peg_level_4;

    uint8_t algorithm                        : 5; //0 - 31
    uint8_t                                  : 3;

    uint8_t feedback_level                   : 3; //0 - 7
    uint8_t oscillator_phase_init            : 1; //0 - 1
    uint8_t                                  : 4;

    uint8_t lfo_speed;                            //0 - 99

    uint8_t lfo_delay_time;                       //0 - 99

    uint8_t pitch_modulation_depth;               //0 - 99

    uint8_t amplitude_modulation_depth;           //0 - 99

    uint8_t lfo_key_sync                     : 1; //0 - 1
    uint8_t lfo_wave                         : 3; //0 - 5
    uint8_t lfo_pitch_modulation_sensitivity : 3; //0 - 7
    uint8_t                                  : 1;

    uint8_t transpose;                            //0 - 48
    char    voice_name[VOICE_NAME_SIZE];          //ASCII
} PackedVoiceParameters_t;

typedef struct SysexHeader_t
{
    uint8_t id;
    uint8_t device    : 4;
    uint8_t substatus : 4;
} SysexHeader_t;

typedef struct ParameterChangeHeader_t
{
    uint8_t group_l   : 2;
    uint8_t group_h   : 5;
    uint8_t parameter;
} ParameterChangeHeader_t;

typedef struct BulkDataHeader_t
{
    uint8_t format;
} BulkDataHeader_t;

typedef union ParameterChangeData_t
{
    uint8_t data_1B;
    uint8_t data_3B[3];
    uint8_t fractional_scaling[4];
} ParameterChangeData_t;

/* table */
typedef PackedVoiceParameters_t Packed32Voice_t[VOICE_COUNT];

/* string */
extern const char* const SYSEX_TYPE_NAME_TABLE[SYSEX_TYPE_COUNT];
extern const char* const BULK_DATA_FORMAT_NAME_TABLE[BULK_DATA_FORMAT_COUNT];
extern const char* const UNIVERSAL_BULK_DATA_CLASSIFICATION_NAME;
extern const char* const UNIVERSAL_BULK_DATA_FORMAT_TABLE[UNIVERSAL_BULK_DATA_COUNT];
extern const char* const UNIVERSAL_BULK_DATA_NAME_TABLE[UNIVERSAL_BULK_DATA_COUNT];

/* constant */
extern const uint8_t BULK_DATA_FORMAT_TABLE[BULK_DATA_FORMAT_COUNT];
extern const size_t UNIVERSAL_BULK_DATA_CLASSIFICATION_SIZE;
extern const size_t UNIVERSAL_BULK_DATA_FORMAT_SIZE;

/* initialisers */
extern const SysexHeader_t SYSEX_HEADER_INITIALISER;
extern const ParameterChangeHeader_t PARAMETER_HEADER_INITIALISER;
extern const BulkDataHeader_t BULK_HEADER_INITIALISER;

/* function */
void process_sysex_data(const void* data_p);
SysexType_t get_header_info(const SysexHeader_t* header_p);
BulkData_t get_bulk_data_header_info(const BulkDataHeader_t* header_p);

PackedVoiceParameters_t pack_voice_parameters(VoiceParameters_t parameters);
VoiceParameters_t unpack_voice_parameters(PackedVoiceParameters_t parameters);

#endif /* HEADERS_DX7_H_ */
