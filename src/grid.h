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

Point *make_grid(Pattern *patterns, uint32_t pat_num, uint32_t max_comb_size, uint32_t level_size);

#endif // !GRID_H
