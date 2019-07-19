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
#include <stdlib.h>
#include "patterns.h"
#include "CSP.h"
#include "grid.h"
#define CHUNK 2


uint32_t min_of_three(uint32_t *a, uint32_t *b, uint32_t *c)
// just return a min of three numbers
{
    if (*a < *b && *a < *c){
        return *a;
    // ok, not a, min of b and c
    } else if (*b < *c){
        return *b;
    // not b and not a -> clearly c
    } else {
        return *c;
    }
}


bool solve_CSP(uint32_t str_num, uint32_t str_len, uint32_t k_, uint32_t pat_num,
               uint32_t pos_num, uint8_t *patterns_1D_arr, uint32_t *pat_to_pos_1D,
               uint32_t *pat_to_pos_num, uint32_t *pat_to_pos_starts,
               uint32_t *pos_1D_arr, uint32_t *pos_to_pat)
// entry point; returns 1 if a solution with the K given exists, 0 otherwise
{   
    bool answer = false;
    size_t total_memory_allocated = 0;
    uint32_t to_cover = str_len - k_;
    // make verbosity parameter later
    printf("# StrLen: %d, K_: %d, to cover: %d\n", str_len, k_, to_cover);

    double exp_ave_ro = (double)to_cover / str_len;
    printf("# Expected Ro: %f\n", exp_ave_ro);

    double inf = 1.0 / str_num;
    double sup = (double)(str_num - 1) / str_num; 
    printf("# Minimal density: %f, maximal density: %f\n", inf, sup);

    uint32_t max_comb_len = min_of_three(&str_num, &str_len, &pat_num);
    printf("# Maximal combination length: %d", max_comb_len);

    // trim obvious cases
    if (exp_ave_ro > sup){return false;  // unreachable density
    } else if (inf >= exp_ave_ro){return true;}  // reachable with any set of combinations

    // read all this stuff
    printf("# Overall %d patterns\n", pat_num);
    uint64_t *max_pat_num = get_max_pat_num(str_num);
    size_t max_pat_num_size = sizeof(uint64_t) * str_num;
    total_memory_allocated += max_pat_num_size;

    Pattern *patterns = (Pattern*)malloc(sizeof(Pattern) * (pat_num + CHUNK));
    uint32_t p_start, p_end;

    size_t size_times_size = sizeof(uint32_t) * (str_num + CHUNK);
    uint32_t *size_times = (uint32_t*)calloc((str_num + CHUNK), sizeof(uint32_t));
    size_t search_list_size = sizeof(Pat_list_search_elem) * (pat_num + CHUNK);
    Pat_list_search_elem *pat_search_list = (Pat_list_search_elem*)malloc(search_list_size);

    // printf("Allocated %zu for search list\n", search_list_size);
    // printf("Allocated %zu for size times array\n", size_times_size);
    total_memory_allocated += search_list_size;
    total_memory_allocated += size_times_size;

    size_t pat_occupies_total = 0;
    size_t pat_pos_total = 0;
    size_t pat_seq_total = 0;

    // read patterns first
    for (uint32_t i = 0; i < pat_num; i++)
    {
        // str_num == pattern length
        // read pattern sequences
        p_start = i * str_num;
        p_end = p_start + str_num;
        size_t pat_seq_size = sizeof(uint8_t) * (str_num + CHUNK);
        patterns[i].pattern_seq = (uint8_t*)malloc(pat_seq_size);
        pat_seq_total += pat_seq_size;
        patterns[i].id = i;  // obviously id

        for (uint32_t j = p_start, n = 0; j < p_end; j++, n++){
            patterns[i].pattern_seq[n] = patterns_1D_arr[j];}
        
        // get occupied positions
        uint32_t occupies_size = sum_pattern(str_num, patterns[i].pattern_seq);
        // printf("Pattern %d occupies %d\n", i, occupies_size);
        patterns[i].occupies_num = occupies_size;
        size_times[occupies_size]++;
        size_t pattern_occ_size = sizeof(uint32_t) * (occupies_size + 1);
        pat_occupies_total += pattern_occ_size;
        patterns[i].occupies = (uint32_t*)malloc(pattern_occ_size);

        uint32_t j_n = 0;
        for (uint32_t j = 0; j < str_num; j++)
        {
            if (patterns[i].pattern_seq[j] == 1)
            {
                patterns[i].occupies[j_n] = j;
                j_n++;
            }
        }

        // also positions
        uint32_t pat_pos_arr_starts = pat_to_pos_starts[i];
        uint32_t pat_pos_arr_size = pat_to_pos_num[i];
        size_t pat_pos_list_size = sizeof(uint32_t) * (pat_pos_arr_size + 1);
        pat_pos_total += pat_pos_list_size;
        patterns[i].positions = (uint32_t*)malloc(pat_pos_list_size);

        // bulky for initiation
        for (uint32_t j = pat_pos_arr_starts, n = 0;
             j < pat_pos_arr_starts + pat_pos_arr_size;
             j++, n++) {patterns[i].positions[n] = pat_to_pos_1D[j];}

        // add elem to search function
        pat_search_list[i].pat_num = pat_to_num(str_num, patterns[i].pattern_seq);
        pat_search_list[i].pat_id = i;
        // positions added; what's left:
        // pattern intersects with and not intersects with
        // at the latter stage
    }

    printf("Max possible and real number of patterns at lvl %u is:\n", str_num);
    for (uint32_t i = 1; i < str_num; i++){
        printf("Of size %u max: %llu actual: %u\n", i, max_pat_num[i], size_times[i]);
    }

    // printf("Totally allocated %zu for pattern sequences\n", pat_seq_total);
    // printf("Totally allocated %zu for patterns occupy\n", pat_occupies_total);
    // printf("Totally allocated %zu for patterns positions\n", pat_pos_total);
    total_memory_allocated += pat_seq_total;
    total_memory_allocated += pat_occupies_total;
    total_memory_allocated += pat_pos_total;

    size_t intersects_size_total = 0;
    // patterns and positions are packed in the two lists of structures
    // now we can find intersecting and non-intersecting patterns
    for (uint32_t i = 0; i < pat_num; i++)
    {
        // find intersections and non-intersections of smaller id
        // not the most efficient algorithm though
        if (i == pat_num - 1)
        {
            // terminal pattern; never start with it
            patterns[i].intersects_with = NULL;
            patterns[i].not_intersects_with = NULL;
            break;
        }

        // define what to compare and compare
        uint32_t pat_num_downstream = pat_num - (i + 1);
        uint32_t intersects = 0;
        uint32_t non_intersects = 0;
        size_t inter_size = sizeof(uint32_t)  * (pat_num_downstream + CHUNK);
        uint32_t* intersects_lst = (uint32_t*)malloc(inter_size);
        uint32_t* non_intersects_lst = (uint32_t*)malloc(inter_size);

        for (uint32_t j = i + 1; j < pat_num; j++)
        {
            // if true -> to intersected, otherwise to non-intersected
            bool i_j_inter = intersect_by_occ(patterns[i].occupies,
                                              patterns[i].occupies_num,
                                              patterns[j].occupies,
                                              patterns[j].occupies_num);
            if (i_j_inter){
                intersects_lst[intersects] = j;
                intersects++;
            } else {
                non_intersects_lst[non_intersects] = j;
                non_intersects++;
            }
        }
        // save to struct
        size_t intersects_size = sizeof(uint32_t) * (intersects + CHUNK);
        patterns[i].intersects_with = (uint32_t*)malloc(intersects_size);
        patterns[i].inters_num = intersects;

        for (uint32_t j = 0; j < intersects; j++)
        {
            patterns[i].intersects_with[j] = intersects_lst[j];
        }        
        
        size_t non_intersects_size = sizeof(uint32_t) * (non_intersects + CHUNK);
        patterns[i].not_intersects_with = (uint32_t*)malloc(non_intersects_size);
        patterns[i].non_inters_num = non_intersects;
        intersects_size_total += intersects_size;
        intersects_size_total += non_intersects_size;

        for (uint32_t j = 0; j < non_intersects; j++)
        {
            patterns[i].not_intersects_with[j] = non_intersects_lst[j];
        }

        free(intersects_lst);
        free(non_intersects_lst);
    }

    // printf("Allocated %zu for intersect patterns\n", intersects_size_total);
    total_memory_allocated += intersects_size_total;
    // sort pattern search array
    // sorted from smaller to bigger
    qsort(pat_search_list, pat_num, sizeof(Pat_list_search_elem), comp_search_elems);

    // read positions then
    size_t pos_array_size = sizeof(Position) * (pos_num + CHUNK);
    Position *positions = (Position*)malloc(pos_array_size);
    // printf("Allocated %zu for positions array\n", pos_array_size);
    total_memory_allocated += pos_array_size;

    // each position intersects with 2 patterns
    uint32_t first_pattern;
    uint32_t second_pattern;

    for (uint32_t i = 0; i < pos_num; i++)
    {
        positions[i].number = pos_1D_arr[i];
        first_pattern = pos_to_pat[i * 2];
        second_pattern = pos_to_pat[i * 2 + 1];
        positions[i].patterns[0] = first_pattern;
        positions[i].patterns[1] = second_pattern;
        patterns[first_pattern].pattern_rev = second_pattern;
        patterns[second_pattern].pattern_rev = first_pattern;
    }

    // now we can go throw grid and try to find the positions
    Grid_output grid = make_grid(str_num, pat_num, patterns, max_comb_len);
    for (uint32_t i = 0; i < grid.grid_len; i++)
    {
        uint32_t **size_paths = get_size_path(&grid.grid[i], size_times, str_num);
    }
    // also count this
    total_memory_allocated += grid.grid_mem_size;
    // done
    // free memory!
    printf("# Totally memory allocated: %0.3f kb\n",
        (double)total_memory_allocated / 1024);
    // Different amount of memory allocated all the time!
    for (uint32_t i = 0; i < pat_num; i++)
    {
        free(patterns[i].pattern_seq);
        free(patterns[i].occupies);
        free(patterns[i].positions);
        free(patterns[i].intersects_with);
        free(patterns[i].not_intersects_with);
    }
    free(patterns);
    free(positions);
    free(size_times);
    free(pat_search_list);
    free(grid.grid);
    return answer;
}
