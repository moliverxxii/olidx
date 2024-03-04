#include <stdio.h>
#include <stdlib.h>

#include "utility.h"
#include "dx7.h"
#include "midi.h"


int main(int argc, char* argv[])
{
    const char* file_name = option_handler(argc, argv);
    printf("file: %s\n", file_name);
	FILE* midi_file_p = fopen(file_name, "r");

	int size;
	do
	{
	    uint8_t* buffer_p = get_sysex_payload(midi_file_p, &size);
        if(buffer_p)
        {
            printf("NEW SYSEX\n");
            printf("Sysex size: %dB\n", size);
            process_sysex_data(buffer_p);
        }
	    free(buffer_p);
	    printf("---------\n");
	} while(size > 0);
	printf("fin\n");
	return 0;
}

