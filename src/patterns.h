// Bogdan Kirilenko
// 2019, Dresden
// Operations with patterns
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
#include <limits.h>
#ifdef _WIN32
#include <io.h>
#define F_OK 0
#elif __unix__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif
#include "read_input.h"


typedef struct
{
    uint8_t *pattern;
    uint32_t times;
    uint32_t reverses;
    uint32_t size;
    bool is_zero;
} Pattern;


typedef struct
{
    uint32_t size;
    uint32_t num;
    uint32_t *ids;
} Size_index;


typedef struct{
    uint32_t dir;
    uint32_t rev;
    bool is_dir;
} Dir_Rev;


void invert_pattern(uint8_t *pattern, uint32_t size);

uint32_t get_col_size(uint8_t *col, uint32_t size);

void __patterns__redefine_patterns(Pattern *patterns, Input_data *in_data);

void is_it_in(Pattern *patterns, uint8_t *column, uint32_t col_size, bool *is_in,
              uint32_t *ind_if_in, uint32_t extracted_num, uint32_t pat_size);

int diff_as_numbers(uint8_t *pat_a, uint8_t *pat_b);

int compare_patterns(const void *a, const void *b);

Pattern *get_patterns(Input_data *input_data);

uint32_t *patterns__get_full_mask(Pattern *patterns, uint32_t dir_pat_num);

#endif // !PATTERNS_H
