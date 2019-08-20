// Bogdan Kirilenko
// 2019, Dresden
// Render sequences
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
#endif
#ifdef __unix__
#include <unistd.h>
#endif
#include "render.h"
#include "CSP.h"
#include "read_input.h"
#include "patterns.h"


uint8_t _swap(uint8_t x)
{
    if (x == 1){return 0;}
    else {return 1;}
}

// show initial program state
uint8_t **render__draw(Pattern *patterns, uint32_t *mask, Input_data *input_data)
{
    uint8_t **ans = (uint8_t**)malloc(input_data->str_num * sizeof(uint8_t*));
    uint32_t dir_num;
    uint32_t rev_num;
    uint8_t dir_ch;
    uint8_t rev_ch;
    uint32_t col_ctr;

    for (uint32_t row = 0; row < input_data->str_num; ++row){
        ans[row] = (uint8_t*)calloc(input_data->act_col_num, sizeof(uint8_t));
        col_ctr = 0;
        for (uint32_t col = 1; col < (input_data->dir_pat_num + 1); ++col){
            rev_num = mask[col];
            dir_num = patterns[col].times - rev_num;
            dir_ch = patterns[col].pattern[row];
            rev_ch = _swap(dir_ch);
            for (uint32_t i = 0; i < dir_num; ++i){
                ans[row][col_ctr] = dir_ch;
                ++col_ctr;
            }
            for (uint32_t i = 0; i < rev_num; ++i){
                ans[row][col_ctr] = rev_ch;
                ++col_ctr;
            }
        }
    }
    return ans;
}


// get zeros number from all rows
uint32_t *render__get_zeros(uint8_t **init_render_data, uint32_t rows, uint32_t cols)
// rows -> input_data->str_num and cols -> input_data->act_col_num;
{
    uint32_t *ans = (uint32_t*)calloc(rows, sizeof(uint32_t));
    uint32_t this_row = 0;

    for (uint32_t i = 0; i < rows; ++i)
    {
        this_row = 0;
        for (uint32_t j = 0; j < cols; ++j){
            if (init_render_data[i][j] == 1){continue;}
            ++this_row;
        }
        ans[i] = this_row;
    }
    return ans;
}


// get minimal possible amount of zeros
uint32_t render__get_min_zeros_amount(uint8_t **init_render_data, uint32_t rows, uint32_t cols)
{
    uint32_t zeros_sum = 0;
    uint32_t p_ones_num = 0;
    uint32_t p_zeros_num = 0;
    for (uint32_t col = 0; col < cols; ++col){
        p_ones_num = 0;
        for (uint32_t row = 0; row < rows; ++row){
            p_ones_num += init_render_data[row][col];
        }
        p_zeros_num = rows - p_ones_num;
        zeros_sum += (p_ones_num > p_zeros_num) ? p_zeros_num : p_ones_num;
    }
    return zeros_sum;
}


// free allocated render
void render__free_render(uint8_t **arr, uint32_t depth)
{
    for (uint32_t i = 0; i < depth; ++i){free(arr[i]);}
    free(arr);
}