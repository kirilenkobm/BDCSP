// Bogdan Kirilenko
// 2019, Dresden
// Make grid and extract basepoints
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
#include "grid.h"
#include "patterns.h"

// make grid
Point *make_grid(Pattern *patterns, uint32_t pat_num, uint32_t max_comb_size)
{
    Point *grid = (Point*)malloc(max_comb_size * sizeof(Point));
    uint32_t *f_max = (uint32_t*)malloc(pat_num * sizeof(uint32_t));
    uint32_t *f_min = (uint32_t*)malloc(pat_num * sizeof(uint32_t));
    

    free(f_max);
    free(f_min);
    return grid;
}
