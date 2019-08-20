// Bogdan Kirilenko
// 2019, Dresden
// Read input array and check it
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
#include "arrstuff.h"

#define MAXCHAR 255
#define W 100
#define H 100
#define REALLOC_STEP 50


// free in data array
void free_in_data(Input_data input_data, uint32_t line_num)
{
    for (uint32_t i = 0; i < (line_num + 1); ++i){free(input_data.in_arr[i]);}
    free(input_data.in_arr);
}


// check if new line is in array
bool __check_is_in(uint8_t **in_arr, uint32_t str_len, uint32_t line_num, bool n_r)
{
    if (line_num == 0){return false;}
    if (n_r){return false;}
    uint8_t *cur = in_arr[line_num];
    bool same = false;
    for (uint32_t i = 0; i < line_num; ++i){
        same = arr_uint8_are_the_same(in_arr[i], cur, str_len);
        if (same){return true;}
    }
    return false;
}


// read and check input stuff
Input_data read_input(char **argv, bool n_r)
{
    Input_data input_data;
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
    input_data.k = strtoul(argv[2], 0L, 10);
    if (input_data.k == 4294967295){
        fprintf(stderr, "Warning: k value read as 4294967295, probably an overflow\n");
    } else if (input_data.k == 0){
        fprintf(stderr, "Error! K expected to be > 0!");
        _show_usage_and_quit(argv[0]);
    }
    verbose("# k = %u \n", input_data.k);

    // and the input array
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
    uint32_t lines_allocated = W;
    uint32_t repeats = 0;
    bool is_in = false;
    bool prev_newline = false;
    input_data.in_arr = (uint8_t**)malloc(lines_allocated * sizeof(uint8_t*));
    input_data.in_arr[line_num] = (uint8_t*)malloc(line_len * sizeof(uint8_t));

    while ((ch = fgetc(fp)) != EOF){
        if (first_line && (char_num >= line_len - 1)){
            line_len += REALLOC_STEP;
            input_data.in_arr[line_num] = (uint8_t*)realloc(input_data.in_arr[line_num],
                                                            line_len * sizeof(uint8_t));
        } else if (first_line && (char_num >= UINT32_MAX - REALLOC_STEP)){
            fprintf(stderr, "Overflow error! Matrix size should not exceed UINT32_MAX x UINT32_MAX\n");
            free_in_data(input_data, line_num);
            exit(1);
        } else if ((char_num >= line_len - 1) && !first_line){
            fprintf(stderr, "Error! Strings expected to have the same length!\n");
            fprintf(stderr, "Violating string: %u\n", line_num);
            free_in_data(input_data, line_num);
            exit(1);
        }

        switch (ch)
        {
            case 49:  // -- "1" -> add to current line
                prev_newline = false;
                input_data.in_arr[line_num][char_num] = 1;
                ++char_num;
                break;
            case 48:  // == "0"
                prev_newline = false;
                input_data.in_arr[line_num][char_num] = 0;
                ++char_num;
                break;
            case 10:  // newline, switch to the next line then
                // new line before this one: continue, do nothing
                if (prev_newline){break;}

                // it was first line: define length
                // to check that other's length is the same
                if (first_line){
                    act_str_len = char_num;
                    first_line = false;
                }
                // is_in = check_is_in()

                // error -> this line length != first line length
                if (char_num != act_str_len)
                {
                    fprintf(stderr, "Error! Strings expected to have the same length!\n");
                    fprintf(stderr, "Violating string: %u\n", line_num + 1);
                    free_in_data(input_data, line_num);
                    exit(1);
                }

                // check if there is a repeat
                // not the most efficien solution, yes
                is_in = __check_is_in(input_data.in_arr, act_str_len, line_num, n_r);
                if (is_in){
                    ++repeats;
                    char_num = 0;
                    break;
                }

                // create new line then
                ++line_num;
                if (line_num > lines_allocated - 1){
                    // too much lines, need to add some extra lines
                    lines_allocated += REALLOC_STEP;
                    input_data.in_arr = (uint8_t**)realloc(input_data.in_arr, lines_allocated * sizeof(uint8_t*));
                }
                if (lines_allocated >= UINT32_MAX){
                    fprintf(stderr, "Overflow error! Matrix size should not exceed UINT32_MAX x UINT32_MAX\n");
                    free_in_data(input_data, line_num);
                    exit(1);
                }
                char_num = 0;
                input_data.in_arr[line_num] = (uint8_t*)malloc(line_len * sizeof(uint8_t));
                prev_newline = true;
                break;
            case 32:  // space, do nothing
                break;
            default:  // something else, error
                fprintf(stderr, "Error: found character %c which is not 1, 0, space or newline.\n", ch);
                free_in_data(input_data, line_num);
                exit(1);
                break;
        }
    }

    // post-processing
    if (char_num == 0){
        // \n - terminated file
        free(input_data.in_arr[line_num]);
    } else if (char_num != act_str_len){
        // happens if the last line had a different length
        fprintf(stderr, "Error: the last line of the file has different lenght!\n");
        free_in_data(input_data, line_num);
        exit(1);
    } else {
        // not \n - terminated file; need to check if the last line is not a repeat
        is_in = __check_is_in(input_data.in_arr, act_str_len, line_num, n_r);
        if (!is_in){++line_num;}
        else {++repeats;}
    }

    // realloc memory; it is likely a bit too much now
    input_data.in_arr = (uint8_t**)realloc(input_data.in_arr, line_num * sizeof(uint8_t*));
    for (uint32_t i = 0; i < line_num; i++){
        input_data.in_arr[i] = (uint8_t*)realloc(input_data.in_arr[i],
                                                 act_str_len * sizeof(uint8_t));
    }

    verbose("# Lines lenght: %u\n", act_str_len);
    verbose("# Lines num: %u\n", line_num);
    verbose("# Found %u repeats\n", repeats);
    if ((act_str_len < 2) || (line_num < 2)){
        fprintf(stderr, "Error! Minimal input size is 2x2!\n");
        free_in_data(input_data, line_num);
        exit(1);
    }
    input_data.str_len = act_str_len;
    input_data.str_num = line_num;
    input_data.act_col_num = 0;
    // for explicity:
    input_data.level_size = line_num;
    return input_data;
}
