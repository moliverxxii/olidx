#include <stdio.h>
#include <stdlib.h>

#include "utility.h"
#include "dx7.h"
#include "midi.h"


int main(int argc, char* argv[])
{
    ProgramOptions_t options;
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

