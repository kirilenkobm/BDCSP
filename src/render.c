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
void render__show_(Pattern *patterns, Input_data *input_data)
{
    // get direct patterns
    uint32_t *direct_ids = (uint32_t*)calloc(input_data->pat_num, sizeof(uint32_t));
    uint32_t dir_pat_num = 0;
    for (uint32_t i = 1; i < input_data->pat_num; ++i){
        if (patterns[i].pattern[0] == 1){direct_ids[dir_pat_num] = i; ++dir_pat_num;}
    }
    uint32_t dir = 0;
    uint32_t rev = 0;
    uint8_t ch = 0;
    // need to write it transposed
    for (uint32_t row = 0; row < input_data->str_num; ++row)
    {
        printf("# ");
        for (uint32_t p_num = 0; p_num < dir_pat_num; ++p_num)
        {
            dir = patterns[p_num].times - patterns[p_num].reverses;
            rev = patterns[p_num].reverses;

            for (uint32_t i = 0; i < dir; ++i){printf("%d ", patterns[p_num].pattern[row]);}
            for (uint32_t i = 0; i < rev; ++i){
                ch = _swap(patterns[p_num].pattern[row]);
                printf("%d ", ch);
            }
        }
        printf("\n");
    }
}
