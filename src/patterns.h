#ifndef PATTERNS_H
#define PATTERNS_H
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
#include "read_input.h"


typedef struct
{
    uint8_t *pattern;
    uint32_t times;
    uint32_t size;
    bool *no_intersect;
} Pattern;

void invert_pattern(uint8_t *pattern, uint32_t size);

bool all_eq(uint8_t *col, uint32_t size);

uint32_t get_col_size(uint8_t *col, uint32_t size);

bool are_the_same(uint8_t *pat_1, uint8_t *pat_2, uint32_t pat_size);

void is_it_in(Pattern *patterns, uint8_t *column, uint32_t col_size, bool *is_in,
              uint32_t *ind_if_in, uint32_t extracted_num, uint32_t pat_size);

int diff_as_numbers(uint8_t *pat_a, uint8_t *pat_b);

int compare_patterns(const void *a, const void *b);

Pattern *get_patterns(Input_data input_data, uint32_t *patterns_num,
                          uint32_t *all_pat_num, uint32_t *act_col_num);

void get_intersection_data(Pattern *patterns, uint32_t patterns_num, uint32_t pat_len);

#endif // !PATTERNS_H
