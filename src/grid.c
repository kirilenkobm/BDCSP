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
#include "patterns.h"


struct point{
    uint32_t point_num;
    uint32_t lvl_num;
    uint32_t char_num;
    bool trivial;
    double density;
    uint8_t point_class;
};


uint32_t *accumulate_sum(uint32_t *func, uint32_t f_len)
// just create accumulated sum array
{
    uint32_t *answer = (uint32_t*)malloc(sizeof(uint32_t) * (f_len + CHUNK));
    uint32_t val = 0;
    for (uint32_t i = 0; i < f_len; i++){
        answer[i] = val + func[i];
        val = answer[i];
    }
    return answer;
}


Point *make_grid(uint32_t str_num, uint32_t pat_num,
                 Pattern *patterns, uint32_t max_comb_len)
// create a grid and return points
{
    size_t f_max_min_size = sizeof(uint32_t) * (pat_num + CHUNK);
    uint32_t *f_max = (uint32_t*)malloc(f_max_min_size);
    uint32_t *f_min = (uint32_t*)malloc(f_max_min_size);
    Pattern *pat_ptr_i = &patterns[0];
    Pattern *pat_ptr_j = &patterns[pat_num - 1];
    for (uint32_t i = 0; i < pat_num; i++)
    // fill fmin and fmax
    {
        f_max[i] = pat_ptr_i->occupies_num;
        f_min[i] = pat_ptr_j->occupies_num;
        pat_ptr_i++;
        pat_ptr_j--;
    }
    // and accumulated forms also
    uint32_t *f_max_acc = accumulate_sum(f_max, pat_num);
    uint32_t *f_min_acc = accumulate_sum(f_min, pat_num);
    
    uint32_t *v_min_ptr = &f_min_acc[0];
    uint32_t *v_max_ptr = &f_max_acc[0];
    uint32_t grid_elems_num = 0;

    for (uint32_t i = 0; i < max_comb_len; i++)
    // compute grid size first
    {
        for (uint32_t p_sum = *v_min_ptr; p_sum < *v_max_ptr + 1; p_sum++)
        {
            uint32_t lvl_full = p_sum / str_num;
            // get rid of...
            if (lvl_full == 0){
                // points without level at all
                continue;
            } else if (p_sum % str_num != 0){
                // and points with incomplete sum
                continue;
            }
            // ok, good element
            grid_elems_num++;
        }
        v_min_ptr++;
        v_max_ptr++;
    }

    // then go to fill this chain finally
    printf("Building a grid with %d elements\n", grid_elems_num);
    size_t grid_size = sizeof(Point) * (grid_elems_num + CHUNK);
    Point *grid = (Point*)malloc(grid_size);
    v_min_ptr = &f_min_acc[0];
    v_max_ptr = &f_max_acc[0];
    uint32_t char_num = 0;
    uint32_t point_id = 0;

    for (uint32_t i = 0; i < max_comb_len; i++)
    // fill the "grid" with points
    // values between v_min ptr and v_max_ptr is what we need
    {
        char_num ++;
        for (uint32_t p_sum = *v_min_ptr; p_sum < *v_max_ptr + 1; p_sum++)
        // apply different sums
        {
            uint32_t lvl_full = p_sum / str_num;
            // get rid of...
            if (lvl_full == 0){
                // points without level at all
                continue;
            } else if (p_sum % str_num != 0){
                // and points with incomplete sum
                continue;
            }
            grid[point_id].lvl_num = lvl_full;
            grid[point_id].char_num = char_num;
            double density = (double)lvl_full / char_num;
            grid[point_id].density = density;
            if ((lvl_full = char_num - 1) && (lvl_full > 1)){
                // ro = (N - 1) / N
                grid[point_id].point_class = 0;
                grid[point_id].trivial = true;
            } else if ((lvl_full == 1) && (char_num == 2)){
                // ro = 1 / 2
                grid[point_id].point_class = 1;
                grid[point_id].trivial = true;
            } else if ((lvl_full == 1) && (char_num > 2)){
                // ro = 1 / N
                grid[point_id].point_class = 2;
                grid[point_id].trivial = true;
            } else {
                grid[point_id].point_class = 3;
                grid[point_id].trivial = false;
            }
        }
        v_min_ptr++;
        v_max_ptr++;
        point_id++;
    }
    grid[point_id].point_class = -1;
    return grid;
}
