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
#include "patterns.h"


int solve_CSP(int str_num, int str_len, int k_, int pat_num, int pos_num, 
              int *patterns_1D_arr, int *pat_to_pos_1D, int *pat_to_pos_num,
              int *pat_to_pos_starts, int *pos_1D_arr, int *pos_to_pat)
// entry point; returns 1 if a solution with the K given exists, 0 otherwise
{   
    bool answer = false;
    int to_cover = str_len - k_;
    // make verbosity parameter later
    printf("# StrLen: %d, K_: %d, to cover: %d\n", str_len, k_, to_cover);
    if (to_cover < 0){return true;}  // very obvious case
    double exp_ave_ro = (double)to_cover / str_len;
    printf("# Expected Ro: %f\n", exp_ave_ro);
    double inf = 1.0 / str_num;
    double sup = (double)(str_num - 1) / str_num; 
    printf("# Minimal density: %f, maximal density: %f\n", inf, sup);

    // trim obvious cases
    if (exp_ave_ro > sup){return false;}  // unreachable density
    else if (inf >= exp_ave_ro){return true;}  // reachable with any set of combinations

    // read all this stuff
    printf("# Overall %d patterns\n", pat_num);
    pattern *patterns = malloc(sizeof(pattern) * pat_num);
    int p_start, p_end;
    // read patterns first
    for (int i = 0; i < pat_num; i++){
        // str_num == pattern length
        // read pattern sequences
        p_start = i * str_num;
        p_end = p_start + str_num;
        patterns[i].pattern_seq = malloc(sizeof(int) * str_num);
        for (int j = p_start, n = 0; j < p_end; j++, n++){
            patterns[i].pattern_seq[n] = patterns_1D_arr[j];}
        patterns[i].id = i;  // obviously id
        // get occupied positions
        int occupies_size = sum_pattern(str_num, patterns[i].pattern_seq);
        patterns[i].occupies_num = occupies_size;
        patterns[i].occupies = malloc(sizeof(int) * occupies_size);
        int j_n = 0;
        for (int j = 0; j < str_num; j++){
            if (patterns[i].pattern_seq[j] == 1){
                patterns[i].occupies[j_n] = j;
                j_n++;}}
        // also positions
        int pat_pos_arr_starts = pat_to_pos_starts[i];
        int pat_pos_arr_size = pat_to_pos_num[i];
        patterns[i].positions = malloc(sizeof(int) * pat_pos_arr_size);
        // bulky for initiation
        for (int j = pat_pos_arr_starts, n = 0;
             j < pat_pos_arr_starts + pat_pos_arr_size;
             j++, n++) {patterns[i].positions[n] = pat_to_pos_1D[j];}
        // positions added; what's left:
        // pattern intersects with and not intersects with
        // at the latter stage
    }
    // patterns and positions are packed in the two lists of structures
    // now we can find intersecting and non-intersecting patterns
    for (int i = 0; i < pat_num; i++){
        // find intersections and non-intersections of smaller id
        // not the most efficient algorithm though
        if (i == pat_num - 1){
            // terminal pattern; never start with it
            patterns[i].intersects_with = NULL;
            patterns[i].not_intersects_with = NULL;
            break;}
        // define what to compare and compare
        int pat_num_downstream = pat_num - (i + 1);
        int intersects = 0;
        int non_intersects = 0;
        int *intersects_lst = malloc(sizeof(int) * pat_num_downstream);
        int *non_intersects_lst = malloc(sizeof(int) * pat_num_downstream);
        for (int j = i + 1; j < pat_num; j++){
            // if true -> to intersected, otherwise to non-intersected
            bool i_j_inter = intersect_by_occ(patterns[i].occupies, patterns[i].occupies_num,
                                              patterns[j].occupies, patterns[j].occupies_num);
            if (i_j_inter){
                intersects_lst[intersects] = j;
                intersects++;
            } else {
                non_intersects_lst[non_intersects] = j;
                non_intersects++;}}
        // save to struct
        patterns[i].intersects_with = malloc(sizeof(int) * intersects);
        patterns[i].inters_num = intersects;
        for (int j = 0; j < intersects; j++){
            patterns[i].intersects_with[j] = intersects_lst[j];}
        patterns[i].not_intersects_with = malloc(sizeof(int) * non_intersects);
        patterns[i].non_inters_num = non_intersects;
        for (int j = 0; j < non_intersects; j++){
            patterns[i].not_intersects_with[j] = non_intersects_lst[j];}
        free(intersects_lst);
        free(non_intersects_lst);
    }

    // read positions then
    printf("# Overal %d positions\n", pos_num);
    position *positions = malloc(sizeof(positions) * pos_num);
    // each position intersects with 2 patterns
    int first_pattern, second_pattern;
    for (int i = 0; i < pos_num; i++){
        positions[i].number = pos_1D_arr[i];
        first_pattern = pos_to_pat[i * 2];
        second_pattern = pos_to_pat[i * 2 + 1];
        positions[i].patterns = malloc(sizeof(int) * 2);
        positions[i].patterns[0] = first_pattern;
        positions[i].patterns[1] = second_pattern;
    }

    // free memory!
    for (int i = 0; i < pat_num; i++){
        free(patterns[i].pattern_seq);
        free(patterns[i].occupies);
        free(patterns[i].positions);
        free(patterns[i].intersects_with);
        free(patterns[i].not_intersects_with);
    }
    free(patterns);
    free(positions);
    return answer;
}
