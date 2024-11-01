/*
 * engine.c
 *
 *  Created on: 5 avr. 2024
 *      Author: moliver
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "engine.h"
#include "help.h"
#include "midi.h"

typedef struct olidx_engine_t
{
    int unpack;
    uint8_t file_number;
    const char* file_root_p;
    const char* unpack_folder_p;
} olidx_engine_t;


olidx_engine_t olidx_engine = {0, 0, NULL, NULL};

int run_engine(int argc, char* argv[])
{
    ProgramOptions_t options = {0};
    olidx_engine.file_root_p = option_handler(argc, argv, &options);
    olidx_engine.unpack = options.unpack;
    olidx_engine.unpack_folder_p = options.unpack_folder_p;
    if(olidx_engine.file_root_p)
    {
        printf("File: %s\n", olidx_engine.file_root_p);
    }
    else
    {
        printf("no file specified: OOST!\n");
        return EXIT_FAILURE;
    }
    FILE* midi_file_p = fopen(olidx_engine.file_root_p, "r");
    if(!midi_file_p)
    {
        printf("can't open file: %s\n", olidx_engine.file_root_p);
        return EXIT_FAILURE;
    }
    int size;
    olidx_engine.file_number = 0;
    do
    {
        uint8_t* buffer_p = midi_get_next_sysex_payload(midi_file_p, &size);
        if(buffer_p)
        {
            printf("--------------\n");
            printf("Payload no: %d\n", ++olidx_engine.file_number);
            printf("Sysex size: %dB\n", size);
            process_sysex_data(buffer_p, size, &options);
            free(buffer_p);
        }
    } while(size > 0);
    printf("fin\n");
    return EXIT_SUCCESS;
}

const char* option_handler(int argc, char* argv[], ProgramOptions_t* options_p)
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
                    flag_b = 1;
                    printf("unpack %s\n", optarg);
                    folder_name_p = malloc(strlen(optarg) + 2);
                    strcpy(folder_name_p, optarg);
                    if(options_p != NULL)
                    {
                        options_p->unpack = 1;
                        options_p->unpack_folder_p = folder_name_p;
                    }
                }
                else
                {
                    printf("can't have more than one file to unpack\n");
                }
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
        strcat(folder_name_p,"/");
    }

    return file_name_p;
}

void process_sysex_data(const void* data_p, size_t length, const ProgramOptions_t* options_p)
{
    SysExData_t* sysex_p = dx7_get_sysex(data_p, length);
    char* file_name_p;
    FILE* file_p;
    char* temp_name_p;
    switch(sysex_p->type)
    {
        case SYSEX_TYPE_BULK:
            if(process_sysex_bulk_data(&sysex_p->bulk_data))
            {
                break;
            }
            /* no break */
        case SYSEX_TYPE_PARAMETER:
        default:
            temp_name_p = strip_extension(path_to_file_name(olidx_engine.file_root_p));
            append_counter(&temp_name_p, olidx_engine.file_number);
            file_name_p = file_name(temp_name_p);
            free(temp_name_p);
            file_p = fopen(file_name_p, "w+");
            if(file_p == NULL)
            {
                OK(OH NON);
            }
            else
            {
                printf("writing file: %s\n", file_name_p);
            }
            midi_write_sysex_payload(file_p, data_p, length);
            fclose(file_p);
            free(file_name_p);
            file_name_p = NULL;
            file_p = NULL;
        break;
    }
    free(sysex_p);
}

int process_sysex_bulk_data(const BulkDataPayload_t* bulk_data_p)
{
    switch(bulk_data_p->type)
    {
        case BULK_DATA_PACKED_32_VOICE:
            if(olidx_engine.unpack)
            {
                //TODO: déballage complet. et déballage simple
                unpack_packed32_voice(*bulk_data_p->packed32_voice_p);
            }
            else
            {
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

void unpack_packed32_voice(const Packed32Voice_t voice_parameters)
{
    SysExData_t sysex_message;
    sysex_message.type = SYSEX_TYPE_BULK;
    sysex_message.bulk_data.type = BULK_DATA_VOICE_EDIT_BUFFER;
    VoiceParameters_t parameters;
    sysex_message.bulk_data.payload_p = &parameters;
    int voice;
    for(voice = 0; voice < VOICE_COUNT; ++voice)
    {
        parameters =  dx7_unpack_voice_parameters(voice_parameters[voice]);
        char* patch_name_p = dx7_copy_patch_name(parameters);
        printf("patch %2d: %*s ", voice+1, VOICE_NAME_SIZE, patch_name_p);
        char* file_root_p = strip_extension(path_to_file_name(olidx_engine.file_root_p));
        append_counter(&file_root_p, olidx_engine.file_number);
        append_counter(&file_root_p, voice+1);
        append_str(&file_root_p, "_");
        append_str(&file_root_p, patch_name_p);
        char* file_name_p = file_name(file_root_p);
        FILE* file_p = fopen(file_name_p, "w+");
        size_t length;
        uint8_t* payload_p = dx7_format_sysex(&sysex_message,
                                              &length,
                                              0);
        printf("writing file: %s\n", file_name_p);
        midi_write_sysex_payload(file_p, payload_p, length);
        fclose(file_p);
        free(payload_p);
        free(file_name_p);
        free(file_root_p);
        free(patch_name_p);
    }
}

char* file_name(const char* root_p)
{
    char* file_name_p = malloc(strlen(olidx_engine.unpack_folder_p) + 1);
    strcpy(file_name_p, olidx_engine.unpack_folder_p);
    append_str(&file_name_p, root_p);
    append_str(&file_name_p, MIDI_SYSEX_EXTENSION);
    return file_name_p;
}





