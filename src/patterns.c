// Bogdan Kirilenko
// 2019, Dresden
// Operations with patterns
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
#include "patterns.h"
#include "CSP.h"
#include "read_input.h"


// just invert the pattern
void invert_pattern(uint8_t *pattern, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i){
        pattern[i] = (pattern[i] == 0) ? 1 : 0;
    }
}


// check if all numbers are the same
bool all_eq(uint8_t *col, uint32_t size)
{
    for (uint32_t i = 1; i < size; ++i){
        if (col[i] != col[i - 1]){return false;}
    }
    return true;
}


// count ones in the patterns
uint32_t get_col_size(uint8_t *col, uint32_t size)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < size; ++i){ans += col[i];}
    return ans;
}


// compare two patterns
bool are_the_same(uint8_t *pat_1, uint8_t *pat_2, uint32_t pat_size)
{
    for (uint32_t i = 0; i < pat_size; ++i){
        if (pat_1[i] != pat_2[i]){return false;}
    }
    return true;
}


// check if the pattern was already added
void is_it_in
(Pattern *patterns, uint8_t *column, uint32_t col_size, bool *is_in,
uint32_t *ind_if_in, uint32_t extracted_num, uint32_t pat_size)
{
    if (extracted_num == 0){
        // a priori not here
        *is_in = false;
        *ind_if_in = 0;
        return;
    }
    bool the_same = false;
    // ok, need to check
    for (uint32_t i = 0; i < extracted_num; ++i)
    {
        if (patterns[i].size != col_size){continue;}
        // ok, size matching
        the_same = are_the_same(patterns[i].pattern, column, pat_size);
        if (the_same){
            // it is inside
            *is_in = true;
            *ind_if_in = i;
            return;
        }
    }
    // nothing found
    *is_in = false;
    *ind_if_in = 0;
}


// compare patterns as binary numbers
int diff_as_numbers(uint8_t *pat_a, uint8_t *pat_b)
{
    // return positive number if a is bigger
    // else negative number
    // they must not be equal
    uint32_t p = 0;
    while (1){
        if (pat_a[p] != pat_b[p]){return pat_a[p] - pat_b[p];}
        ++p;
    }
}

// func to compare patterns while sorting
int compare_patterns(const void *a, const void *b)
{ 
    Pattern *ia = (Pattern *)a;
    Pattern *ib = (Pattern *)b;
    if (ib->size > ia->size){return 1;}
    else if (ia->size > ib->size){return -1;}
    else {return diff_as_numbers(ib->pattern, ia->pattern);}
}


// split input data in patterns
Pattern *get_patterns(Input_data *input_data)
{   
    uint32_t act_col_num = 0;
    Pattern *patterns = (Pattern*)malloc(input_data->str_len * sizeof(Pattern));
    uint32_t extracted_patterns = 0;
    bool drop_col = false;
    uint32_t col_size = 0;

    for (uint32_t i = 0; i < input_data->str_len; ++i){
        patterns[i].pattern = (uint8_t*)calloc(input_data->str_num + 1, sizeof(uint8_t));
        patterns[i].times = 0;
        patterns[i].size = 0;
    }
    // go over the columns
    for (uint32_t col_num = 0; col_num < input_data->str_len; ++col_num){
        uint8_t *column = (uint8_t*)calloc(input_data->str_num, sizeof(uint8_t));
        for (uint32_t row_num = 0; row_num < input_data->str_num; ++row_num){
            column[row_num] = input_data->in_arr[row_num][col_num];
        }
        drop_col = all_eq(column, input_data->str_num);
        if (drop_col){
            free(column);
            continue;
        }
        if (column[0] == 0){invert_pattern(column, input_data->str_num);}
        col_size = get_col_size(column, input_data->str_num);

        // check if already in
        bool is_in = false;
        uint32_t ind_if_in = 0;
        is_it_in(patterns, column, col_size, &is_in, &ind_if_in,
                 extracted_patterns, input_data->str_num);
        if (is_in){
            // if in: just increase times
            ++patterns[ind_if_in].times;
        } else {
            // not in - add a new patterns
            for (uint32_t i = 0; i < input_data->str_num; ++i){
                patterns[extracted_patterns].pattern[i] = column[i];
            }
            patterns[extracted_patterns].size = col_size;
            patterns[extracted_patterns].times = 1;
            ++extracted_patterns;
        }
        ++act_col_num;
        free(column);
    }
    input_data->act_col_num = act_col_num;
    // add fake 0-pattern
    for (uint32_t i = 0; i < input_data->str_num; ++i){patterns[extracted_patterns].pattern[i] = 1;}
    patterns[extracted_patterns].size = input_data->str_num;
    patterns[extracted_patterns].times = 0;
    ++extracted_patterns;

    if (extracted_patterns > (UINT32_MAX / 2)) {
        fprintf(stderr, "Overflow error. Number of patterns > UINT32_MAX\n");
        return NULL;
    }

    patterns = (Pattern*)realloc(patterns, extracted_patterns * sizeof(Pattern));
    // now sort this stuff
    // TODO: actually I don't need this qsort, but if I skip it it raises segfault
    qsort(patterns, extracted_patterns, sizeof(Pattern), compare_patterns);
    uint32_t full_len = (2 * extracted_patterns - 1);
    verbose("# Found %u variable columns\n", input_data->act_col_num);
    verbose("# Extracted %u direct patterns\n", extracted_patterns - 1);
    verbose("# Full patterns array takes %u\n", full_len);

    input_data->pat_num = full_len;
    patterns = (Pattern*)realloc(patterns, full_len * sizeof(Pattern));
    for (uint32_t i = 1; i < extracted_patterns; ++i){
        uint32_t minus_i = full_len - i;
        patterns[i].reverses = 0;
        patterns[minus_i].times = patterns[i].times;
        patterns[minus_i].reverses = patterns[i].times;
        patterns[minus_i].size = input_data->str_num - patterns[i].size;
        patterns[minus_i].pattern = (uint8_t*)calloc(input_data->str_num + 1, sizeof(uint8_t));
        for (uint32_t j = 0; j < input_data->str_num; ++j){
            patterns[minus_i].pattern[j] = patterns[i].pattern[j];
        }
        invert_pattern(patterns[minus_i].pattern, input_data->str_num);
    }
    qsort(patterns, full_len, sizeof(Pattern), compare_patterns);
    for (uint32_t i = 0; i < full_len; ++i){
        patterns[i].no_intersect = NULL;
    }
    return patterns;
}


// check if rev pattern is rev
bool _check_is_rev(uint8_t *dir, uint8_t *rev, uint32_t str_num)
{
    // if patterns are inverted, xor must be 1 all the time
    for (uint32_t i = 0; i < str_num; ++i){
        if (!(dir[i] ^ rev[i])){return false;}
    }
    return true;
}


// get direct - inverted pattern index
Dir_Rev *get_dir_rev_data(Pattern *patterns, uint32_t pat_arr_size, uint32_t str_num)
{
    // actually not optimal decision
    // just to make it work now, it must be a more optimal solution
    // hopefully, it will be implemented at some point
    Dir_Rev *dir_rev_index = (Dir_Rev*)malloc(pat_arr_size * sizeof(Dir_Rev));
    dir_rev_index[0].dir = 0;
    dir_rev_index[0].rev = 0;
    uint32_t cur_size = 0;
    uint32_t rev_size = 0;
    bool is_rev = false;
    for (uint32_t i = 1; i < pat_arr_size; ++i)
    {
        dir_rev_index[i].dir = i;
        cur_size = patterns[i].size;
        rev_size = str_num - cur_size;
        if (patterns[i].pattern[0] == 1){dir_rev_index[i].is_dir = true;}
        else {dir_rev_index[i].is_dir = false;}

        for (uint32_t j = 1; j < pat_arr_size; ++j)
        {
            if (patterns[j].size != rev_size){continue;}
            is_rev = _check_is_rev(patterns[i].pattern, patterns[j].pattern, str_num);
            if (is_rev){
                dir_rev_index[i].rev = j;
            }
        }
    }
    return dir_rev_index;
}

// check if patterns intersect
bool patterns_intersect(uint8_t *pat_1, uint8_t *pat_2, uint32_t pat_len)
{
    for (uint32_t i = 0; i < pat_len; ++i){
        if (pat_1[i] && pat_2[i]){return true;}
    }
    return false;
}


// make logical AND over the lists of bools
bool *inter_and(bool *a, bool *b, uint32_t size)
{
    bool *ans = (bool*)calloc(size, sizeof(bool));
    for (uint32_t i = 0; i < size; ++i){
        if (a[i] && b[i]){ans[i] = true;}
    }
    return ans;
}


// fill intersections data
void get_intersection_data(Pattern *patterns, uint32_t patterns_num, uint32_t pat_len)
{
    uint32_t f_pat_size = 0;
    uint32_t f_minus = 0;
    bool p_and_s_intersect;
    // allocate a ton of memory first and then shrink it down
    for (uint32_t p_id = 1; p_id < patterns_num; ++p_id){
        patterns[p_id].no_intersect = (bool*)malloc(patterns_num * sizeof(bool));
    }

    for (uint32_t p_id = 1; p_id < patterns_num; ++p_id){
        f_pat_size = patterns[p_id].size;
        f_minus = patterns_num - p_id;
        // Sadly O^2
        for (uint32_t s_id = 1; s_id < patterns_num; ++s_id)
        {
            // if ((p_id == s_id) || (s_id == f_minus)){continue;}
            p_and_s_intersect = patterns_intersect(patterns[p_id].pattern,
                                                   patterns[s_id].pattern,
                                                   pat_len);
            if (p_and_s_intersect){
                patterns[p_id].no_intersect[s_id] = false;
                patterns[s_id].no_intersect[p_id] = false;
            } else {
                patterns[p_id].no_intersect[s_id] = true;
                patterns[s_id].no_intersect[p_id] = true;
            }
        }
    }
}


// create size index: to find patterns by size
Size_index *index_sizes(Pattern *patterns, uint32_t pat_arr_size, uint32_t sizes_num)
{
    Size_index *sizes_index = (Size_index*)malloc(sizes_num * sizeof(Size_index));
    uint32_t *prts = (uint32_t*)calloc(sizes_num, sizeof(uint32_t));
    for (uint32_t i = 0; i < sizes_num; ++i){
        sizes_index[i].size = i;
        sizes_index[i].num = 0;
    }
    uint32_t cur_size = 0;
    for (uint32_t i = 1; i < pat_arr_size; ++i){
        cur_size = patterns[i].size;
        sizes_index[cur_size].num += 1;
    }
    for (uint32_t i = 1; i < sizes_num; ++i){
        sizes_index[i].ids = (uint32_t*)calloc(sizes_index[i].num, sizeof(uint32_t));
    }
    uint32_t cur_ind = 0;
    for (uint32_t i = 1; i < pat_arr_size; ++i){
        cur_size = patterns[i].size;
        cur_ind = prts[cur_size];
        sizes_index[cur_size].ids[cur_ind] = i;
        prts[cur_size] += 1;
    }
    free(prts);
    return sizes_index;
}


// return a number where index == 1
uint32_t _get_one_ind(uint8_t *pattern, uint32_t str_num)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < str_num; ++i){
        if (pattern[i] == 1){
            ans = i;
            break;
        }
    }
    return ans;
}


// return positions occupied by one-dot patterns
uint32_t *index_ones(Pattern *patterns, uint32_t arr_size, uint32_t str_num)
{
    uint32_t *ans = (uint32_t*)calloc(str_num, sizeof(uint32_t));
    uint32_t ind = 0;
    for (uint32_t i = 0; i < arr_size; ++i)
    {
        if (patterns[i].size != 1){continue;}
        // if one -> get occupied position
        ind = _get_one_ind(patterns[i].pattern, str_num);
        ans[ind] = i;
    }
    verbose("# Single-dot patterns distribution:\n# ");
    for (uint32_t i = 0; i < str_num; ++i){verbose("%u ", ans[i]);}
    verbose("\n");
    return ans;
}
