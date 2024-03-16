/*
 * help.cpp
 *
 *  Created on: 16 mars 2024
 *      Author: moliver
 */
#include "help.h"

static const char* const HELP_TEXT =
"help\n"
"-f <file>   : open file <file>\n"
"-h          : show this help\n"
"-u <folder> : unpack into <folder>\n"
;


const char* get_help()
{
    return HELP_TEXT;
}
