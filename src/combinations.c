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

#define ALLOC_STEP 10
#define HCOMB_LEN 2


// get combinations of density == 1/2
uint32_t *_get_combs_of_two(Pattern *patterns, uint32_t pat_num, Dir_Rev *dir_rev, uint32_t *found_num)
{
    uint32_t found_allocated = ALLOC_STEP * HCOMB_LEN;
    uint32_t *found = (uint32_t*)calloc(found_allocated, sizeof(uint32_t));
    uint32_t rev_id = 0;
    uint32_t dir_id = 0;
    for (uint32_t i = 0; i < pat_num; ++i)
    {   
        if (*found_num > found_allocated - 2){
            found_allocated += ALLOC_STEP;
            found = (uint32_t*)realloc(found, found_allocated * sizeof(uint32_t));
        }
        if (!dir_rev[i].is_dir){continue;}
        if (patterns[i].times < 2){continue;}
        // only direct patterns appearing > once
        dir_id = dir_rev[i].dir;
        rev_id = dir_rev[i].rev;
        found[*found_num] = dir_id;
        *found_num += 1;
        found[*found_num] = rev_id;
        *found_num += 1;
    }
    found = (uint32_t*)realloc(found, *found_num * sizeof(uint32_t));
    return found;
}


// extractor itself; need to fix confusing function names
uint32_t *_get_combs(uint32_t *sizes_arr, Size_index *size_index, Pattern *patterns, Dir_Rev *dir_rev,
                    uint32_t size, uint32_t pat_num, uint32_t *ones_ind, uint32_t str_num, uint32_t *_found)
{
    uint32_t allocated = ALLOC_STEP * size;
    uint32_t *ans = (uint32_t*)calloc(allocated, sizeof(uint32_t));
    return ans;
}


// heavy stuff - extract trivial combinations, wrapper
uint32_t *_get_combs_from_point(Point point, Pattern *patterns, Dir_Rev *dir_rev, uint32_t pat_num,
                                Size_index *size_index, uint32_t *ones_ind, uint32_t str_num, uint32_t *found_num)
{
    uint32_t size = point.comb_size;
    uint32_t *sizes_arr = (uint32_t*)calloc(size, sizeof(uint32_t));
    uint32_t s_arr_start = 0;
    uint32_t s_arr_end = 0;
    uint32_t found_in_iter = 0;
    uint32_t found_allocated = ALLOC_STEP * size;
    uint32_t *found = (uint32_t*)calloc(found_allocated, sizeof(uint32_t));
    for (uint32_t comb_num = 0; comb_num < point.comb_num; ++comb_num)
    {
        s_arr_start = comb_num * size;
        s_arr_end = s_arr_start + size;
        for (uint32_t i = s_arr_start, j = 0; i < s_arr_end; ++i, ++j){
            sizes_arr[j] = point.combinations[i];
        }
        found_in_iter = 0;
        uint32_t *combs_found = _get_combs(sizes_arr, size_index, patterns, dir_rev, size,
                                           pat_num, ones_ind, str_num, &found_in_iter);
        free(combs_found);
    }
    free(sizes_arr);
    return found;
}


// extract combinations according the grid guides
Combination *extract_combinations(Point *grid, uint32_t grid_size, Pattern *patterns, uint32_t pat_num,
                                  Size_index *size_index, uint32_t str_len, uint32_t str_num, uint32_t *ones_ind,
                                  Dir_Rev *dir_rev, uint32_t *combinations_num)
{
    uint32_t comb_allocated = ALLOC_STEP;
    uint32_t comb_found = 0;
    uint32_t found_num = 0;
    Combination *combinations = (Combination*)malloc(comb_allocated * sizeof(Combination));

    // go point-by-point
    for (uint32_t p_num = 0; p_num < grid_size; ++p_num)
    {
        // no ways for this point -> skip it
        if (grid[p_num].comb_num == 0){continue;}
        // density == 1/2
        if (grid[p_num].comb_size == 2)
        {
            found_num = 0;
            uint32_t *combs_of_two = _get_combs_of_two(patterns, pat_num, dir_rev, &found_num);
            uint32_t found_pt = 0;
            uint32_t to_fill = found_num / 2;
            if (to_fill > comb_allocated){
                comb_allocated += to_fill;
                combinations = (Combination*)realloc(combinations, comb_allocated * sizeof(Combination));
            }
            for (uint32_t i = 0; i < to_fill; ++i){
                combinations[i].density = 0.5;
                combinations[i].size = 2;
                combinations[i].id = i;
                combinations[i].patterns = (uint32_t*)malloc(2 * sizeof(uint32_t));
                combinations[i].patterns[0] = combs_of_two[found_pt];
                ++found_pt;
                combinations[i].patterns[1] = combs_of_two[found_pt];
                ++found_pt;
                ++comb_found;
            }
            *combinations_num = comb_found;
            free(combs_of_two);
            continue;
        }
        // ok, more complicated case
        found_num = 0;
        uint32_t *combs_here = _get_combs_from_point(grid[p_num], patterns, dir_rev, pat_num,
                                                     size_index, ones_ind, str_num, &found_num);
        free(combs_here);
    }
    return combinations;
}
