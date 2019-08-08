#ifndef COMBINATIONS_H
#define COMBINATIONS_H
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
#include "combinations.h"
#include "patterns.h"
#include "grid.h"

typedef struct
{
    uint32_t id;
    uint32_t patterns;
    uint32_t size;
    double density;
} Combination;

Combination *extract_combinations(Point *grid, Pattern *patterns, Size_index *size_index,
                                  uint32_t str_len, uint32_t str_num, uint32_t *ones_ind);

#endif // COMBINATIONS_H
