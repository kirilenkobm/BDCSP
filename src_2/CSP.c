// Bogdan Kirilenko
// 2019, Dresden
// Entry point, reading input, producing the output
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
#include "patterns.h"

bool v = false;
Input_data input_data;
Pattern_num *patterns;

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
void free_all(uint32_t str_len, uint32_t str_num, uint32_t patterns_num)
{
    // free memory
    for (uint32_t i = 0; i < patterns_num; ++i){free(patterns[i].pattern);}
    free(patterns);
    for (uint32_t i = 0; i < str_num; ++i){free(input_data.in_arr[i]);}
    free(input_data.in_arr);
}


// entry point
int main(int argc, char ** argv)
{
    if (argc < 3){_show_usage_and_quit(argv[0]);}
    // enable verbosity; TODO: do it nicer
    if (argc >= 4 && strcmp(argv[3], "-v") == 0){
        v = true;
        verbose("# Verbose mode activated\n");
    }

    input_data = read_input(argv);
    uint32_t patterns_num = 0;
    uint32_t act_col_num = 0;
    patterns = get_patterns(input_data, &patterns_num, &act_col_num);

    // temp: show patterns
    for (uint32_t i = 0; i < patterns_num; ++i){
        for (uint32_t j = 0; j < input_data.str_num; j++){
            printf("%u ", patterns[i].pattern[j]);
        }
        printf("\n");
        printf("%u %u\n", patterns[i].times, patterns[i].size);
    }

    free_all(input_data.str_len, input_data.str_num, patterns_num);
    return 0;
}
