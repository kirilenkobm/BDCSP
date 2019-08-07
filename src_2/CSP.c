// Bogdan Kirilenko
// 2019, Dresden
// Entry point, reads and checks input data
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h> 
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include "CSP.h"
#include "read_input.h"

bool v = false;

// show help and exit
void _show_usage_and_quit(char * executable)
{
    fprintf(stderr, "Usage: %s [input file] [k] [-v]\n", executable);
    fprintf(stderr, "[input file]: text file containing input or stdin\n");
    fprintf(stderr, "[k]: minimal distance to check, positive integer number\n");
    fprintf(stderr, "[-v]: enable verosity mode\n");
    exit(1);
}


// show verbose messages
void verbose(const char * restrict format, ...)
{
    if(!v) {return;}
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    return;
}


// free all memory
void free_all(){return;}


// entry point
int main(int argc, char ** argv)
{
    if (argc < 3){_show_usage_and_quit(argv[0]);}
    // enable verbosity; TODO: do it nicer
    if (argc >= 4 && strcmp(argv[3], "-v") == 0){
        v = true;
        verbose("Verbose mode activated\n");
    }

    Input_data input_data = read_input(argv);
    

    // free memory
    for (uint32_t i = 0; i < (input_data.str_num); ++i){free(input_data.in_arr[i]);}
    free(input_data.in_arr);
    return 0;
}
