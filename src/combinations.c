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


void __print_arr(uint32_t *arr, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i){printf("%u ", arr[i]);}
    printf("\n");
}


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


// get AND for bool arrays
void _and_over_bool_arr(bool *to, bool *from, uint32_t arr_len)
{
    for (uint32_t i = 0; i < arr_len; ++i){
        to[i] = (to[i] && from[i]);
    }
}

// get mask of compatible patterns
bool *_get_compatibles(uint32_t *chain, Pattern *patterns, uint32_t chain_len, uint32_t pat_num)
{
    bool *compatibles = (bool*)malloc(pat_num * sizeof(bool));
    for (uint32_t i = 0; i < pat_num; ++i){compatibles[i] = true;}
    for (uint32_t i = 0; i < chain_len; ++i){
        _and_over_bool_arr(compatibles, patterns[chain[i]].no_intersect, pat_num);
    }
    // for (uint32_t i = 0; i < pat_num; i++){printf("%d ", compatibles[i]);}
    // printf("\n");
    return compatibles;
}


// get pattern ids we can continue with
uint32_t *_get_to_continue
(uint32_t *size_fit, uint32_t size_fit_size, bool *compat_mask,
uint32_t pat_num, uint32_t *counter, uint32_t last_elem)
{
    uint32_t *answer = (uint32_t*)calloc(size_fit_size, sizeof(uint32_t));
    for (uint32_t i = 0; i < size_fit_size; ++i){
        if (size_fit[i] < last_elem){continue;}
        else if (!compat_mask[size_fit[i]]){continue;}
        answer[*counter] = size_fit[i];
        *counter += 1;
    }
    return answer;
}


// copy from one arr to another
void _copy_to_buff(uint32_t *to, uint32_t *from, uint32_t num)
{
    for (uint32_t i = 0; i < num; ++i){to[i] = from[i];}
}


// copy paste buffer to chain
void _copy_buf_to_chain
(uint32_t **chain, uint32_t **buffer, uint32_t iter_num, uint32_t buff_used, uint32_t path_len)
{   
    for (uint32_t i = 0; i < buff_used; ++i){
        chain[i] = calloc(path_len, sizeof(uint32_t));
        for (uint32_t j = 0; j < iter_num; ++j){
            chain[i][j] = buffer[i][j];
        }
    }
}


void __print_chain(uint32_t **chain, uint32_t chains_num, uint32_t depth)
{
    for (uint32_t i = 0; i < chains_num; ++i){
        for (uint32_t d = 0; d < depth; ++d){
            printf("%u ", chain[i][d]);
        }
        printf("\n");
    }
}


// find combinations forming the size path
uint32_t **_find_combinations
(uint32_t *size_path, uint32_t path_size, Size_index *size_index, uint32_t str_num,
Pattern *patterns, uint32_t pat_num, uint32_t *count)
{
    uint32_t allocated_combs = ALLOC_STEP;
    uint32_t **answer = (uint32_t**)malloc(allocated_combs * sizeof(uint32_t*));
    // allocate chain for first iteration
    uint32_t allocated_chain = size_index[size_path[0]].num;
    uint32_t **chain = (uint32_t**)malloc(allocated_chain * sizeof(uint32_t*));
    for (uint32_t i = 0; i < allocated_chain; ++i){
        chain[i] = (uint32_t*)calloc(path_size, sizeof(uint32_t));
        chain[i][0] = size_index[size_path[0]].ids[i];
    }

    uint32_t extensions = 0;
    uint32_t sizes_num_here = 0;
    uint32_t *sizes_fit;
    uint32_t buff_used = 0;


    // add elements to chain one-by-one
    for (uint32_t iter_num = 1; iter_num < path_size; ++iter_num)
    {
        // initiate buffer to keep intermediate results
        printf("PRINTED CHAIN:\n");
        __print_chain(chain, allocated_chain - 1, path_size);
        uint32_t buff_allocated = ALLOC_STEP;
        buff_used = 0;
        uint32_t **buffer = (uint32_t**)malloc(buff_allocated * sizeof(uint32_t));
        // go over the chain elements, find extensions
        for (uint32_t ch_num = 0; ch_num < allocated_chain - 1; ++ch_num)
        {
            uint32_t *current_chain = chain[ch_num];
            uint32_t last_in_the_chain = current_chain[iter_num - 1];
            printf("Last in the chain %u\n", last_in_the_chain);
            // get compatibility mask
            bool *chain_compatibles = _get_compatibles(current_chain, patterns, iter_num, pat_num);
            sizes_fit = size_index[size_path[iter_num]].ids;
            sizes_num_here = size_index[size_path[iter_num]].num;
            // get pattern ids which are:
            // - less than the last id in the chain
            // - compatible with the chain
            // - have proper size
            uint32_t continue_size = 0;
            uint32_t *continue_with = _get_to_continue(sizes_fit, sizes_num_here, chain_compatibles,
                                                       pat_num, &continue_size, last_in_the_chain);

            if (continue_size == 0){
                // no way to continue chain
                printf("Nothing found for %u\n", last_in_the_chain);
                free(chain_compatibles);
                free(continue_with);
                continue;
            }

            // extend buffer if needed
            if ((buff_used + continue_size) > (buff_allocated - 1)){
                buff_allocated += (continue_size + ALLOC_STEP);
                buffer = (uint32_t**)realloc(buffer, buff_allocated * sizeof(uint32_t*));
            }

            // fill buffer with chain data + extensions
            for (uint32_t i = 0; i < continue_size; ++i){
                buffer[buff_used] = (uint32_t*)calloc(path_size, sizeof(uint32_t));
                _copy_to_buff(buffer[buff_used], chain[ch_num], iter_num);
                buffer[buff_used][iter_num] = continue_with[i];
                ++buff_used;
            }
            // add data to buffer  
            free(continue_with);
            free(chain_compatibles);
        }
        printf("buf_used is %u \n", buff_used);
        // for (uint32_t i = 0; i < allocated_chain; ++i){free(chain[i]);}
        // free(chain);
        // no extensions -> couldn't grow the chain -> faile
        if (buff_used == 0){
            free(buffer);
            return answer;
        }
        // extended: replace chain with buffer
        allocated_chain = buff_used;
        chain = (uint32_t**)realloc(chain, allocated_chain * sizeof(uint32_t*));

        __print_chain(buffer, buff_used, path_size);
        _copy_buf_to_chain(chain, buffer, iter_num + 1, buff_used, path_size);
        for (uint32_t i = 0; i < buff_used; ++i){free(buffer[i]);}
        free(buffer);
    }

    printf("OK NEED TO RETURN ANSWER\n");
    printf("In result the chain is:\n");
    __print_chain(chain, allocated_chain, path_size);
    // free chain
    for (uint32_t i = 0; i < allocated_chain; ++i){free(chain[i]);}
    free(chain);
    exit(0);
    return answer;
}


// PROBLEM: very confusing function names
// extract combinations according the grid guides
Combination *extract_combinations
(Point *grid, uint32_t grid_size, Pattern *patterns, uint32_t pat_num, Size_index *size_index, uint32_t str_len,
uint32_t str_num, uint32_t *ones_ind, Dir_Rev *dir_rev, uint32_t *combinations_num)
{
    uint32_t comb_allocated = ALLOC_STEP;
    uint32_t comb_found = 0;
    uint32_t comb_found_for_path = 0;
    uint32_t found_num = 0;
    Combination *combinations = (Combination*)malloc(comb_allocated * sizeof(Combination));
    uint32_t s_path_start;
    uint32_t s_path_end;

    // go point-by-point
    for (uint32_t point_num = 0; point_num < grid_size; ++point_num)
    {
        // no ways for this point -> skip it
        if (grid[point_num].comb_num == 0){continue;}
        // density == 1/2
        if (grid[point_num].comb_size == 2)
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
        // each point contains >= 1 size paths
        // need to process these paths separately
        for (uint32_t path_num = 0; path_num < grid[point_num].comb_num; ++path_num)
        {
            // so, need to extract the path first
            s_path_start = path_num * grid[point_num].comb_size;
            s_path_end = s_path_start + grid[point_num].comb_size;
            uint32_t *size_path = (uint32_t*)malloc(grid[point_num].comb_size * sizeof(uint32_t));
            for (uint32_t i = s_path_start, j = 0; i < s_path_end; ++i, ++j){
                size_path[j] = grid[point_num].combinations[i];
                printf("%u ", size_path[j]);
            }
            printf("\n");
            // extracted sizes, let's find patterns
            comb_found_for_path = 0;
            uint32_t **search_res = _find_combinations(size_path, grid[point_num].comb_size, size_index,
                                                       str_num, patterns, pat_num, &comb_found_for_path);
            if (comb_found_for_path == 0){
                free(size_path);
                free(search_res);
                continue;
            }

            for (uint32_t i = 0; i < comb_found_for_path; ++i){free(search_res[i]);}
            free(search_res);
            free(size_path);
        }
    }
    return combinations;
}
