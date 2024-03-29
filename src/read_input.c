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
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#define F_OK 0
#elif __unix__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#include "CSP.h"
#include "read_input.h"
#include "arrstuff.h"

#define V_MAX 3
#define MAXCHAR 255
#define W 100
#define H 100
#define REALLOC_STEP 50


// free in data array
void free_in_data(Input_data *input_data, uint32_t line_num)
{
    for (uint32_t i = 0; i < (line_num + 1); ++i){free(input_data->in_arr[i]);}
    free(input_data->in_arr);
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


// swap 1 to 0 and back
void swap_1_0(uint8_t *v){*v = 1 - *v;}


// cast the first line to 111111111
void cast_to_ones(uint8_t **arr, uint32_t str_num, uint32_t str_len)
{
    // iter over columns
    for (uint32_t col_num = 0; col_num < str_len; ++col_num)
    {
        if (arr[0][col_num] == 1){continue;}
        // need to swap the column
        for (uint32_t row_num = 0; row_num < str_num; ++row_num){swap_1_0(&arr[row_num][col_num]);}
    }
}


// get comparison info to choose first string
F_string_compare get_arr_data(uint8_t **arr, uint32_t str_num, uint32_t str_len)
{
    F_string_compare ans;
    ans.line_num = 0;
    uint32_t *zero_nums = (uint32_t*)calloc(str_num, sizeof(uint32_t));
    for (uint32_t i = 0; i < str_num; ++i){
        for (uint32_t j = 0; j < str_len; ++j){if (arr[i][j] == 0){zero_nums[i] += 1;}}
    }
    ans.max_zeros = 0;
    ans.min_zeros = UINT32_MAX;
    for (uint32_t i = 0; i < str_num; ++i){
        if (zero_nums[i] > ans.max_zeros) {ans.max_zeros = zero_nums[i];}
        if ((zero_nums[i] < ans.min_zeros) && (zero_nums[i] != 0)) {ans.min_zeros = zero_nums[i];}
    }
    ans.delta = ans.max_zeros - ans.min_zeros;
    return ans;
}


// compare line datas for best line search
int line_comparator(const void *a, const void *b)
{
    F_string_compare *ia = (F_string_compare*)a;
    F_string_compare *ib = (F_string_compare*)b;
    if (ia->max_zeros == ib->max_zeros){return ib->delta - ia->delta;}
    return ib->max_zeros - ia->max_zeros;
}

// prepare input data for algorithm
void read_input__prepare_data(Input_data *input_data)
{
    verbose(1, "Input preparation\n");
    F_string_compare *comp_arr = (F_string_compare*)malloc(input_data->str_num *
                                                           sizeof(F_string_compare));
    for (uint32_t s_num = 0; s_num < input_data->str_num; ++s_num)
    {
        uint8_t **copy = arr_2D_uint8_copy(input_data->in_arr,
                                           input_data->str_num,
                                           input_data->str_len);
        // swap arrays, no need if the first string
        if (s_num != 0){arr_2D_uint8_swap_lines(copy, 0, s_num);}
        // cast to ones and get necessary data
        cast_to_ones(copy, input_data->str_num, input_data->str_len);
        // extract comparison data
        F_string_compare cur_comp = get_arr_data(copy,
                                                 input_data->str_num,
                                                 input_data->str_len);
        
        cur_comp.line_num = s_num;
        comp_arr[s_num] = cur_comp;
        // free data
        for (uint32_t i = 0; i < input_data->str_num; ++i) {free(copy[i]);}
        free(copy);
    }
    // now define the best
    qsort(comp_arr, input_data->str_num, sizeof(F_string_compare), line_comparator);
    uint32_t best_line_num = comp_arr[0].line_num;
    if (best_line_num != 0) {arr_2D_uint8_swap_lines(input_data->in_arr, 0, best_line_num);}
}


// transpose input dataset
void read_input__transpose(Input_data *input_data)
{
    uint32_t init_str_num = input_data->str_num;
    uint32_t init_str_len = input_data->str_len;
    input_data->str_len = init_str_num;
    input_data->str_num = init_str_len;

    uint8_t **new_arr = (uint8_t**)malloc(init_str_len * sizeof(uint8_t*));
    for (uint32_t i = 0; i < init_str_len; ++i){new_arr[i] = (uint8_t*)calloc(init_str_num, sizeof(uint8_t));}

    for (uint32_t row = 0; row < init_str_num; ++row){
        for (uint32_t col = 0; col < init_str_len; ++col){
            new_arr[col][row] = input_data->in_arr[row][col];
        }
    }
    for (uint32_t i = 0; i < init_str_num; ++i){free(input_data->in_arr[i]);}
    free(input_data->in_arr);
    input_data->in_arr = new_arr;
}


// read and check input array and K
void read_input__main_args(char **argv, Input_data *input_data)
{
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
    input_data->k = strtoul(argv[2], 0L, 10);
    if (input_data->k == 4294967295){
        fprintf(stderr, "Warning: k value read as 4294967295, probably an overflow\n");
    } else if (input_data->k == 0){
        fprintf(stderr, "Error! K expected to be > 0!\n");
        _show_usage_and_quit(argv[0]);
    }
    verbose(1, "# k = %u \n", input_data->k);

    // and the input array

    // first check that this is a regular file, not a dir
    struct stat path_stat;
    stat(argv[1], &path_stat);
    if (!S_ISREG(path_stat.st_mode)){
         fprintf(stderr, "Sorry, but %s is not a regular file, likely a directory.\n", argv[1]);
        _show_usage_and_quit(argv[0]);       
    }

    // so read the file then
    FILE *fp = NULL;
    if (strcmp(argv[1], "stdin") == 0) {
        fp = stdin;  // if stdin --> ok
    } else if (access(argv[1], F_OK) != -1) {
        // not stdin; check that file exists
        fp = fopen(argv[1], "r");
    } else {
        fprintf(stderr, "Sorry, but %s doesn't exist or is unavailable\n", argv[1]);
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
    input_data->in_arr = (uint8_t**)malloc(lines_allocated * sizeof(uint8_t*));
    input_data->in_arr[line_num] = (uint8_t*)malloc(line_len * sizeof(uint8_t));

    while ((ch = fgetc(fp)) != EOF){
        if (first_line && (char_num >= line_len - 1)){
            line_len += REALLOC_STEP;
            input_data->in_arr[line_num] = (uint8_t*)realloc(input_data->in_arr[line_num],
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
                input_data->in_arr[line_num][char_num] = 1;
                ++char_num;
                break;
            case 48:  // == "0"
                prev_newline = false;
                input_data->in_arr[line_num][char_num] = 0;
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
                is_in = __check_is_in(input_data->in_arr, act_str_len,
                                      line_num, input_data->no_repeats);
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
                    input_data->in_arr = (uint8_t**)realloc(input_data->in_arr,
                                                            lines_allocated * sizeof(uint8_t*));
                }
                if (lines_allocated >= UINT32_MAX){
                    fprintf(stderr, "Overflow error! Matrix size should not \
                                     exceed UINT32_MAX x UINT32_MAX\n");
                    free_in_data(input_data, line_num);
                    exit(1);
                }
                char_num = 0;
                input_data->in_arr[line_num] = (uint8_t*)malloc(line_len * sizeof(uint8_t));
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
        // \n - terminated file, we allocated one unnecessary array
        free(input_data->in_arr[line_num]);
    } else if (char_num != act_str_len){
        // happens if the last line had a different length
        fprintf(stderr, "Error: the last line of the file has different lenght!\n");
        free_in_data(input_data, line_num);
        exit(1);
    } else {
        // not \n - terminated file; need to check if the last line is not a repeat
        is_in = __check_is_in(input_data->in_arr, act_str_len, line_num, input_data->no_repeats);
        if (!is_in){++line_num;}
        else {++repeats;}
    }

    // realloc memory; it is likely a bit too much now
    input_data->in_arr = (uint8_t**)realloc(input_data->in_arr, line_num * sizeof(uint8_t*));
    for (uint32_t i = 0; i < line_num; i++){
        input_data->in_arr[i] = (uint8_t*)realloc(input_data->in_arr[i],
                                                  act_str_len * sizeof(uint8_t));
    }

    verbose(1, "# Lines lenght: %u\n", act_str_len);
    verbose(1, "# Lines num: %u\n", line_num);
    verbose(1, "# Found %u repeats\n", repeats);
    if ((act_str_len < 2) || (line_num < 2)){
        fprintf(stderr, "Error! Minimal input size is 2x2!\n");
        fprintf(stderr, "Please note that duplicated strings are considered as only one string.\n");
        free_in_data(input_data, line_num);
        exit(1);
    }
    input_data->str_len = act_str_len;
    input_data->str_num = line_num;
    input_data->act_col_num = 0;
    // for explicity:
    input_data->level_size = line_num;
    verbose(1, "# Finished reading\n");
}


bool __get_num_param(char *arg, char *name, uint8_t *val)
{
    char *c;
    for (c = arg; *c; ++c)
    {
        if (*c == 0){break;}
        if (!isdigit(*c))
        {
            fprintf(stderr, "Warining: %s expected to be a positive numerical value, got %s\n", 
                    name, arg);
            fprintf(stderr, "Will try to ignore this if possible\n");
            return false;
        }
    }
    *val = (uint8_t)atoi(arg);
    return true;
}


// read optional arguments
void read_input__opt_args(int argc, char**argv, Input_data *input_data)
{
    // default values
    input_data->show_patterns = false;
    input_data->no_repeats = false;
    input_data->init_render_show = false;
    input_data->optimize_f_line = false;
    input_data->show_help = false;
    input_data->show_version = false;
    input_data->log_level = 0;
    input_data->sanity_check = false;
    input_data->save_render = false;
    input_data->save_render_to = NULL;
    input_data->magic = false;
    input_data->transpose = false;

    if (strcmp(argv[1], "-h") == 0) {
        input_data->show_help = true;
        return;
    } else if (strcmp(argv[1], "-V") == 0){
        input_data->show_version = true;
        return;
    }

    int op = 3;
    while (argv[op] && op < argc)
    {
        if (strcmp(argv[op], "-v") == 0){
            // enable verbose, also need to read required verbosity level
            ++op;
            if (op == argc) {
                // if the last arg
                input_data->log_level = 1;
                return;
            }
            bool _read = __get_num_param(argv[op], "-v", &input_data->log_level);
            if (!_read){
                // default verbosity level - 1
                --op;
                input_data->log_level = 1;
            }
            if (input_data->log_level > 3){
                fprintf(stderr, "Warning, verbosity level %d is not implemented, will use %d\n",
                        input_data->log_level, V_MAX);
                input_data->log_level = 3;
            } else if (input_data->log_level == 0){
                fprintf(stderr, "Warning, -v = 0 has no effect.\n");
            }
        } else if (strcmp(argv[op], "-sr") == 0 || strcmp(argv[op], "--sr") == 0){
            // save final render to...
            ++op;
            if (op == argc) {
                // if the last arg
                fprintf(stderr, "Warning, --sr option need a value, it was not provided, skipping\n");
                return;
            }
            input_data->save_render = true;
            input_data->save_render_to = argv[op];
            // check if file can be written
            FILE *test_f;
            test_f = fopen(input_data->save_render_to, "w");
            if (test_f == NULL) {
                // file is unavailable
                fprintf(stderr, "Error! Cannot write to %s\n", input_data->save_render_to);
                _show_usage_and_quit(argv[0]);
            }
            // ok, we can write to this file
            fclose(test_f);

        } else if (strcmp(argv[op], "-p") == 0){
            // show patterns
            input_data->show_patterns = true;
        } else if (strcmp(argv[op], "-nr") == 0 || strcmp(argv[op], "--nr") == 0){
            // user promises there are no repeats
            input_data->no_repeats = true;
        } else if (strcmp(argv[op], "-r") == 0){
            // we need initial render of program state
            input_data->init_render_show = true;
        } else if (strcmp(argv[op], "-f") == 0) {
            input_data->optimize_f_line = true;
        } else if (strcmp(argv[op], "-V") == 0) {
            input_data->show_version = true;
        } else if (strcmp(argv[op], "-h") == 0) {
            input_data->show_help = true;
        } else if (strcmp(argv[op], "-s") == 0) {
            input_data->sanity_check = true;
        } else if (strcmp(argv[op], "-z") == 0) {
            input_data->magic = true;
        } else if (strcmp(argv[op], "-t") == 0) {
            input_data->transpose = true;
        } else {
            fprintf(stderr, "Ignore unknown parameter %s\n", argv[op]);
            // _show_usage_and_quit(argv[0]);
        }
        ++op;
    }
}


// compute humming distance
uint32_t __get_h_dist(uint8_t *line_1, uint8_t *line_2, uint32_t len)
{
    uint32_t dist = 0;
    for (uint32_t i = 0; i < len; ++i){
        if (line_1[i] != line_2[i]){++dist;}
    }
    return dist;
}


int __comp_uint32(const void *a, const void *b) 
{
    int f = *((uint32_t*)a);
    int s = *((uint32_t*)b);
    if (f > s) {return -1;}
    if (f < s) {return 1;}
    return 0;
}


// exxtract average line and get K for this line
uint32_t read_input__get_dist_to_average_line(Input_data *input_data, uint32_t *hum_dists)
{
    uint8_t *ave_line = (uint8_t*)calloc(input_data->str_len, sizeof(uint8_t));
    uint32_t zeros = 0;
    uint32_t ones = 0;
    for (uint32_t col_num = 0; col_num < input_data->str_len; ++col_num)
    {
        zeros = 0;
        ones = 0;
        // count zeros and ones in the column
        for (uint32_t row_num = 0; row_num < input_data->str_num; ++row_num){
            if (input_data->in_arr[row_num][col_num] == 0){++zeros;}
            else {++ones;}
        }
        // get average number
        // if ones == zeros - doesn't really matter
        ave_line[col_num] = (ones > zeros) ? 1 : 0;
    }
    uint32_t dist = 0;
    for (uint32_t l_num = 0; l_num < input_data->str_num; ++l_num){
        dist = __get_h_dist(ave_line, input_data->in_arr[l_num], input_data->str_len);
        hum_dists[l_num] = dist;
    }
    // uint32_t ans = arr_max(hum_dists, input_data->str_num);
    qsort(hum_dists, input_data->str_num, sizeof(uint32_t), __comp_uint32);
    uint32_t max_1 = hum_dists[0];
    uint32_t max_2 = hum_dists[1];
    uint32_t m_delta = max_1 - max_2;
    if (m_delta % 2 == 1) {--m_delta;}
    // free allocated
    uint32_t ans = max_1 - (m_delta / 2);
    free(ave_line);
    // free(hum_dists);
    return ans;
}
