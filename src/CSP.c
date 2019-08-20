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
// #include "grid.h"
// #include "combinations.h"
#include "render.h"
#include "traverse.h"


bool v = false;


// in support for free_all func
struct allocated_data{
    uint8_t **input_arr;
    uint32_t str_num;
    uint32_t patterns_num;
    Pattern *patterns;
    Dir_Rev *dir_rev_index;
    uint32_t *ones_index;
    uint32_t init_r_data_depth;
    uint8_t **init_render_data;
    uint32_t *zeros_nums;
    uint32_t *zero_mask;
    uint32_t *full_mask;
} allocated;


// show help and exit
void _show_usage_and_quit(char * executable)
{
    fprintf(stderr, "Usage: %s [input file] [k] [-v] [-p]\n", executable);
    fprintf(stderr, "[input file]: text file containing input or stdin\n");
    fprintf(stderr, "[k]: minimal distance to check, positive integer number\n");
    fprintf(stderr, "[-v]: enable verosity mode\n");
    fprintf(stderr, "[-p]: show patterns\n");
    fprintf(stderr, "[-nr]: you promise there are no repetative strings (not recommended) =)\n");
    fprintf(stderr, "[-r]: render initial state (not recommended on big datasets)\n");
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
void free_all()
{
    for (uint32_t i = 0; i < allocated.patterns_num; ++i){
        free(allocated.patterns[i].pattern);
        free(allocated.patterns[i].no_intersect);
    }
    free(allocated.patterns);

    for (uint32_t i = 0; i < allocated.str_num; ++i){free(allocated.input_arr[i]);}
    free(allocated.input_arr);
    free(allocated.dir_rev_index);

    free(allocated.ones_index);
    for (uint32_t i = 0; i < allocated.init_r_data_depth; ++i){free(allocated.init_render_data[i]);}
    free(allocated.init_render_data);

    free(allocated.zeros_nums);
    free(allocated.zero_mask);
    free(allocated.full_mask);
    verbose("# Memory freed\n");
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


// just array sum
uint32_t arr_sum(uint32_t *arr, uint32_t size)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < size; ++i){ans += arr[i];}
    return ans;
}


// max of array
uint32_t arr_max(uint32_t *arr, uint32_t size)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < size; ++i){
        if (arr[i] > ans){ans = arr[i];}
    }
    return ans;
}


// get initial densities
void get_init_density_range(uint32_t *inf_cov, uint32_t *sup_cov, Input_data *input_data,
                            Dir_Rev *dir_rev_index, Pattern *patterns)
{
    // *exp_dens = (double)input_data->to_cover / input_data->act_col_num;

    uint32_t cur_pat_size = 0;
    uint32_t cur_pat_times = 0;
    uint32_t unit = 0;
    uint32_t col_left = input_data->act_col_num;
    bool stop = false;

    // compute the lowest density
    for (uint32_t pat_id = 0; pat_id < input_data->pat_num; ++pat_id){
        cur_pat_size = patterns[pat_id].size;
        cur_pat_times = patterns[pat_id].times;
        unit = input_data->str_num - cur_pat_size + 1;
        for (uint32_t i = 0; i < cur_pat_times; i++){
            if (unit > col_left){
                stop = true;
                break;
            }
            col_left -= unit;
            *inf_cov += 1;
        }
        if (stop) {break;}
    }

    stop = false;
    verbose("# Min covered levels: %u\n", *inf_cov);
    // *inf = (double)min_lvl_cov / input_data->act_col_num;
    uint32_t max_size = 0;
    uint64_t max_pat_sum = 0;
    uint32_t max_pat_id = 0;
    uint32_t rev_id = 0;

    // then the highest potential density
    for (uint32_t pat_id = 0; pat_id < input_data->pat_num; ++pat_id){
        if (!dir_rev_index[pat_id].is_dir){continue;}
        // consider only direct primers
        rev_id = dir_rev_index[pat_id].rev;
        cur_pat_times = patterns[pat_id].times;
        max_pat_id = (patterns[pat_id].size > patterns[rev_id].size) ? pat_id : rev_id;
        max_size = patterns[max_pat_id].size;
        max_pat_sum += ((uint64_t)max_size * cur_pat_times);
    }
    *sup_cov = max_pat_sum / input_data->level_size;
    verbose("# Max covered levels: %llu\n", *sup_cov);
    // *sup = (double)max_covered_levels / input_data->act_col_num;

    // if fails here -> I did a mistake
    assert(*inf_cov <= *sup_cov);
}


// entry point
int main(int argc, char ** argv)
{
    // some default parameters
    bool no_repeats = false;
    bool show_patterns = false;
    bool init_render_show = false;

    // check args number, read extra args
    if (argc < 3){_show_usage_and_quit(argv[0]);}
    // TODO: move to another function?
    for (int op = 3; op < argc; ++op)
    {
        if (strcmp(argv[op], "-v") == 0){
            // enable verbose
            v = true;
            verbose("# Verbose mode activated\n");
        } else if (strcmp(argv[op], "-p") == 0){
            // show patterns
            show_patterns = true;
        } else if (strcmp(argv[op], "-nr") == 0){
            // user promises there are no repeats
            no_repeats = true;
        } else if (strcmp(argv[op], "-r") == 0){
            // we need initial render of program state
            init_render_show = true;
        } else {
            fprintf(stderr, "Error: unknown parameter %s\n", argv[op]);
            _show_usage_and_quit(argv[0]);
        }
    }

    // set defaults to (potentially) allocated stuff
    allocated.input_arr = NULL;
    allocated.patterns_num = 0;
    allocated.patterns = NULL;
    allocated.dir_rev_index = NULL;
    allocated.ones_index = NULL;
    allocated.init_r_data_depth = 0;
    allocated.init_render_data = NULL;
    allocated.zeros_nums = NULL;
    allocated.zero_mask = NULL;
    allocated.full_mask = NULL;

    // read and check input
    Input_data input_data = read_input(argv, no_repeats);
    allocated.input_arr = input_data.in_arr;
    allocated.str_num = input_data.str_num;

    Pattern *patterns = get_patterns(&input_data);
    if (patterns == NULL){
        free_all();
        exit(2);
    }
    allocated.patterns = patterns;
    allocated.patterns_num = input_data.pat_num;

    if (show_patterns)  // show patterns if required
    {
        printf("# Extracted patterns are:\n");
        for (uint32_t i = 0; i < input_data.pat_num; ++i){
            printf("# ID: %u:\n", i);
            for (uint32_t j = 0; j < input_data.str_num; j++){printf("%u ", patterns[i].pattern[j]);}
            printf("\t# Appears: %u; size: %u\n\n", patterns[i].times, patterns[i].size);
        }
    }
    Dir_Rev *dir_rev_index = get_dir_rev_data(patterns, input_data.pat_num, input_data.str_num);
    allocated.dir_rev_index = dir_rev_index;

    // case if K is too high and no need to compute anything
    if (input_data.k >= input_data.act_col_num){
        verbose("# Answer branch 1\n");
        printf("The answer is:\nTrue\n");
        free_all();
        return 0;
    }
    // otherwise I did a mistake
    assert(input_data.act_col_num > 0);

    input_data.to_cover = input_data.act_col_num - input_data.k;
    verbose("# Need to cover %u columns\n", input_data.to_cover);

    // get initial values
    uint32_t max_comb_len = min_of_three(&input_data.str_num, &input_data.str_len, &input_data.pat_num);
    uint32_t inf_cov = 0;
    uint32_t sup_cov = 0;
    get_init_density_range(&inf_cov, &sup_cov, &input_data, dir_rev_index, patterns);
    // verbose("# Inf: %f; Exp dens: %f; Sup: %f\n", inf, exp_dens, sup);
    verbose("# Max comb length: %u\n", max_comb_len);

    // in case if expected density is not in [inf, sup)
    if (input_data.to_cover <= inf_cov){
        verbose("# Answer branch 2\n");
        printf("The answer is:\nTrue\n");
        free_all();
        return 0;
    } else if (input_data.to_cover > sup_cov){
        verbose("# Answer branch 2\n");
        printf("The answer is:\nFalse\n");
        free_all();
        return 0;
    }

    // not so obvious case, get intersection data first
    get_intersection_data(patterns, input_data.pat_num, input_data.str_num);
    uint32_t *ones_index = index_ones(patterns, input_data.pat_num, input_data.str_num);
    allocated.ones_index = ones_index;

    uint32_t *zero_mask = (uint32_t*)calloc(input_data.dir_pat_num + 1, sizeof(uint32_t));
    uint32_t *full_mask = patterns__get_full_mask(patterns, input_data.dir_pat_num);
    allocated.zero_mask = zero_mask;
    allocated.full_mask = full_mask;

    uint8_t **init_render_data = render__draw(patterns, zero_mask, &input_data);
    if (init_render_show){
        printf("Initial state render:\n");
        render__show_arr(init_render_data, input_data.str_num, input_data.act_col_num);
    }

    uint32_t *zeros_nums = render__get_zeros(init_render_data, input_data.str_num, input_data.act_col_num);
    uint32_t max_zeros = arr_max(zeros_nums, input_data.str_num);
    uint32_t min_zeros_amount = render__get_min_zeros_amount(init_render_data, input_data.str_num, input_data.act_col_num);
    uint32_t baseline = input_data.act_col_num - max_zeros;
    uint32_t allowed_zeros = input_data.act_col_num - input_data.to_cover;
    uint32_t total_allowed_zeros = allowed_zeros * input_data.str_num;
    verbose("# Max zeros num %u; min zeros sum %u\n", max_zeros, min_zeros_amount);
    verbose("# Baseline: %u, max allowed zeros per line: %u\n", baseline, allowed_zeros);
    verbose("# Total allowed zeros: %u\n", total_allowed_zeros);

    if (baseline >= input_data.to_cover){
        // eventually got answer
        verbose("# answer branch 3\n");
        printf("The answer is:\nTrue\n");
        free_all();
        return 0;
    } else if (total_allowed_zeros < min_zeros_amount){
        // also an answer; not sure if it is possible
        verbose("# answer branch 3\n");
        printf("The answer is:\nFalse\n");
        free_all();
        return 0;
    }

    uint32_t cover_left = input_data.to_cover - (input_data.act_col_num - max_zeros);

    verbose("# Initial zeros nums:\n# ");
    for (uint32_t i = 0; i < input_data.str_num; ++i){verbose("%u ", zeros_nums[i]);}
    verbose("\n");
    verbose("# Max zeros: %u; Left to cover: %u\n", max_zeros, cover_left);

    allocated.init_render_data = init_render_data;
    allocated.init_r_data_depth = input_data.str_num;
    allocated.zeros_nums = zeros_nums;

    // "just" extract result and output it
    bool result = traverse__run(zero_mask, full_mask, zeros_nums, &input_data, patterns);
    char *answer = (result) ? "True" : "False";
    printf("The answer is:\n%s\n", answer);
    free_all();
    return 0;
}
