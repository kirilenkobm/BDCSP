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

#define MAXCHAR 255
#define W 100
#define H 100
#define REALLOC_STEP 50

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
    // ok, read K
    char *c;
    for (c = argv[2]; *c; ++c)
    {
        if (*c == 0){break;}
        if (!isdigit(*c))
        {
            fprintf(stderr, "Error: k expected to be a positive numerical value, got %s\n", argv[2]);
            _show_usage_and_quit(argv[0]);
        }
    }

    uint32_t k = strtoul(argv[2], 0L, 10);
    if (k == 4294967295){fprintf(stderr, "Warning: k value read as 4294967295, probably an overflow\n");}
    verbose("k = %u \n", k);

    // read input array
    FILE *fp = NULL;
    if (strcmp(argv[1], "stdin") == 0) {
        fp = stdin;  // if stdin --> ok
    } else if (access(argv[1], F_OK) != -1) {
        // not stdin; check that file exists
        fp = fopen(argv[1], "r");
    } else {
        fprintf(stderr, "Sorry, but %s doesn't exist or unavailable\n", argv[1]);
        _show_usage_and_quit(argv[0]);
    }

    char ch;
    bool first_line = true;
    uint32_t line_num = 0;
    uint32_t line_len = H;
    uint32_t char_num = 0;
    uint32_t act_str_len = 0;
    uint8_t **in_arr = (uint8_t**)malloc(W * sizeof(uint8_t*));
    in_arr[line_num] = (uint8_t*)malloc(line_len * sizeof(uint8_t));
    while ((ch = fgetc(fp)) != EOF){
        if ((char_num >= line_len - 1) && first_line){
            line_len += REALLOC_STEP;
            in_arr[line_num] = (uint8_t*)realloc(in_arr[line_num], line_len * sizeof(uint8_t));
        } else if ((char_num >= line_len - 1) && !first_line){
            fprintf(stderr, "Error! Strings expected to have the same length!\n");
            fprintf(stderr, "Violating string: %u\n", line_num);
            for (uint32_t i = 0; i < (line_num + 1); ++i){free(in_arr[i]);}
            free(in_arr);
            exit(1);
        }

        switch (ch)
        {
        case 49:  // 1
            in_arr[line_num][char_num] = 1;
            ++char_num;
            break;
        case 48:  // 0
            in_arr[line_num][char_num] = 0;
            ++char_num;
            break;
        case 10:  // \n
            if (first_line){
                act_str_len = char_num;
                first_line = false;
            }
            if (char_num != act_str_len)
            {
                fprintf(stderr, "Error! Strings expected to have the same length!\n");
                fprintf(stderr, "Violating string: %u\n", line_num + 1);
                for (uint32_t i = 0; i < (line_num + 1); ++i){free(in_arr[i]);}
                free(in_arr);
                exit(1);
            }
            ++line_num;
            char_num = 0;
            in_arr[line_num] = (uint8_t*)malloc(line_len * sizeof(uint8_t));            
            break;
        default:  // something else, error
            fprintf(stderr, "Error: found character which is not 1, 0 or \\n \n");
            for (uint32_t i = 0; i < (line_num + 1); ++i){free(in_arr[i]);}
            free(in_arr);
            exit(1);
            break;
        }
    }
    if (char_num == 0){
        // \n -terminated file
        free(in_arr[line_num]);
    } else if (char_num != act_str_len){
        fprintf(stderr, "Error: the last line of the file has different lenght!\n");
        for (uint32_t i = 0; i < (line_num + 1); ++i){free(in_arr[i]);}
        free(in_arr);
        exit(1);
    } else {
        ++line_num;
    }
    verbose("Lines lenght: %u\n", char_num);
    verbose("Lines num: %u\n", line_num);

    for (uint32_t j = 0; j < line_num; j++){
        printf("Str num: %u:\n", j + 1);
        for (uint32_t i = 0; i < act_str_len; i++){
            printf("%u ", in_arr[j][i]);
        }
        printf("\n");
    }
    // free memory
    for (uint32_t i = 0; i < (line_num); ++i){free(in_arr[i]);}
    free(in_arr);
    return 0;
}
