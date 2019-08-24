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
#include <limits.h>
#ifdef _WIN32
#include <io.h>
#define F_OK 0
#elif __unix__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif
#include "CSP.h"
#include "patterns.h"
#include "render.h"
#include "traverse.h"
#include "arrstuff.h"
#define MOVES_STEP 10

#define _MOVES_MULT 1
extern bool v;


typedef struct{
    uint32_t **arrays;
    uint32_t arrs_num;
} Mask_memory;

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


// just compare two z comps by id
int Z_comp_order(const void *a, const void *b)
{
    return (((Z_compare*)a)->assign_to_pat - ((Z_compare*)b)->assign_to_pat);
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
    printf("Assigned to pattern: %u\n", z_comp->assign_to_pat);
    printf("Assigned to move: %u\n", z_comp->assign_to_move);
    printf("\n");
}


// just print a move
void __print_move(Move *move){printf("# Move: PAT_ID %u SIZE %d\n", move->pat_id, move->size);}


// compare two zero-distr outputs
Z_compare compare_Z_dist(uint32_t *before, uint32_t *after, uint32_t len)
{
    Z_compare res;
    res.min_zeros = 0;
    res.min_zeros_delta = 0;
    res.minus_ = 0;
    res.plus_ = 0;
    uint32_t before_max =  arr_max(before, len);
    uint32_t after_max = arr_max(after, len);
    res.min_zeros_delta = after_max - before_max;
    res.min_zeros = after_max;
    // printf("Min delta: %d\n", res.min_zeros_delta);
    for (uint32_t i = 0; i < len; ++i){
        if (before[i] == after[i]){continue;}
        else if (before[i] > after[i]){res.minus_ += (before[i] - after[i]);}
        else {res.plus_ += (after[i] - before[i]);}
    }
    return res;
}


// wipe state, return 0 - state
void __wipe_state(State *state)
{   
    free(state->pat_mask);
    free(state->moves);
    state->pat_mask = NULL;
    state->moves = NULL;
    state->prev_pat = 0;
    state->moves_num = 0;
    state->cur_move = 0;
}


// apply move to mask
void __apply_move(uint32_t *mask, Move *move) {mask[move->pat_id] += move->size;}


// check that masks are the same or not


// check whether we've already been there
bool __is_in_mask(uint32_t *mask, uint32_t mask_size, State *states, uint32_t states_num)
{
    bool are_same = false;
    for (uint32_t i = 0; i < states_num; ++i)
    {
        are_same = arr_uint32_are_the_same(states[i].pat_mask, mask, mask_size);
        if (are_same){return true;}
    }
    return false;
}


// check if array is in memory
bool __is_in_memory(Mask_memory *mask_memory, uint32_t *mask, uint32_t mask_size)
{
    if (mask_memory->arrs_num == 0) {return false;}
    bool are_same = false;
    for (uint32_t i = 0; i < mask_memory->arrs_num; ++i){
        are_same = arr_uint32_are_the_same(mask_memory->arrays[i], mask, mask_size);
        if (are_same) {return true;}
    }
    return false;
}


// add mask to memory
void __add_to_memory(Mask_memory *mask_memory, uint32_t *mask, uint32_t mask_size)
{
    uint32_t current_num = mask_memory->arrs_num;
    mask_memory->arrays[current_num] = arr_1D_uint32_copy(mask, mask_size);
    mask_memory->arrs_num += 1;
}


// count zeros if the mask given applied
uint32_t *traverse_get_z_dist
(Pattern *patterns, uint32_t dist_size, uint32_t *cur_mask, uint32_t mask_size, uint32_t pat_num)
{
    uint32_t *res = (uint32_t*)calloc(dist_size, sizeof(uint32_t));
    uint32_t rev_pat_id = 0;
    uint32_t rev_time = 0;
    uint32_t dir_time = 0;

    // mask item -> one pattern X times
    for (uint32_t pat_id = 1; pat_id < mask_size; ++pat_id)
    {
        rev_pat_id = pat_num - pat_id;
        rev_time = cur_mask[pat_id];
        dir_time = patterns[pat_id].times - rev_time;
        // iter over pattern; str_num
        for (uint32_t i = 0; i < dist_size; ++i){
            // use a trick, if I need to + number of zeros, number of zeros
            // is pat with 1 (direct_pattern) * rev_time, which is zero
            // I * dir_time and get 0
            res[i] += ((patterns[pat_id].pattern[i] * rev_time)
                     + (patterns[rev_pat_id].pattern[i] * dir_time));
        }
    }
    return res;
}


// update program state
void __upd_prog_state
(State *states, Masks_data *mask, uint32_t *cur_state, bool *end, bool *res,
Input_data *input_data, Pattern *patterns, uint32_t *m_c_f, Mask_memory *mask_memory)
{
    // in this case we tried all possible paths for this state
    if (states[*cur_state].moves_num == states[*cur_state].cur_move)
    {
        verbose("****GO BACK! Were %u moves depth %u\n",
                states[*cur_state].moves_num, *cur_state);
        __wipe_state(&states[*cur_state]);
        if (*cur_state == 0){*end = true;}  // no states < 0, break the loop
        else {*cur_state = *cur_state - 1;}  // goto previous one
        return;  // nothing to do in this function anymore
    }

    // ok, try next moves
    uint32_t cur_move_num = states[*cur_state].cur_move;
    Move cur_move = states[*cur_state].moves[cur_move_num];
    states[*cur_state].cur_move += 1;  // keep in memory + move
    uint32_t *cur_mask = traverse__mask_copy(states[*cur_state].pat_mask, mask->mask_size);
    __apply_move(cur_mask, &cur_move);

    // bool already_is_in = __is_in_mask(cur_mask, mask->mask_size, states, *cur_state);
    bool already_is_in = __is_in_memory(mask_memory, cur_mask, mask->mask_size);
    if (already_is_in)
    {
        free(cur_mask);
        verbose("*****been there, go back\n");
        if (*cur_state == 0){*end = true;}  // no states < 0, break the loop
        else {*cur_state = *cur_state - 1;}  // goto previous one
        return;  // nothing to do in this function anymore
    } else {
        __add_to_memory(mask_memory, cur_mask, mask->mask_size);
    }

    uint32_t *init_z_dist = traverse_get_z_dist(patterns,
                                                input_data->str_num,
                                                cur_mask,
                                                mask->mask_size,
                                                input_data->pat_num);
    if (v) {arr_1D_uint32_print(cur_mask, mask->mask_size);}
    
    // initiate next compares and initiate actual number
    uint32_t allocated_ =  input_data->dir_pat_num * 2;
    Z_compare* next_compares = (Z_compare*)malloc(sizeof(Z_compare) * allocated_);
    Move *next_moves = (Move*)malloc(sizeof(Move) * allocated_);

    uint32_t moves_count = 0;
    uint32_t p_num = 0;
    int8_t change = 0;
    bool is_in_states = false;
    bool is_in_memory = false;

    for (uint32_t iter = 2; iter < (mask->mask_size * 2); ++iter)
    {   
        // need to try both +1 and -1 for each pattern
        p_num = iter / 2;
        change = (iter - (p_num * 2)) ? -1 : 1;
        // ok, I think I need to block this feature
        if (change == -1) {continue;}  // I don't know, do I need -1 moves or not
        // this is why there are so many comments

        // cannot have value < 0 or > MAX
        if ((change == -1) && (cur_mask[p_num] == 0)){continue;}
        if (((change == 1) && cur_mask[p_num] == mask->full_mask[p_num])){continue;}
        cur_mask[p_num] += change;
        is_in_states = __is_in_mask(cur_mask, mask->mask_size, states, *cur_state);
        is_in_memory = __is_in_memory(mask_memory, cur_mask, mask->mask_size);
        if (is_in_states || is_in_memory){
            // repeats are not allowed
            verbose("***skip repeating step\n");
            cur_mask[p_num] -= change;
            continue;
        }

        uint32_t *zeros_dist = traverse_get_z_dist(patterns,
                                                   input_data->str_num,
                                                   cur_mask,
                                                   mask->mask_size,
                                                   input_data->pat_num);
        // compare distributions; free render we don't need anymore
        Z_compare compare = compare_Z_dist(init_z_dist, zeros_dist, input_data->str_num);

        // if move makes everything worse -> skip it
        if (compare.min_zeros_delta >= 0){
            free(zeros_dist);
            cur_mask[p_num] -= change;
            continue;
        }

        // define and fill this mmove
        Move this_move;
        this_move.pat_id = p_num;
        this_move.size = change;

        compare.assign_to_move = moves_count;
        compare.assign_to_pat = p_num;

        next_compares[moves_count] = compare;
        next_moves[moves_count] = this_move;

        ++moves_count;
        free(zeros_dist);
        // return mask back
        cur_mask[p_num] -= change;
    }
    verbose("# Possible %u moves out of %u allocated\n", moves_count, allocated_);
    free(init_z_dist);  // we don't need this array anymore
    *cur_state += 1;

    if (moves_count == 0){
        // no moves possible -> go back
        verbose("***Nothing found, go back\n");
        if (*cur_state == 0){*end = true;}
        else {*cur_state = *cur_state - 1;}  // goto previous one
        return;  // nothing to do in this function anymore
    } else if (moves_count < allocated_){
        allocated_ = moves_count;  // realloc arrays
        next_compares = (Z_compare*)realloc(next_compares, sizeof(Z_compare) * allocated_);
        next_moves = (Move*)realloc(next_moves, sizeof(Move) * allocated_);
    }
    // sort to get best ones, sort moves correspondingly
    qsort(next_compares, allocated_, sizeof(Z_compare), compare_Z_compares);
    Move *sort_moves = (Move*)malloc(allocated_ * sizeof(Move*));
    uint32_t move_id = 0;
    for (uint32_t i = 0; i < allocated_; ++i){
        move_id = next_compares[i].assign_to_move;
        sort_moves[i].pat_id = next_moves[move_id].pat_id;
        sort_moves[i].size = next_moves[move_id].size;
    }
    free(next_moves);

    // find out whether we have the answer already
    uint32_t ones_cov = input_data->act_col_num - next_compares[0].min_zeros;
    if (ones_cov > *m_c_f) {*m_c_f = ones_cov;}  // update max reached val
    verbose("# Ones coverage: %u; Zeros: %u\n", ones_cov, next_compares[0].min_zeros);
    bool already_ans_true = (ones_cov >= input_data->to_cover);
    if (already_ans_true){
        // we have an answer -> so break execution
        verbose("# Answer branch 5\n");
        *end = true;
        *res = true;
        return;
    }

    // fill the next state
    states[*cur_state].pat_mask = cur_mask;
    states[*cur_state].moves = sort_moves;
    states[*cur_state].moves_num = allocated_;
    states[*cur_state].prev_pat = cur_move.pat_id;
    states[*cur_state].cur_move = 0;
    states[*cur_state].prev_p_sign = cur_move.size;
    // free allocated stuff
}


// call in-depth search for answer
// initiate the first program state
// and then run state updater to get the answer
bool traverse__run
(uint32_t *zero_mask, uint32_t *full_mask, uint32_t *init_z_dist,
Input_data *input_data, Pattern *patterns)
{
    bool res = false;  // default answer
    uint32_t states_num = input_data->act_col_num * _MOVES_MULT;
    verbose("# Search depth %u\n", states_num);
    Mask_memory mask_memory;
    mask_memory.arrays = (uint32_t**)malloc((states_num + 1) * sizeof(uint32_t*));
    mask_memory.arrs_num = 0;
    State *states = (State*)malloc((states_num + 1) * sizeof(State));
    uint32_t mask_size = (input_data->dir_pat_num + 1);

    // initiate states with default values
    for (uint32_t i = 1; i < states_num; ++i)
    {
        states[i].pat_mask = NULL;
        states[i].moves = NULL;
        states[i].moves_num = 0;
        states[i].cur_move = 0;
        states[i].prev_pat = 0;
        states[i].prev_p_sign = zero;
    }

    states[0].pat_mask = traverse__mask_copy(zero_mask, mask_size);
    // states[0].moves = NULL;

    // initiate moves, only + moves available
    Move *initial_moves = (Move*)malloc(sizeof(Move) * input_data->dir_pat_num);
    Z_compare* init_compares = (Z_compare*)malloc(sizeof(Z_compare) * input_data->dir_pat_num);
    uint32_t i_moves_count = 0;


    for (uint32_t p_num = 1; p_num < mask_size; ++p_num)
    {
        zero_mask[p_num] = 1;
        uint32_t *zeros_dist = traverse_get_z_dist(patterns,
                                            input_data->str_num,
                                            zero_mask,
                                            mask_size,
                                            input_data->pat_num);
        // don't check for "possible" because shift=1 possible everywhere
        // orherwise, there is a bug
        Z_compare compare = compare_Z_dist(init_z_dist, zeros_dist, input_data->str_num);
        if (compare.min_zeros_delta >= 0){
            free(zeros_dist);
            zero_mask[p_num] = 0;
            continue;
        }

        Move this_move;
        this_move.pat_id = p_num;
        this_move.size = 1;
        compare.assign_to_move = i_moves_count;
        compare.assign_to_pat = p_num;

        init_compares[i_moves_count] = compare;
        initial_moves[i_moves_count] = this_move;
        ++i_moves_count;
        // free allocated stuff, return mask to status-quo
        zero_mask[p_num] = 0;
        free(zeros_dist);
    }
    verbose("# Found %u initial moves\n", i_moves_count);
    if (i_moves_count == 0){
        free(initial_moves);
        free(init_compares);
        for (uint32_t i = 0; i < states_num; ++i){
            free(states[i].pat_mask);
            free(states[i].moves);
        }
        free(states);
        verbose("# Answer branch 4\n");
        verbose("# Cannot find initial move\n");
        return false;
    }

    // we have initial moves
    init_compares = (Z_compare*)realloc(init_compares, i_moves_count * sizeof(Z_compare));
    initial_moves = (Move*)realloc(initial_moves, i_moves_count * sizeof(Move));
    qsort(init_compares, i_moves_count, sizeof(Z_compare), compare_Z_compares);
    // uint32_t cutoff = 0;
    // int dir;
    // bool found = false;

    uint32_t ones_cov = input_data->act_col_num - init_compares[0].min_zeros;
    bool already_ans_true = (ones_cov >= input_data->to_cover);
    bool already_ans_false = (init_compares[0].min_zeros_delta == 1);
    assert(!(already_ans_false && already_ans_true));  // should never happen that both are true
    uint32_t cutoff = i_moves_count;
    if (!already_ans_true && !already_ans_false)
    {
        for (uint32_t i = 0; i < i_moves_count; ++i){
            if (init_compares[i].min_zeros_delta >= 0){
                cutoff = i;
                break;
            }
        }
    }
    else
    {
        // we have an answer already
        free(initial_moves);
        free(init_compares);
        for (uint32_t i = 0; i < states_num; ++i){
            free(states[i].pat_mask);
            free(states[i].moves);
        }
        free(states);
        verbose("# Answer branch 4\n");
        if (already_ans_true){
            return true;
        } else {
            verbose("# Cannot make the first move\n");
            return false;
        }
    }
    // so now we have a cutoff and can write the initial array
    verbose("# Initial cutoff is: %u\n", cutoff);
    init_compares = (Z_compare*)realloc(init_compares, cutoff * sizeof(Z_compare));
    qsort(init_compares, cutoff, sizeof(Z_compare), Z_comp_order);

    // filt moves now
    Move *filt_moves = (Move*)malloc(cutoff * sizeof(Move));
    uint32_t move_id = 0;

    for (uint32_t i = 0; i < cutoff; ++i){
        move_id = init_compares[i].assign_to_move;
        filt_moves[i].pat_id = initial_moves[move_id].pat_id;
        filt_moves[i].size = initial_moves[move_id].size;
    }

    // finally create initial state
    states[0].moves = filt_moves;
    // if (v) {for (uint32_t i = 0; i < cutoff; ++i){__print_move(&filt_moves[i]);}}
    states[0].moves_num = cutoff;
    states[0].cur_move = 0;
    
    uint32_t cur_state = 0;
    bool end = false;
    Masks_data masks;
    masks.zero_mask = zero_mask;
    masks.full_mask = full_mask;
    masks.mask_size = mask_size;
    uint32_t max_cov_found = 0;

    // main loop
    verbose("# Started search loop.\n");
    for (uint32_t step = 0; step < states_num; ++step)
    {
        verbose("# Step num %u / %u\n", step, states_num - 1);
        __upd_prog_state(states, &masks, &cur_state, &end, &res, input_data,
                         patterns, &max_cov_found, &mask_memory);
        if (end){break;}  // interrupt if something is bad
    }

    if (!res){
        verbose("Maximal coverage found is %u / %u\n", max_cov_found, input_data->to_cover);
    }
    free(init_compares);
    free(initial_moves);
    for (uint32_t i = 0; i < states_num; ++i){
        free(states[i].pat_mask);
        free(states[i].moves);
    }

    free(states);
    verbose("# Freed traverse data\n");
    return res;
}
