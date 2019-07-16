/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "CSP.h"
#include "grid.h"

struct point{
    uint32_t point_num;
    uint32_t lvl_num;
    uint32_t char_num;
    bool base;
    bool trivial;
    double density;
};

Point *make_grid(uint32_t str_num, uint32_t str_len, uint32_t *size_times,
                 Pattern **patterns, Position **positions)
{
    for (uint32_t i = 0; i < str_num; i++){
        printf("Size %d times %d\n", i, size_times[i]);
    }
    Point *grid = malloc(sizeof(Point) * 10);
    return grid;
}
