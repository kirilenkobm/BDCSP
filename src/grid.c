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
#include "grid.h"

struct point{
    uint32_t point_num;
    uint32_t lvl_num;
    uint32_t char_num;
    bool base;
    bool trivial;
    double density;
};

Point *make_grid(uint32_t pos_num, uint32_t str_num, uint16_t *size_times)
{
    Point *grid = malloc(sizeof(Point) * 10);
    return grid;
}
