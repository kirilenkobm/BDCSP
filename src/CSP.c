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
#include "patterns.h"
#include "grid.h"


uint32_t pattern_seq_to_id(pat_list_search_elem *pat_search_list, int l, int r, uint32_t x) 
{
    if (r >= l)
    { 
        int mid = l + (r - l) / 2; 
        if (pat_search_list[mid].pat_num == x) {
            return pat_search_list[mid].pat_id;
        } else if (pat_search_list[mid].pat_num > x) {
            return pattern_seq_to_id(pat_search_list, l, mid - 1, x);
        } else {
        return pattern_seq_to_id(pat_search_list, mid + 1, r, x);
        }
    }
    // nothing found; should never happen, but...
    return 0;
}


int solve_CSP(uint32_t str_num, uint32_t str_len, uint32_t k_, uint32_t pat_num, uint32_t pos_num,
              uint8_t *patterns_1D_arr, uint32_t *pat_to_pos_1D, uint32_t *pat_to_pos_num,
              uint32_t *pat_to_pos_starts, uint32_t *pos_1D_arr, uint32_t *pos_to_pat)
// entry point; returns 1 if a solution with the K given exists, 0 otherwise
{   
    bool answer = false;
    uint32_t to_cover = str_len - k_;
    // make verbosity parameter later
    printf("# StrLen: %d, K_: %d, to cover: %d\n", str_len, k_, to_cover);
    double exp_ave_ro = (double)to_cover / str_len;
    printf("# Expected Ro: %f\n", exp_ave_ro);
    double inf = 1.0 / str_num;
    double sup = (double)(str_num - 1) / str_num; 
    printf("# Minimal density: %f, maximal density: %f\n", inf, sup);

    // trim obvious cases
    if (exp_ave_ro > sup){return false;  // unreachable density
    } else if (inf >= exp_ave_ro){return true;}  // reachable with any set of combinations

    // read all this stuff
    printf("# Overall %d patterns\n", pat_num);
    pattern *patterns = malloc(sizeof(pattern) * pat_num);
    uint32_t p_start, p_end;
    uint16_t *size_times = (uint16_t*)calloc(str_num, sizeof(uint16_t));
    pat_list_search_elem *pat_search_list = malloc(sizeof(pat_list_search_elem) \
                                                   * pat_num);

    // read patterns first
    for (uint32_t i = 0; i < pat_num; i++)
    {
        // str_num == pattern length
        // read pattern sequences
        p_start = i * str_num;
        p_end = p_start + str_num;
        patterns[i].pattern_seq = malloc(sizeof(uint8_t) * str_num);
        for (uint32_t j = p_start, n = 0; j < p_end; j++, n++){
            patterns[i].pattern_seq[n] = patterns_1D_arr[j];}
        patterns[i].id = i;  // obviously id
        // get occupied positions
        uint32_t occupies_size = sum_pattern(str_num, patterns[i].pattern_seq);
        patterns[i].occupies_num = occupies_size;
        size_times[occupies_size]++;
        patterns[i].occupies = malloc(sizeof(uint32_t) * occupies_size);
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
        patterns[i].positions = malloc(sizeof(uint32_t) * pat_pos_arr_size);
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
        uint32_t *intersects_lst = malloc(sizeof(int) * pat_num_downstream);
        uint32_t *non_intersects_lst = malloc(sizeof(int) * pat_num_downstream);
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
        patterns[i].intersects_with = malloc(sizeof(int) * intersects);
        patterns[i].inters_num = intersects;
        for (uint32_t j = 0; j < intersects; j++)
        {
            patterns[i].intersects_with[j] = intersects_lst[j];
        }
        patterns[i].not_intersects_with = malloc(sizeof(int) * non_intersects);
        patterns[i].non_inters_num = non_intersects;
        for (uint32_t j = 0; j < non_intersects; j++)
        {
            patterns[i].not_intersects_with[j] = non_intersects_lst[j];
        }
        free(intersects_lst);
        free(non_intersects_lst);
    }

    // sort pattern search array
    // sorted from smaller to bigger
    qsort(pat_search_list, pat_num, sizeof(pat_list_search_elem), comp_search_elems);

    // read positions then
    // uint32_t *pattern_rev = malloc(sizeof(int) * pat_num);
    position *positions = malloc(sizeof(positions) * pos_num);

    // each position intersects with 2 patterns
    uint32_t first_pattern;
    uint32_t second_pattern;

    for (uint32_t i = 0; i < pos_num; i++)
    {
        positions[i].number = pos_1D_arr[i];
        first_pattern = pos_to_pat[i * 2];
        second_pattern = pos_to_pat[i * 2 + 1];
        // positions[i].patterns = malloc(sizeof(int) * 2);
        positions[i].patterns[0] = first_pattern;
        positions[i].patterns[1] = second_pattern;
        // pattern_rev[first_pattern] = second_pattern;
        // pattern_rev[second_pattern] = first_pattern;
        patterns[first_pattern].pattern_rev = second_pattern;
        patterns[second_pattern].pattern_rev = first_pattern;
    }

    // now we can go throw grid and try to find the positions
    Point *grid = make_grid(pos_num, str_num, size_times);

    // free memory!
    for (uint32_t i = 0; i < pat_num; i++)
    {
        // left print patterns for now
        // will remove later
        for (uint32_t j = 0; j < str_num; j++)
        {
            printf("%d ", patterns[i].pattern_seq[j]);
        }
        printf("| pattern id %d\n", i);
        free(patterns[i].pattern_seq);
        free(patterns[i].occupies);
        free(patterns[i].positions);
        free(patterns[i].intersects_with);
        free(patterns[i].not_intersects_with);
    }
    free(patterns);
    free(positions);
    // free(pattern_rev);
    free(size_times);
    free(pat_search_list);
    return answer;
}
