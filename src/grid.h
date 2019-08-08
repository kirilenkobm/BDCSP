#ifndef GRID_H
#define GRID_H
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

typedef struct
{
    uint32_t lvl;
    uint32_t comb_size;
    double density;
    uint32_t comb_num;
    uint32_t *combinations;
} Point;

typedef struct
{
    uint32_t number;
    uint32_t quantity;
} Num_q;

int cmpfunc (const void * a, const void * b);

uint32_t *accumulate_sum(uint32_t *arr, uint32_t arr_size);

Num_q *count_elements(uint32_t *arr, uint32_t arr_size, uint32_t *q);

uint32_t check_current(Num_q *path, uint32_t cur_ind);

Num_q *_get_zero_num_q();

uint32_t arr_sum(uint32_t *arr, uint32_t up_to);

void add_to_zero_counter(Num_q *counter, uint32_t *arr, uint32_t arr_size);

uint32_t _elem_search(int64_t l, int64_t r, uint32_t w);

uint32_t *get_path(uint32_t sub_size, uint32_t *prev_path, uint32_t prev_p_len,
                   uint32_t _cur_val, uint32_t _cur_ind, uint32_t req_sum,
                   uint32_t arr_size);

Point *make_grid(Pattern *patterns, uint32_t pat_num, uint32_t max_comb_size, uint32_t level_size);

#endif // !GRID_H
