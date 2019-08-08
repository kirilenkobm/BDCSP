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
#include "grid.h"

bool v = false;
bool pat_intersect_allocated = false;
bool grid_allocated = false;
bool show_patterns = false;
uint32_t grid_size;
Input_data input_data;
Pattern *patterns;
Point *grid;

// show help and exit
void _show_usage_and_quit(char * executable)
{
    fprintf(stderr, "Usage: %s [input file] [k] [-v] [-p]\n", executable);
    fprintf(stderr, "[input file]: text file containing input or stdin\n");
    fprintf(stderr, "[k]: minimal distance to check, positive integer number\n");
    fprintf(stderr, "[-v]: enable verosity mode\n");
    fprintf(stderr, "[-p]: show patterns\n");
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
    if (pat_intersect_allocated){
        for (uint32_t i = 0; i < patterns_num; ++i){free(patterns[i].no_intersect);}
    }
    free(patterns);
    for (uint32_t i = 0; i < str_num; ++i){free(input_data.in_arr[i]);}
    free(input_data.in_arr);
    if (grid_allocated){
        for (uint32_t i = 0; i < grid_size; ++i){free(grid[i].combinations);}
        free(grid);
    }
}


// just return min of three numbers
uint32_t min_of_three(uint32_t *a, uint32_t *b, uint32_t *c)
{
    if (*a < *b && *a < *c){
        return *a;
    // ok, not a, min of b and c
    } else if (*b < *c){
        return *b;
    // not b and not a -> clearly c
    } else {
        return *c;
    }
}


// get initial densities
void get_init_density_range(uint32_t to_cover, double *inf, double *sup,
                            double *exp_dens, uint32_t pat_num, uint32_t act_col_num)
{
    *sup = (double)(act_col_num - 1) / act_col_num;
    *exp_dens = (double)to_cover / act_col_num;

    uint32_t cur_pat_size = 0;
    uint32_t cur_pat_times = 0;
    uint32_t unit = 0;
    uint32_t min_lvl_cov = 0;
    uint32_t col_left = act_col_num;
    bool stop = false;

    // lower density is the most complicated thing
    for (uint32_t pat_id = 0; pat_id < pat_num; ++pat_id){
        cur_pat_size = patterns[pat_id].size;
        cur_pat_times = patterns[pat_id].times;
        unit = input_data.str_num - cur_pat_size + 1;
        for (uint32_t i = 0; i < cur_pat_times; i++){
            if (unit > col_left){
                stop = true;
                break;
            }
            col_left -= unit;
            ++min_lvl_cov;
        }
        if (stop) {break;}
    }
    verbose("# Min covered levels: %u\n", min_lvl_cov);
    *inf = (double)min_lvl_cov / act_col_num;
}


// entry point
int main(int argc, char ** argv)
{
    // check args number, read extra args
    if (argc < 3){_show_usage_and_quit(argv[0]);}
    for (int op = 3; op < argc; ++op)
    {
        if (strcmp(argv[op], "-v") == 0){
            // enable verbose
            v = true;
            verbose("# Verbose mode activated\n");
        } else if (strcmp(argv[op], "-p") == 0){
            // show patterns
            show_patterns = true;
        } else {
            fprintf(stderr, "Error: unknown parameter %s\n", argv[op]);
            _show_usage_and_quit(argv[0]);
        }
    }

    // read and check input
    input_data = read_input(argv);
    uint32_t patterns_num = 0;
    uint32_t pat_arr_size = 0;
    uint32_t act_col_num = 0;
    patterns = get_patterns(input_data, &patterns_num, &pat_arr_size, &act_col_num);
    if (patterns == NULL){free_all(input_data.str_len, input_data.str_num, pat_arr_size); exit(2);}

    if (show_patterns)  // show patterns if required
    {
        printf("# Direct patterns are:\n");
        for (uint32_t i = 0; i < patterns_num; ++i){
            printf("# ID: %u:\n", i);
            for (uint32_t j = 0; j < input_data.str_num; j++){printf("%u ", patterns[i].pattern[j]);}
            printf("\n");
            printf("# Appears: %u; size: %u\n\n", patterns[i].times, patterns[i].size);
        }
    }
    
    // case if K is too high and no need to compute anything
    if (input_data.k >= act_col_num){
        verbose("# Answer branch 1\n");
        printf("The answer is:\nTrue\n");
        free_all(input_data.str_len, input_data.str_num, pat_arr_size);
        return 0;
    }
    uint32_t to_cover = act_col_num - input_data.k;
    verbose("# Need to cover %u columns\n", to_cover);

    // get initial values
    uint32_t max_comb_len = min_of_three(&input_data.str_num, &input_data.str_len, &patterns_num);
    double sup;
    double inf;
    double exp_dens;
    get_init_density_range(to_cover, &inf, &sup, &exp_dens, patterns_num, act_col_num);
    verbose("# Inf: %f; Exp dens: %f; Sup: %f\n", inf, exp_dens, sup);

    // in case if expected density is not in [inf, sup)
    if (exp_dens <= inf){
        verbose("# Answer branch 2\n");
        printf("The answer is:\nTrue\n");
        free_all(input_data.str_len, input_data.str_num, pat_arr_size);
        return 0;
    } else if (exp_dens > sup){
        verbose("# Answer branch 2\n");
        printf("The answer is:\nFalse\n");
        free_all(input_data.str_len, input_data.str_num, pat_arr_size);
        return 0;
    }

    // not so obvious case, get intersection data first
    get_intersection_data(patterns, pat_arr_size, input_data.str_num);
    pat_intersect_allocated = true;
    // and create the grid
    grid = make_grid(patterns, pat_arr_size, max_comb_len, input_data.str_num);
    grid_allocated = true;
    grid_size = max_comb_len - 1;
    free_all(input_data.str_len, input_data.str_num, pat_arr_size);
    return 0;
}
