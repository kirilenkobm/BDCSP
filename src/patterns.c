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
#include <limits.h>
#ifdef _WIN32
#include <io.h>
#define F_OK 0
#elif __unix__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif
#include "patterns.h"
#include "CSP.h"
#include "read_input.h"
#include "arrstuff.h"


// just invert the pattern
void invert_pattern(uint8_t *pattern, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i){
        pattern[i] = 1 - pattern[i];
    }
}


// count ones in the patterns
uint32_t get_col_size(uint8_t *col, uint32_t size)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < size; ++i){ans += col[i];}
    return ans;
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
        the_same = arr_uint8_are_the_same(patterns[i].pattern, column, pat_size);
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
    Pattern *patterns = (Pattern*)malloc((input_data->str_len + 2) * sizeof(Pattern));
    uint32_t extracted_patterns = 0;
    bool drop_col = false;
    uint32_t col_size = 0;

    for (uint32_t i = 0; i < (input_data->str_len + 2); ++i){
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
        drop_col = arr_uint8_all_eq(column, input_data->str_num);
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
    qsort(patterns, extracted_patterns, sizeof(Pattern), compare_patterns);
    uint32_t full_len = (2 * extracted_patterns - 1);
    verbose("# Found %u variable columns\n", input_data->act_col_num);
    verbose("# Extracted %u direct patterns\n", extracted_patterns - 1);
    input_data->dir_pat_num = extracted_patterns - 1;
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
    return patterns;
}


// check if patterns intersect
bool patterns_intersect(uint8_t *pat_1, uint8_t *pat_2, uint32_t pat_len)
{
    for (uint32_t i = 0; i < pat_len; ++i){
        if (pat_1[i] && pat_2[i]){return true;}
    }
    return false;
}


// get mask max for each pattern
uint32_t *patterns__get_full_mask(Pattern *patterns, uint32_t dir_pat_num)
{
    uint32_t *ans = (uint32_t*)calloc(dir_pat_num + 1, sizeof(uint32_t));
    for (uint32_t i = 0; i < (dir_pat_num + 1); ++i){
        ans[i] = patterns[i].times;
    }
    return ans;
}
