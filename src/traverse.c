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
//     uint32_t min_zeros_delta;
//     uint32_t minus_;
//     uint32_t plus_;
// } Z_compare;


// create array copy
uint32_t *traverse__mask_copy(uint32_t *mask, uint32_t len)
{
    uint32_t *ans = (uint32_t*)calloc(len, sizeof(uint32_t));
    for (uint32_t i = 0; i < len; ++i){ans[i] = mask[i];}
    return ans;
}


// compare two zero-distr outputs
Z_compare compare_Z_dist(uint32_t *before, uint32_t *after, uint32_t len)
{
    Z_compare res;
    res.min_zeros = 0;
    res.min_zeros_delta = 0;
    res.minus_ = 0;
    res.plus_ = 0;
    return res;
}


// call in-depth search for answer
bool traverse__run
(uint32_t *zero_mask, uint32_t *full_mask, uint32_t *init_z_dist, Input_data *input_data, Pattern *patterns)
{
    bool res = false;  // default answer
    uint32_t states_num = input_data->act_col_num * 4;
    State *states = (State*)malloc(states_num * sizeof(State*));
    uint32_t mask_size = (input_data->dir_pat_num + 1);

    // initiate states with default values
    for (uint32_t i = 0; i < states_num; ++i){
        states[i].pat_mask = (uint32_t*)calloc(mask_size, sizeof(uint32_t));
        states[i].moves = NULL;
        states[i].moves_num = 0;
        states[i].cur_move = 0;
    }

    states[0].pat_mask = traverse__mask_copy(zero_mask, mask_size);

    // initiate moves, only + moves available
    Move *initial_moves = (Move*)malloc(sizeof(Move) * input_data->dir_pat_num);
    uint32_t i_moves_count = 0;

    uint32_t *move_mask = traverse__mask_copy(zero_mask, mask_size);

    for (uint32_t p_num = 1; p_num < input_data->dir_pat_num; ++p_num){
        uint32_t *move_mask = traverse__mask_copy(zero_mask, mask_size);
        move_mask[p_num] = 1;
        uint8_t **move_render = render__draw(patterns, move_mask, input_data);
        uint32_t *zeros_dist = render__get_zeros(move_render,
                                                 input_data->str_num, 
                                                 input_data->act_col_num);
        // don't check for "possible" because shift=1 possible everywhere
        // orherwise, there is a bug
        Z_compare compare = compare_Z_dist(init_z_dist, zero_mask, input_data->str_num);
        // free allocated stuff, return mask to status-quo
        render__free_render(move_render, input_data->str_num);
        move_mask[p_num] = 0;
        free(zeros_dist);
    }

    free(move_mask);
    free(initial_moves);
    for (uint32_t i = 0; i < states_num; ++i){
        free(states[i].pat_mask);
        free(states[i].moves);
    }
    free(states);
    verbose("# Freed traverse data\n");
    return res;
}
