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




