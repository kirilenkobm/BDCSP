// Bogdan Kirilenko
// 2019, Dresden
// Make grid and extract basepoints
// Actually, a subset sum problem solution
// in natural numbers, but for high density
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
#include "CSP.h"
#include "grid.h"
#include "patterns.h"

#define CHUNK 2


// comparing function for qsort
int cmpfunc (const void * a, const void * b) {
   return (*(int*)a - *(int*)b);
}


// create accumulated sum array
uint32_t *accumulate_sum(uint32_t *arr, uint32_t arr_size)
{
    uint32_t *res = (uint32_t*)malloc(arr_size * sizeof(uint32_t));
    res[0] = arr[0];
    for (uint32_t i = 1; i < arr_size; i++){res[i] = res[i - 1] + arr[i];}
    return res;
}


// count available sizes
Num_q *count_elements(uint32_t *arr, uint32_t arr_size, uint32_t *q)
{
    Num_q *res = (Num_q*)malloc(arr_size * sizeof(Num_q));
    // the first element is 0 -> skip it
    // ititiate with the first element
    uint32_t cur_val = arr[1];
    res[*q].number = cur_val;
    res[*q].quantity = 0;
    for (uint32_t i = 1; i < arr_size; i++)
    // go over elements
    // it is granted that they are arranged as:
    // x x x x y y y z z
    {   
        if (arr[i] < cur_val){
            // new elem
            cur_val = arr[i];  // now initiate next value
            *q = *q + 1;  // cannot do with *q++ by some reason
            res[*q].number = cur_val;
            res[*q].quantity = 1;
        } else {
            // the same value, just increase the quantity
            res[*q].quantity++;
        }
    }
    // shrink array size
    size_t shrinked = sizeof(Num_q) * (*q + CHUNK);
    res = (Num_q*)realloc(res, shrinked);
    // terminate the sequence with 0
    *q = *q + 1;
    res[*q].number = 0;
    res[*q].quantity = 0;
    return res;
}


// make grid
Point *make_grid(Pattern *patterns, uint32_t pat_num, uint32_t max_comb_size, uint32_t level_size)
{
    Point *grid = (Point*)malloc((max_comb_size - 1) * sizeof(Point));
    uint32_t *f_max = (uint32_t*)malloc(pat_num * sizeof(uint32_t));
    uint32_t *f_min = (uint32_t*)malloc(pat_num * sizeof(uint32_t));
    f_min[0] = 0;
    f_max[0] = 0;
    for (uint32_t i = 1; i < pat_num; ++i){f_min[i] = patterns[i].size;}
    qsort(f_min, pat_num, sizeof(uint32_t), cmpfunc);
    for (uint32_t i = 1; i < pat_num; ++i){f_max[i] = f_min[pat_num - i];}
    uint32_t *f_max_acc = accumulate_sum(f_max, pat_num);
    uint32_t *f_min_acc = accumulate_sum(f_min, pat_num);

    uint32_t uniq_num = 0;
    Num_q *num_count = count_elements(f_max, pat_num, &uniq_num);
    verbose("# %u unique sizes in the dataset\n", uniq_num);
    uint32_t point_id = 0;

    for (uint32_t subset_size = 2; subset_size < (max_comb_size + 1); ++subset_size)
    {
        printf("Subset size %u\n", subset_size);
        point_id = subset_size - 2;
        grid[point_id].lvl = 1;
        grid[point_id].density = (double)1 / subset_size;
        grid[point_id].comb_size = subset_size;
        grid[point_id].comb_num = 0;

        if (subset_size == 2){
            // simple case, can do it differently
            grid[point_id].combinations = (uint32_t*)calloc(uniq_num, sizeof(uint32_t));
            uint32_t cur_s = 0;
            uint32_t neg_s = 0;
            uint32_t p_ptr = 0;
            for (uint32_t i = 0; i < uniq_num; ++i){
                cur_s = num_count[i].number;
                neg_s = level_size - cur_s;
                if (neg_s > cur_s){break;}
                grid[point_id].combinations[p_ptr] = cur_s;
                ++p_ptr;
                grid[point_id].combinations[p_ptr] = neg_s;
                ++p_ptr;
                grid[point_id].comb_num += 1;
            }
        }
        // subset size > 2
        // readlly need to solve subset sum problem
    }

    free(num_count);
    free(f_max);
    free(f_min);
    free(f_max_acc);
    free(f_min_acc);
    return grid;
}
