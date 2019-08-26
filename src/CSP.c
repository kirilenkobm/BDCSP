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
#include <limits.h>
#ifdef _WIN32
#include <io.h>
#define F_OK 0
#elif __unix__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif
#include "CSP.h"
#include "read_input.h"
#include "patterns.h"
#include "render.h"
#include "traverse.h"
#include "arrstuff.h"
#define VERSION "0.01 Unreleased"


uint8_t log_level = 0;


// in support for free_all func
struct allocated_data{
    uint8_t **input_arr;
    uint32_t str_num;
    uint32_t patterns_num;
    Pattern *patterns;
    uint32_t init_r_data_depth;
    uint8_t **init_render_data;
    uint32_t *zeros_nums;
    uint32_t *zero_mask;
    uint32_t *full_mask;
} allocated;


// print version and quit
void _show_version() 
{
    printf("BDCSP %s\nCopyright 2019 Bogdan Kirilenko, Dresden\n", VERSION);
    exit(0);
}


// show help and exit
void _show_usage_and_quit(char * executable)
{
    fprintf(stderr, "Usage: %s [input file] [k] [-v] [-p]\n", executable);
    fprintf(stderr, "[input file]: text file containing input or stdin\n");
    fprintf(stderr, "[k]: minimal distance to check, positive integer number\n");
    fprintf(stderr, "[-v] <number 0 .. 3>: enable verosity mode, set logging level from 1 to 3, 0 - nothing\n");
    fprintf(stderr, "[-h]: show this message\n");
    fprintf(stderr, "[-V]: show version\n");
    fprintf(stderr, "[-p]: show patterns\n");
    fprintf(stderr, "[-nr]/[--nr]: you promise there are no repetative strings (not recommended) =)\n");
    fprintf(stderr, "[-r]: render initial state (not recommended on big datasets)\n");
    fprintf(stderr, "[-f]: optimize first line\n");
    fprintf(stderr, "[-s]: sanity checks, just check the input correctness and quit\n");
    fprintf(stderr, "[-sr]/[--sr] <filaname>: save final render to file\n");
    fprintf(stderr, "[-a]: try to get distance to average line\n");
    exit(1);    fprintf(stderr, "[-sr]/[--sr] <filaname>: save final render to file\n");

}


// show verbose messages
void verbose(uint8_t lvl, const char * format, ...)
{
    // if(!v) {return;}
    if (log_level < lvl) {return;}
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
    }
    free(allocated.patterns);

    for (uint32_t i = 0; i < allocated.str_num; ++i){free(allocated.input_arr[i]);}
    free(allocated.input_arr);

    render__free_render(allocated.init_render_data, allocated.init_r_data_depth);

    free(allocated.zeros_nums);
    free(allocated.zero_mask);
    free(allocated.full_mask);
    verbose(1, "# Memory freed\n");
}

// get initial densities
void get_init_density_range
(uint32_t *inf_cov, uint32_t *sup_cov, Input_data *input_data, Pattern *patterns)
{
    uint32_t cur_pat_size = 0;
    uint32_t cur_pat_times = 0;
    uint32_t unit = 0;
    uint32_t col_left = input_data->act_col_num;
    bool stop = false;

    // compute the lowest density
    uint32_t start_go = ((input_data->pat_num - 1 ) / 2) + 1;
    uint32_t rev_id = 0;
    // for (uint32_t pat_id = input_data->pat_num - 1; pat_id >0; --pat_id){
    for (uint32_t pat_id = start_go; pat_id < input_data->pat_num; ++pat_id){
        // rev_id = input_data->pat_num - 1;
        cur_pat_size = input_data->str_num - patterns[pat_id].size;
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
    verbose(1, "# Min covered levels (Inf): %u\n", *inf_cov);
    uint32_t max_size = 0;
    uint64_t max_pat_sum = 0;
    uint32_t max_pat_id = 0;
    rev_id = 0;

    // then the highest potential density
    for (uint32_t pat_id = 0; pat_id < input_data->pat_num; ++pat_id){
        if (patterns[pat_id].pattern[0] != 1){continue;}
        // consider only direct primers
        rev_id = input_data->pat_num - pat_id;
        cur_pat_times = patterns[pat_id].times;
        max_pat_id = (patterns[pat_id].size > patterns[rev_id].size) ? pat_id : rev_id;
        max_size = patterns[max_pat_id].size;
        max_pat_sum += ((uint64_t)max_size * cur_pat_times);
    }
    *sup_cov = max_pat_sum / input_data->level_size;
    verbose(1, "# Max covered levels (Sup): %llu\n", *sup_cov);
    // if fails here -> I did a mistake
    assert(*inf_cov <= *sup_cov);
}


// entry point
int main(int argc, char ** argv)
{
    // read arguments
    if (argc == 1) {_show_usage_and_quit(argv[0]);}
    Input_data input_data;
    read_input__opt_args(argc, argv, &input_data);
    if (input_data.show_help) {_show_usage_and_quit(argv[0]);}
    if (input_data.show_version) {_show_version();}
    if (argc < 3){_show_usage_and_quit(argv[0]);}
    log_level = input_data.log_level;
    verbose(1, "# Set verbosity level %d\n", log_level);
    read_input__main_args(argv, &input_data);

    // set defaults to (potentially) allocated stuff
    allocated.input_arr = input_data.in_arr;
    allocated.str_num = input_data.str_num;
    allocated.patterns_num = 0;
    allocated.patterns = NULL;
    allocated.init_r_data_depth = 0;
    allocated.init_render_data = NULL;
    allocated.zeros_nums = NULL;
    allocated.zero_mask = NULL;
    allocated.full_mask = NULL;

    // check for average line
    if (input_data.average_line) {
        uint32_t ave_k = read_input__get_dist_to_average_line(&input_data);
        if (ave_k <= input_data.k){
            verbose(1, "# Answer branch 0\n");
            printf("The answer is:\nTrue\n");
            free_all();
            return 0;
        }
    }

    if (input_data.optimize_f_line){
        read_input__prepare_data(&input_data);
    }

    Pattern *patterns = get_patterns(&input_data);
    if (patterns == NULL){
        free_all();
        exit(2);
    }
    allocated.patterns = patterns;
    allocated.patterns_num = input_data.pat_num;

    if (input_data.show_patterns)  // show patterns if required
    {
        printf("# Extracted patterns are:\n");
        for (uint32_t i = 0; i < input_data.pat_num; ++i){
            printf("# ID: %u:\n", i);
            for (uint32_t j = 0; j < input_data.str_num; j++){printf("%u ", patterns[i].pattern[j]);}
            printf("\t# Appears: %u; size: %u\n\n", patterns[i].times, patterns[i].size);
        }
    }

    if (input_data.sanity_check){
        // in this case all we need to know that out input data is OK
        free_all();
        printf("Sanity check - success\n");
        exit(0);
    }

    // case if K is too high and no need to compute anything
    if (input_data.k >= input_data.act_col_num){
        verbose(1, "# Answer branch 1\n");
        printf("The answer is:\nTrue\n");
        free_all();
        return 0;
    }
    // otherwise I did a mistake
    assert(input_data.act_col_num > 0);

    input_data.to_cover = input_data.act_col_num - input_data.k;
    verbose(1, "# Need to cover %u columns\n", input_data.to_cover);

    // get initial values
    uint32_t inf_cov = 0;
    uint32_t sup_cov = 0;
    get_init_density_range(&inf_cov, &sup_cov, &input_data, patterns);

    // in case if expected density is not in [inf, sup)
    if (input_data.to_cover <= inf_cov){
        verbose(1, "# Answer branch 2\n");
        printf("The answer is:\nTrue\n");
        free_all();
        return 0;
    } else if (input_data.to_cover > sup_cov){
        verbose(1, "# Answer branch 2\n");
        printf("The answer is:\nFalse\n");
        free_all();
        return 0;
    }

    // not so obvious case, get intersection data first
    uint32_t *zero_mask = (uint32_t*)calloc(input_data.dir_pat_num + 1, sizeof(uint32_t));
    uint32_t *full_mask = patterns__get_full_mask(patterns, input_data.dir_pat_num);
    allocated.zero_mask = zero_mask;
    allocated.full_mask = full_mask;

    uint8_t **init_render_data = render__draw(patterns, zero_mask, &input_data);
    if (input_data.init_render_show){
        printf("Initial state render:\n");
        arr_2D_uint8_print(init_render_data, input_data.str_num, input_data.act_col_num);
    }

    uint32_t *zeros_nums = render__get_zeros(init_render_data,
                                             input_data.str_num,
                                             input_data.act_col_num);
    uint32_t max_zeros = arr_max(zeros_nums, input_data.str_num);
    uint32_t min_zeros_amount = render__get_min_zeros_amount(init_render_data,
                                                             input_data.str_num,
                                                             input_data.act_col_num);
    uint32_t baseline = input_data.act_col_num - max_zeros;
    uint32_t allowed_zeros = input_data.act_col_num - input_data.to_cover;
    uint32_t total_allowed_zeros = allowed_zeros * input_data.str_num;
    verbose(2, "# Max zeros num %u; min zeros sum %u\n", max_zeros, min_zeros_amount);
    verbose(2, "# Baseline: %u, max allowed zeros per line: %u\n", baseline, allowed_zeros);
    verbose(2, "# Total allowed zeros: %u\n", total_allowed_zeros);

    if (baseline >= input_data.to_cover){
        // eventually got answer
        verbose(1, "# answer branch 3\n");
        printf("The answer is:\nTrue\n");
        free_all();
        return 0;
    } else if (total_allowed_zeros < min_zeros_amount){
        // also an answer; not sure if it is possible
        verbose(1, "# answer branch 3\n");
        printf("The answer is:\nFalse\n");
        free_all();
        return 0;
    }

    uint32_t cover_left = input_data.to_cover - (input_data.act_col_num - max_zeros);

    verbose(2, "# Initial zeros nums:\n# ");
    for (uint32_t i = 0; i < input_data.str_num; ++i){verbose(2, "%u ", zeros_nums[i]);}
    verbose(2, "\n");
    verbose(1, "# Max zeros: %u; Left to cover: %u\n", max_zeros, cover_left);

    allocated.init_render_data = init_render_data;
    allocated.init_r_data_depth = input_data.str_num;
    allocated.zeros_nums = zeros_nums;

    // "just" extract result and output it
    bool ans_ = traverse__run(zero_mask, full_mask, zeros_nums, &input_data, patterns);
    char *answer = (ans_) ? "True" : "False";
    printf("The answer is:\n%s\n", answer);
    free_all();
    return 0;
}
