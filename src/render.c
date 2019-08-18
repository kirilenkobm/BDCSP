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
#include <unistd.h>
#include <limits.h>
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
uint8_t **render__draw(Pattern *patterns, Input_data *input_data, uint32_t *dir_pat_num)
{
    uint8_t **ans = (uint8_t**)malloc(input_data->str_num * sizeof(uint8_t*));
    uint32_t *dir_pattern_ids = (uint32_t*)calloc(input_data->pat_num, sizeof(uint32_t));
    uint32_t dir_pat_ctr = 0;
    for (uint32_t i = 1; i < input_data->pat_num; ++i){
        *dir_pat_num += (patterns[i].times - patterns[i].reverses);
        if (patterns[i].pattern[0] != 1){continue;}
        dir_pattern_ids[dir_pat_ctr] = i;
        ++dir_pat_ctr;
    }

    uint32_t dir_num;
    uint32_t rev_num;
    uint8_t dir_ch;
    uint8_t rev_ch;
    uint32_t col_ctr;

    for (uint32_t row = 0; row < input_data->str_num; ++row){
        ans[row] = (uint8_t*)calloc(*dir_pat_num, sizeof(uint8_t));
        col_ctr = 0;
        for (uint32_t col = 0; col < dir_pat_ctr; ++col){
            rev_num = patterns[dir_pattern_ids[col]].reverses;
            dir_num = patterns[dir_pattern_ids[col]].times - rev_num;
            dir_ch = patterns[dir_pattern_ids[col]].pattern[row];
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


// print 2D render
void render__show_arr(uint8_t **arr, uint32_t rows, uint32_t cols)
{
    for (uint32_t row = 0; row < rows; ++row){
        printf("# ");
        for (uint32_t col = 0; col < cols; ++col){
            printf("%d ", arr[row][col]);
        }
        printf("\n");
    }
}


// get zeros number from all rows
uint32_t *render__get_zeros(uint8_t **init_render_data, uint32_t rows, uint32_t cols)
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

