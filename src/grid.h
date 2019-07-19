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
    uint32_t point_sum;
    bool trivial;
    double density;
    uint8_t point_class;
};

typedef struct point Point;

struct grid_output{
    Point *grid;
    uint32_t grid_len;
    size_t grid_mem_size;
    uint32_t *f_max_acc;
    uint32_t *f_min_acc;
};

typedef struct grid_output Grid_output;

uint32_t *accumulate_sum(uint32_t *func, uint32_t f_len);

Grid_output make_grid(uint32_t str_num, uint32_t pat_num,
                 Pattern *patterns, uint32_t max_comb_len);

uint32_t __get_next_size(uint32_t current, uint32_t *all_numbers, uint32_t all_num_size);

uint32_t __check_current(uint32_t current, uint32_t *path_counter, uint32_t *all_moves,
                         uint32_t *all_numbers, uint32_t all_num_size);

int summarize(uint32_t *arr, uint32_t arr_len);


uint32_t *get_size_path(Point *point, uint32_t prev_path, uint32_t prev_path_size, uint32_t *size_times, 
                        int32_t str_num, uint32_t *f_max_acc, uint32_t *f_min_acc, uint32_t current);

#endif // !GRID_H
