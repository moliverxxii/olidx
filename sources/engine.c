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
#include "dx7.h"
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
        printf("file: %s\n", olidx_engine.file_root_p);
        printf("extension: %s\n", get_extension(olidx_engine.file_root_p));
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
        uint8_t* buffer_p = get_next_sysex_payload(midi_file_p, &size);
        if(buffer_p)
        {
            printf("Payload no: %d\n", ++olidx_engine.file_number);
            printf("Sysex size: %dB\n", size);
            process_sysex_data(buffer_p, size, &options);
        }
        free(buffer_p);
        printf("---------\n");
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
    SysExData_t* sysex_p = get_dx7_sysex(data_p, length);
    char* file_name_p;
    FILE* file_p;
    switch(sysex_p->type)
    {
        case SYSEX_TYPE_BULK:
            if(options_p->unpack && sysex_p->bulk_data.type == BULK_DATA_PACKED_32_VOICE)
            {
//                VoiceParameters_t parameters = unpack_voice_parameters();
//                break;
            }
        case SYSEX_TYPE_PARAMETER:
        default:
            printf("writing payload...\n");
            file_name_p = file_name(path_to_file_name(olidx_engine.file_root_p),
                                    olidx_engine.file_number);
            printf("file name: %s\n", file_name_p);
            file_p = fopen(file_name_p, "w+");
            if(file_p == NULL)
            {
                OK(OH NON);
            }
            write_sysex_payload(file_p, data_p, length);
            fclose(file_p);
            free(file_name_p);
            file_name_p = NULL;
            file_p = NULL;
        break;
    }
    free(sysex_p);
}

char* file_name(const char* root_p, uint8_t counter)
{
    size_t value = strlen(olidx_engine.unpack_folder_p) + strlen(root_p) + 3 + strlen(MIDI_SYSEX_EXTENSION) + 1;
    char* file_name_p = malloc(value);
    strcpy(file_name_p, olidx_engine.unpack_folder_p);
    strcat(file_name_p, root_p);
    char suffix[4] = {0};
    sprintf(suffix, "%03hhu", counter);
    strcat(file_name_p, suffix);
    strcat(file_name_p, MIDI_SYSEX_EXTENSION);
    return file_name_p;
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




