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

// split input data in patterns
uint8_t **get_patterns(Input_data input_data, uint32_t *patterns_num)
{
    uint8_t **patterns = (uint8_t**)malloc(input_data.str_len * sizeof(uint8_t*));
    uint32_t extracted_patterns = 0;
    bool drop_col = false;

    for (uint32_t i = 0; i < input_data.str_len; ++i){
        patterns[i] = (uint8_t*)calloc(input_data.str_num, sizeof(uint8_t));
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
        for (uint32_t i = 0; i < input_data.str_num; ++i){patterns[extracted_patterns][i] = column[i];}
        ++extracted_patterns;
        free(column);
    }
    patterns = (uint8_t**)realloc(patterns, extracted_patterns * sizeof(uint8_t*));
    *patterns_num = extracted_patterns;
    verbose("Extracted %u direct patterns\n", extracted_patterns);
    return patterns;
}

