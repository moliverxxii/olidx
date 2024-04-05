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

typedef struct olidx_engine_t
{
    int unpack;
    uint8_t file_number;
    const char file_root_p;
} olidx_engine_t;

int run_engine(int argc, char* argv[])
{
    ProgramOptions_t options = {0};
    const char* file_name = option_handler(argc, argv, &options);
    if(file_name)
    {
        printf("file: %s\n", file_name);
    }
    else
    {
        printf("no file specified: OOST!\n");
        return EXIT_FAILURE;
    }
    FILE* midi_file_p = fopen(file_name, "r");
    if(!midi_file_p)
    {
        printf("can't open file: %s\n", file_name);
        return EXIT_FAILURE;
    }
    int size;
    int sysex_counter = 0;
    do
    {
        uint8_t* buffer_p = get_next_sysex_payload(midi_file_p, &size);
        if(buffer_p)
        {
            printf("Payload no: %d\n", ++sysex_counter);
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
                    printf("unpack %s\n", optarg);
                    folder_name_p = malloc(strlen(optarg) + 1);
                    strcpy(folder_name_p, optarg);
                    if(options_p != NULL)
                    {
                        options_p->unpack = 1;
                    }
                }
                else
                {
                    printf("can't have more than one file to unpack\n");
                }
                flag_b = 1;
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
        free(folder_name_p);
    }

    return file_name_p;
}

void process_sysex_data(const void* data_p, size_t length, const ProgramOptions_t* options_p)
{
    SysExData_t* sysex_p = get_dx7_sysex(data_p, length);
    switch(sysex_p->type)
    {
        case SYSEX_TYPE_BULK:
            if(options_p->unpack && sysex_p->bulk_data.type == BULK_DATA_PACKED_32_VOICE)
            {
            }
        break;
        case SYSEX_TYPE_PARAMETER:
        break;
        default:
        break;
    }
    free(sysex_p);
}

char* file_name(const char* root_p, uint8_t counter)
{
    char* file_name_p = malloc(strlen(root_p) + 4);
    char suffix[4];
    strcpy(file_name_p, root_p);
    sprintf(suffix, "%03hhu", counter);
    strcat(file_name_p, suffix);
    return file_name_p;
}



