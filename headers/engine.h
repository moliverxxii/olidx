/*
 * engine.h
 *
 *  Created on: 5 avr. 2024
 *      Author: moliver
 */

#ifndef HEADERS_ENGINE_H_
#define HEADERS_ENGINE_H_

#include "dx7.h"

typedef struct ProgramOptions_t
{
    int unpack;
    const char* unpack_folder_p;
} ProgramOptions_t;

int run_engine(int argc, char* argv[]);
const char* option_handler(int argc, char* argv[], ProgramOptions_t* options_p);

void process_sysex_data(const void* data_p,
                        size_t length,
                        const ProgramOptions_t* options_p);
int process_sysex_bulk_data(const BulkDataPayload_t* bulk_data_p);
void unpack_packed32_voice(const Packed32Voice_t voice_parameters);
char* file_name(const char* root_p);

#endif /* HEADERS_ENGINE_H_ */
