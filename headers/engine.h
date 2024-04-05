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
} ProgramOptions_t;
const char* option_handler(int argc, char* argv[], ProgramOptions_t* options_p);

void process_sysex_data(const void* data_p,
                        size_t length,
                        const ProgramOptions_t* options_p);




#endif /* HEADERS_ENGINE_H_ */
