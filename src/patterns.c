
// Bogdan Kirilenko
// 2019, Dresden
// Split input array in patterns
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
void is_it_in(Pattern_num *patterns, uint8_t *column, uint32_t col_size, bool *is_in,
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
    Pattern_num *ia = (Pattern_num *)a;
    Pattern_num *ib = (Pattern_num *)b;
    if (ia->size != ib->size){
        return ib->size - ia->size;
    } else {
        return diff_as_numbers(ib->pattern, ia->pattern);
    }
}


// split input data in patterns
Pattern_num *get_patterns(Input_data input_data, uint32_t *patterns_num, uint32_t *act_col_num)
{
    Pattern_num *patterns = (Pattern_num*)malloc(input_data.str_len * sizeof(Pattern_num));
    uint32_t extracted_patterns = 0;
    bool drop_col = false;
    uint32_t col_size = 0;

    for (uint32_t i = 0; i < input_data.str_len; ++i){
        patterns[i].pattern = (uint8_t*)calloc(input_data.str_num, sizeof(uint8_t));
        patterns[i].times = 0;
        patterns[i].size = 0;
    }
    // go over the columns
    for (uint32_t col_num = 0; col_num < input_data.str_len; ++col_num){
        uint8_t *column = (uint8_t*)calloc(input_data.str_num, sizeof(uint8_t));
        for (uint32_t row_num = 0; row_num < input_data.str_num; ++row_num){
            column[row_num] = input_data.in_arr[row_num][col_num];
        }
        drop_col = all_eq(column, input_data.str_num);
        if (drop_col){
            free(column);
            continue;
        }
        if (column[0] == 0){invert_pattern(column, input_data.str_num);}
        col_size = get_col_size(column, input_data.str_num);

        // check if already in
        bool is_in = false;
        uint32_t ind_if_in = 0;
        is_it_in(patterns, column, col_size, &is_in, &ind_if_in,
                 extracted_patterns, input_data.str_num);
        if (is_in){
            // if in: just increase times
            ++patterns[ind_if_in].times;
        } else {
            // not in - add a new patterns
            for (uint32_t i = 0; i < input_data.str_num; ++i){patterns[extracted_patterns].pattern[i] = column[i];}
            patterns[extracted_patterns].size = col_size;
            patterns[extracted_patterns].times = 1;
            ++extracted_patterns;
        }
        *act_col_num = *act_col_num + 1;
        free(column);
    }
    patterns = (Pattern_num*)realloc(patterns, extracted_patterns * sizeof(Pattern_num));
    // now sort this stuff
    qsort(patterns, extracted_patterns, sizeof(Pattern_num), compare_patterns);
    *patterns_num = extracted_patterns;
    verbose("# Found %u variable columns\n", *act_col_num);
    verbose("# Extracted %u direct patterns\n", extracted_patterns);
    return patterns;
}