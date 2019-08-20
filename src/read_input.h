// Bogdan Kirilenko
// 2019, Dresden
// Read input array and check it
#ifndef READ_INPUT_H
#define READ_INPUT_H
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

typedef struct
{
    uint8_t **in_arr;
    uint32_t str_num;
    uint32_t str_len;
    uint32_t k;
    uint32_t to_cover;
    uint32_t pat_num;
    uint32_t dir_pat_num;
    uint32_t act_col_num;
    uint32_t level_size;
    bool v;
    bool show_patterns;
    bool no_repeats;
    bool init_render_show;
} Input_data;

void free_in_data(Input_data *input_data, uint32_t line_num);

void read_input__main_args(char **argv, Input_data *Input_data);

void read_input__opt_args(int argc, char**argv, Input_data *input_data);

#endif  // !READ_INPUT_H
