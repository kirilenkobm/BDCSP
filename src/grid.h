/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/
#ifndef GRID_H
#define GRID_H
#include "patterns.h"

struct point{
    uint32_t point_num;
    uint32_t lvl_num;
    uint32_t char_num;
    bool trivial;
    double density;
    uint8_t point_class;
};

typedef struct point Point;

uint32_t *accumulate_sum(uint32_t *func, uint32_t f_len);

Point *make_grid(uint32_t str_num, uint32_t pat_num,
                 Pattern *patterns, uint32_t max_comb_len);

#endif // !GRID_H
