/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/
#ifndef GRID_H
#define GRID_H


typedef struct point Point;

Point *make_grid(uint32_t str_num, uint32_t str_len, uint32_t *size_times,
                 Pattern **patterns, Position **positions);

#endif // !GRID_H
