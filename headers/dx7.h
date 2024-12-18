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

#include "utility.h"
#include "midi.h"

/* macros */
#define VOICE_NAME_SIZE                    10
#define VOICE_COUNT                        32
#define PERFORMANCE_COUNT                  32
#define MICRO_TUNING_CARTRIDGE_COUNT       64
#define FRACTIONAL_SCALING_CARTRIDGE_COUNT 64
#define UNIVERSAL_BULK_DATA_CLASSIFICATION_SIZE 4U
#define UNIVERSAL_BULK_DATA_FORMAT_SIZE         6U


#define CONVERT_STRUCT_PARAMETER(SOURCE_STRUCT, DESTINATION_STRUCT, PARAMETER)\
    ((DESTINATION_STRUCT).PARAMETER = (SOURCE_STRUCT).PARAMETER)

/* enumerations */
typedef enum Operator_t
{
    OPERATOR_6 = 0,
    OPERATOR_5,
    OPERATOR_4,
    OPERATOR_3,
    OPERATOR_2,
    OPERATOR_1,
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
    BULK_DATA_FORMAT_PACKED_32_VOICE        = 0x09,
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
    BYTE_COUNT_VOICE_EDIT_BUFFER      =  155,
    BYTE_COUNT_SUPPLEMENT_EDIT_BUFFER =   49,
    BYTE_COUNT_PACKED_32_SUPPLEMENT   = 1120,
    BYTE_COUNT_PACKED_32_VOICE        = 4096
} BulkDataByteCount_t;

typedef enum UniversalBulkDataByteCount_t
{
    BYTE_COUNT_PERFORMANCE_EDIT_BUFFER =   61,
    BYTE_COUNT_PACKED_32_PERFORMANCE   = 1642,
    BYTE_COUNT_SYSTEM_SET_UP           =  112,
    BYTE_COUNT_MICRO_TUNING            =  266,
    BYTE_COUNT_FRACTIONAL_SCALING      =  502,
} UniversalBulkDataByteCount_t;

typedef enum UniversalBulkDataRepeat_t
{
    REPEAT_PERFORMANCE_EDIT_BUFFER        =  1,
    REPEAT_PACKED_32_PERFORMANCE          =  1,
    REPEAT_SYSTEM_SET_UP                  =  1,
    REPEAT_MICRO_TUNING                   =  1,
    REPEAT_MICRO_TUNING_CARTRIDGE         = 64,
    REPEAT_FRACTIONAL_SCALING_EDIT_BUFFER =  1,
    REPEAT_FRACTIONAL_SCALING_CARTRIDGE   = 32,
} UniversalBulkDataRepeat_t;


typedef enum ParameterChange_t
{
    PARAMETER_CHANGE_VOICE = 0,
    PARAMETER_CHANGE_SUPPLEMENT,
    PARAMETER_CHANGE_MICRO_TUNING,
    PARAMETER_CHANGE_FRACTIONAL_SCALING,
    PARAMETER_CHANGE_PERFORMANCE,
    PARAMETER_CHANGE_SYSTEM_SET_UP,
    PARAMETER_CHANGE_COUNT
} ParameterChange_t;


typedef enum FractionalScalingRange_t
{
//TODO: notes.
    FRACTIONAL_SCALING_COUNT = 40
} FractionalScalingRange_t;

/* structures */
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

//TODO: DEFINE.
typedef struct SupplementVoiceParameters_t
{
} SupplementVoiceParameters_t;

//TODO: DEFINE.
typedef struct PackedSupplementVoiceParameters_t
{
} PackedSupplementVoiceParameters_t;

//TODO: DEFINE.
typedef struct PerformanceParameters_t
{
} PerformanceParameters_t;

//TODO: DEFINE.
typedef struct SystemSetup_t
{

} SystemSetup_t;

typedef TwoByte_t MicroTuningParameter_t;
typedef TwoByte_t FractionalScalingOffset_t;
typedef TwoByte_t FractionalScalingParameter_t;

typedef struct FractionalScalingOperatorParameters_t
{
    FractionalScalingOffset_t offset;
    FractionalScalingParameter_t level[FRACTIONAL_SCALING_COUNT];
} FractionalScalingOperatorParameters_t;

typedef struct SysexHeader_t
{
    uint8_t id;
    uint8_t device    : 4;
    uint8_t substatus : 4;
} SysexHeader_t;

typedef struct ParameterChangeHeader_t
{
    uint8_t group_h   : 2;
    uint8_t group_g   : 5;
    uint8_t           : 1;
    uint8_t parameter;
} ParameterChangeHeader_t;

typedef struct BulkDataHeader_t
{
    uint8_t format;
} BulkDataHeader_t;


typedef char UniversalBulkDataClassificationName_t[UNIVERSAL_BULK_DATA_CLASSIFICATION_SIZE];
typedef char UniversalBulkDataFormatName_t[UNIVERSAL_BULK_DATA_FORMAT_SIZE];

typedef struct UniversalBulkDataHeader_t
{
    UniversalBulkDataClassificationName_t classification;
    UniversalBulkDataFormatName_t         format;
} UniversalBulkDataHeader_t;

/* unions */

/* tables */
typedef PackedVoiceParameters_t Packed32Voice_t[VOICE_COUNT];
typedef PackedSupplementVoiceParameters_t Packed32SupplementVoice_t[VOICE_COUNT];
typedef PerformanceParameters_t Packed32Performance_t[PERFORMANCE_COUNT];
typedef MicroTuningParameter_t MicroTuningParameters_t[MIDI_NOTE_COUNT];
typedef MicroTuningParameters_t MicroTuningCartridge_t[MICRO_TUNING_CARTRIDGE_COUNT];
typedef FractionalScalingOperatorParameters_t FractionalScalingParameters_t[OPERATOR_COUNT];
typedef FractionalScalingParameters_t FractionalScalingCartridge_t[FRACTIONAL_SCALING_CARTRIDGE_COUNT];

/* complex types */
typedef struct UniversalBulkDataPayload_t
{
    UniversalBulkData_t type;
    union
    {
        void*                          payload_p;
        PerformanceParameters_t*       performance_parameters_p;        //PERFORMANCE_EDIT_BUFFER
        Packed32Performance_t*         packed32_performance_p;          //PACKED_32_PERFORMANCE
        SystemSetup_t*                 system_setup_p;                  //SYSTEM_SET_UP
        //MICRO_TUNING_EDIT_BUFFER
        //MICRO_TUNING_MEMORY_0
        //MICRO_TUNING_MEMORY_1
        MicroTuningParameters_t*       micro_tuning_edit_parameters_p;
        MicroTuningCartridge_t*        micro_tuning_cartridge_p;        //MICRO_TUNING_CARTRIDGE
        FractionalScalingParameters_t* fractional_scaling_parameters_p; //FRACTIONAL_SCALING_EDIT_BUFFER
        FractionalScalingCartridge_t*  fractional_scaling_cartridge_p;  //FRACTIONAL_SCALING_CARTRIDGE
    };
} UniversalBulkDataPayload_t;

typedef struct BulkDataPayload_t
{
    BulkData_t type;
    union
    {
        void*                        payload_p;
        VoiceParameters_t*           voice_parameters_p;            //VOICE_EDIT_BUFFER
        SupplementVoiceParameters_t* supplement_voice_parameters_p; //SUPPLEMENT_EDIT_BUFFER
        Packed32Voice_t*             packed32_voice_p;              //PACKED_32_SUPPLEMENT
        Packed32SupplementVoice_t*   packed32_supplement_voice_p;   //PACKED_32_VOICE
        UniversalBulkDataPayload_t   universal;
    };
} BulkDataPayload_t;

typedef struct ParameterPayload_t
{
    ParameterChange_t parameter;
    union
    {
        uint8_t data;
        uint8_t micro_tuning[3];
        uint8_t fractional_scaling_data[4];
    };
} ParameterPayload_t;

typedef struct SysExData_t
{
    SysexType_t type;
    union
    {
        BulkDataPayload_t  bulk_data;
        ParameterPayload_t parameter_change;
    };
} SysExData_t;


/* strings */
extern const char* const SYSEX_TYPE_NAME_TABLE[SYSEX_TYPE_COUNT];
extern const char* const BULK_DATA_FORMAT_NAME_TABLE[BULK_DATA_FORMAT_COUNT];
extern const char* const UNIVERSAL_BULK_DATA_CLASSIFICATION_NAME;
extern const char* const UNIVERSAL_BULK_DATA_FORMAT_TABLE[UNIVERSAL_BULK_DATA_COUNT];
extern const char* const UNIVERSAL_BULK_DATA_NAME_TABLE[UNIVERSAL_BULK_DATA_COUNT];

/* constants */
extern const ParameterChangeHeader_t PARAMETER_CHANGE_GROUP_TABLE[PARAMETER_CHANGE_COUNT];
extern const uint8_t BULK_DATA_FORMAT_TABLE[BULK_DATA_FORMAT_COUNT];
extern const size_t BULK_DATA_BYTE_COUNT_TABLE[BULK_DATA_FORMAT_COUNT];
extern const size_t UNIVERSAL_BULK_DATA_BYTE_COUNT_TABLE[UNIVERSAL_BULK_DATA_COUNT];
extern const size_t UNIVERSAL_BULK_DATA_REPEAT_TABLE[UNIVERSAL_BULK_DATA_COUNT];

/* initialisers */
extern const SysexHeader_t SYSEX_HEADER_INITIALISER;
extern const SysexHeader_t SYSEX_HEADER_INITIALISER_YAMAHA;
extern const ParameterChangeHeader_t PARAMETER_HEADER_INITIALISER;
extern const BulkDataHeader_t BULK_HEADER_INITIALISER;
extern const UniversalBulkDataHeader_t UNIVERSAL_BULK_HEADER_INITIALISER;
extern const SysExData_t SYSEX_DATA_INITIALISER;

/* functions */
/**
 * formats dx7 SysEx payload and return pointer to the payload.
 */
uint8_t* dx7_format_sysex(const SysExData_t* sysex_data_p, size_t* length_p, uint8_t device_id);

/**
 * returns a pointer to an interpreted dx7 SysEx structure.
 * @oaram payload_p the bytes of data from the MIDI sysex file.
 */
SysExData_t* dx7_get_sysex(const uint8_t* payload_p, size_t length);
ParameterPayload_t dx7_get_sysex_parameter(const uint8_t* payload_p);
BulkDataPayload_t dx7_get_sysex_bulk_data(const uint8_t* bulk_payload_p);
/**
 * returns pointer to a formatted  dx7 sysex byte bulk payload.
 * @param bulk_data_p pointer to a bulk data structure.
 */
uint8_t* dx7_format_bulk_payload(const BulkDataPayload_t* bulk_data_p,
                                 size_t* length_p);

/**
 * returns pointer to a formatted dx7 sysex byte universal bulk payload.
 * @param data_p pointer to a universal bulk data structure.
 */
uint8_t* dx7_format_universal_bulk_payload(const UniversalBulkDataPayload_t* data_p,
                                           size_t* data_length_p);

/**
 * wraps a bulk data payload with two byte byte count and checksum.
 * returns pointer to the wrapped data.
 */
uint8_t* dx7_wrap_bulk_payload(const void* data_p,
                               size_t data_length,
                               size_t* format_length_p);

/**
 * Formats a dx7 parameter payload.
 * @returns a formated DX7 SysEx parameter.
 * @param parameter_p pointer to a parameter structure.
 */
uint8_t* dx7_format_parameter_payload(const ParameterPayload_t* parameter_p,
                                      size_t* length_p);
ParameterChangeHeader_t dx7_get_parameter_header(const ParameterPayload_t* parameter_p);
SysexType_t dx7_get_header(const SysexHeader_t* header_p);
BulkData_t dx7_get_bulk_data_header(const BulkDataHeader_t* header_p);

PackedVoiceParameters_t dx7_pack_voice_parameters(VoiceParameters_t parameters);
VoiceParameters_t dx7_unpack_voice_parameters(PackedVoiceParameters_t parameters);

char* dx7_copy_patch_name(VoiceParameters_t parameters);

#endif /* HEADERS_DX7_H_ */
