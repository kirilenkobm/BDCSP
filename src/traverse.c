// Bogdan Kirilenko
// 2019, Dresden
// Walking over hypercube
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
#include "patterns.h"
#include "render.h"
#include "traverse.h"
#define MOVES_STEP 10
#define N printf("\n");  // temp shit

// typedef struct{
//     uint32_t pat_id;
//     int8_t size;
// } Move;

// typedef struct{
//     uint32_t *pat_mask;
//     Move *moves;
//     uint32_t moves_num;
//     uint32_t cur_move;
// } State;

// typedef struct{
//     uint32_t min_zeros;
//     int8_t min_zeros_delta;
//     uint32_t minus_;
//     uint32_t plus_;
//     uint32_t assign_to;
// } Z_compare;


// comparator for Z results
int compare_Z_compares(const void *a, const void *b)
{ 
    Z_compare *ia = (Z_compare *)a;
    Z_compare *ib = (Z_compare *)b;
    // different deltas on min value?
    if (ia->min_zeros_delta < ib->min_zeros_delta){return -1;}
    else if (ia->min_zeros_delta > ib->min_zeros_delta){return 1;}
    // maybe something else is different
    if (ia->minus_ > ib->minus_){return -1;}
    else if (ia->minus_ < ib->minus_){return 1;}
    return 0;  // the same for nom
}


// create array copy
uint32_t *traverse__mask_copy(uint32_t *mask, uint32_t len)
{
    uint32_t *ans = (uint32_t*)calloc(len, sizeof(uint32_t));
    for (uint32_t i = 0; i < len; ++i){ans[i] = mask[i];}
    return ans;
}


// just print Z compare data
void __print_Z_compare(Z_compare *z_comp)
{
    printf("#Z Min zeros: %u; Mz delta: %d\n", z_comp->min_zeros, z_comp->min_zeros_delta);
    printf("#Z Minus: %u Plus: %u\n", z_comp->minus_, z_comp->plus_);
    printf("Assigned to: %u\n", z_comp->assign_to_move);
    printf("\n");
}


// compare two zero-distr outputs
Z_compare compare_Z_dist(uint32_t *before, uint32_t *after, uint32_t len)
{
    Z_compare res;
    res.min_zeros = 0;
    res.min_zeros_delta = 0;
    res.minus_ = 0;
    res.plus_ = 0;
    // printf("Before: \n");
    // for (uint32_t i = 0; i < len; ++i){printf("%u ", before[i]);}
    // printf("\nAfter:\n");
    // for (uint32_t i = 0; i < len; ++i){printf("%u ", after[i]);}
    // N
    uint32_t before_max =  arr_max(before, len);
    uint32_t after_max = arr_max(after, len);
    res.min_zeros_delta = after_max - before_max;
    printf("Min delta: %d\n", res.min_zeros_delta);
    for (uint32_t i = 0; i < len; ++i){
        if (before[i] == after[i]){continue;}
        else if (before[i] > after[i]){res.minus_ += (before[i] - after[i]);}
        else {res.plus_ += (after[i] - before[i]);}
    }
    printf("Minuses: %u pluses: %u\n", res.minus_, res.plus_);
    N
    return res;
}


// call in-depth search for answer
bool traverse__run
(uint32_t *zero_mask, uint32_t *full_mask, uint32_t *init_z_dist, Input_data *input_data, Pattern *patterns)
{
    bool res = false;  // default answer
    uint32_t states_num = input_data->act_col_num * 4;
    verbose("# Search depth %u\n", states_num);
    State *states = (State*)malloc(states_num * sizeof(State));
    uint32_t mask_size = (input_data->dir_pat_num + 1);

    // initiate states with default values
    for (uint32_t i = 0; i < states_num; ++i){
        states[i].pat_mask = (uint32_t*)calloc(mask_size, sizeof(uint32_t));
        states[i].moves = (Move*)malloc(MOVES_STEP * sizeof(Move));
        states[i].moves_num = 0;
        states[i].cur_move = 0;
    }

    states[0].pat_mask = traverse__mask_copy(zero_mask, mask_size);
    states[0].moves = NULL;

    // initiate moves, only + moves available
    Move *initial_moves = (Move*)malloc(sizeof(Move) * input_data->dir_pat_num);
    Z_compare* init_compares = (Z_compare*)malloc(sizeof(Z_compare) * input_data->dir_pat_num);

    uint32_t i_moves_count = 0;

    uint32_t *move_mask = traverse__mask_copy(zero_mask, mask_size);

    for (uint32_t p_num = 1; p_num < mask_size; ++p_num){
        uint32_t *move_mask = traverse__mask_copy(zero_mask, mask_size);
        move_mask[p_num] = 1;
        uint8_t **move_render = render__draw(patterns, move_mask, input_data);
        // render__show_arr(move_render, input_data->str_num, input_data->act_col_num);
        uint32_t *zeros_dist = render__get_zeros(move_render,
                                                 input_data->str_num, 
                                                 input_data->act_col_num);
        // don't check for "possible" because shift=1 possible everywhere
        // orherwise, there is a bug
        Z_compare compare = compare_Z_dist(init_z_dist, zeros_dist, input_data->str_num);
        Move this_move;
        this_move.pat_id = p_num;
        this_move.size = 1;
        compare.assign_to_move = i_moves_count;
        compare.assign_to_pat = p_num;

        init_compares[i_moves_count] = compare;
        initial_moves[i_moves_count] = this_move;
        ++i_moves_count;
        // free allocated stuff, return mask to status-quo
        render__free_render(move_render, input_data->str_num);
        move_mask[p_num] = 0;
        free(zeros_dist);
    }


    // we have initial moves
    qsort(init_compares, input_data->dir_pat_num, sizeof(Z_compare), compare_Z_compares);
    uint32_t cutoff = 0;

    if (init_compares[0].min_zeros_delta != -1)
    {
        // this is ok, out best move is -1
        
    }
    else
    {
        // no way
        free(move_mask);
        free(initial_moves);
        free(init_compares);
        for (uint32_t i = 0; i < states_num; ++i){
            free(states[i].pat_mask);
            free(states[i].moves);
        }
        free(states);
        verbose("# Cannot make the first move\n");
        return false;
    }

    for (uint32_t i = 1; i < input_data->dir_pat_num; ++i){
        __print_Z_compare(&init_compares[i]);
        int dir = compare_Z_compares(&init_compares[i - 1], &init_compares[i]);
        printf("%d \n", dir);
    }

    free(move_mask);
    free(initial_moves);
    free(init_compares);

    for (uint32_t i = 0; i < states_num; ++i){
        free(states[i].pat_mask);
        free(states[i].moves);
    }
    free(states);
    verbose("# Freed traverse data\n");
    return res;
}
