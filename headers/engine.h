/*
 * engine.h
 *
 *  Created on: 5 avr. 2024
 *      Author: moliver
 */

#ifndef HEADERS_ENGINE_H_
#define HEADERS_ENGINE_H_

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

char* file_name(const char* root_p, uint8_t counter);
const char* path_to_file_name(const char* path_p);
char* get_extension(const char* path_p);
int is_extension_valid(const char* path_p, const char* extension);

#endif /* HEADERS_ENGINE_H_ */
