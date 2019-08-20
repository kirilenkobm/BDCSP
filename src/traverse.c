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


typedef struct{
    uint32_t *zero_mask;
    uint32_t *full_mask;
    uint32_t mask_size;
} Masks_data;


extern bool v;


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
void __print_move(Move *move)
{
    if (!v) {return;}
    printf("# Move: PAT_ID %u SIZE %d\n", move->pat_id, move->size);
}


// just print a mask
void __print_mask(uint32_t *mask, uint32_t mask_size){
    if (!v){return;}
    for (uint32_t i = 0; i < mask_size; ++i){printf("%u ", mask[i]);}
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
    res.min_zeros = after_max;
    // printf("Min delta: %d\n", res.min_zeros_delta);
    for (uint32_t i = 0; i < len; ++i){
        if (before[i] == after[i]){continue;}
        else if (before[i] > after[i]){res.minus_ += (before[i] - after[i]);}
        else {res.plus_ += (after[i] - before[i]);}
    }
    // printf("Minuses: %u pluses: %u\n", res.minus_, res.plus_);
    // N
    return res;
}


// copy moves array
Move *__copy_moves(Move *arr, uint32_t len)
{
    Move *res = (Move*)malloc(len * sizeof(Move));
    for (uint32_t i = 0; i < len; ++i){
        res[i].pat_id = arr[i].pat_id;
        res[i].size = arr[i].size;
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


// get the next state and moves
void __upd_prog_state
(State *states, Masks_data *mask, uint32_t *cur_state, bool *end, 
bool *res, Input_data *input_data, Pattern *patterns)
{   
    if (states[*cur_state].moves_num == states[*cur_state].cur_move)
    // in this case we tried all possible paths for this state
    {
        __wipe_state(&states[*cur_state]);
        if (*cur_state == 0){*end = true;}  // no states < 0, break the loop
        else {*cur_state = *cur_state - 1;}  // goto previous one
        return;  // nothing to do in this function anymore
    }
    // ok, we have a chance to do something
    // get the current move and try to evolve
    Move cur_move = states[*cur_state].moves[states[*cur_state].cur_move];
    ++states[*cur_state].cur_move;

    uint32_t *cur_mask = traverse__mask_copy(states[*cur_state].pat_mask, mask->mask_size);
    __apply_move(cur_mask, &cur_move);
    *cur_state += 1;  // goto next step then

    // actually we need current render data
    uint8_t **init_render = render__draw(patterns, cur_mask, input_data);
    uint32_t *init_z_dist = render__get_zeros(init_render,
                                              input_data->str_num, 
                                              input_data->act_col_num);
    render__free_render(init_render, input_data->str_num);
    __print_mask(cur_mask, mask->mask_size);
    // render__show_arr(init_render, input_data->str_num, input_data->act_col_num);

    // need to find next good moves
    // partly copy-paste of the init function
    Move *next_moves = (Move*)malloc(sizeof(Move) * input_data->dir_pat_num * 2);
    Z_compare* next_compares = (Z_compare*)malloc(sizeof(Z_compare) * input_data->dir_pat_num * 2);
    uint32_t moves_count = 0;

    for (uint32_t p_num = 1; p_num < mask->mask_size; ++p_num)
    {
        // FOR PLUS ONE
        if (cur_mask[p_num] + 1 <= mask->full_mask[p_num])
        // we can add +1 move then
        {
            cur_mask[p_num] += 1;
            uint8_t **move_render = render__draw(patterns, cur_mask, input_data);
            uint32_t *zeros_dist = render__get_zeros(move_render,
                                                     input_data->str_num, 
                                                     input_data->act_col_num);
            Z_compare compare = compare_Z_dist(init_z_dist, zeros_dist, input_data->str_num);
            // write data
            Move this_move;
            this_move.pat_id = p_num;
            this_move.size = 1;
            compare.assign_to_move = moves_count;
            compare.assign_to_pat = p_num;
            next_compares[moves_count] = compare;
            next_moves[moves_count] = this_move;
            ++moves_count;
            // return status-quo
            cur_mask[p_num] -= 1;
            render__free_render(move_render, input_data->str_num);
            free(zeros_dist);
        }
        // FOR MINUS ONE
        if (cur_mask[p_num] > 0)
        // if we are here -> we can add -1 move
        {
            cur_mask[p_num] -= 1;
            uint8_t **move_render = render__draw(patterns, cur_mask, input_data);
            uint32_t *zeros_dist = render__get_zeros(move_render,
                                                     input_data->str_num, 
                                                     input_data->act_col_num);
            Z_compare compare = compare_Z_dist(init_z_dist, zeros_dist, input_data->str_num);
            // write data again
            // TODO: make it a single function, not two repeats of the same block of code!
            Move this_move;
            this_move.pat_id = p_num;
            this_move.size = -1;
            compare.assign_to_move = moves_count;
            compare.assign_to_pat = p_num;
            next_compares[moves_count] = compare;
            next_moves[moves_count] = this_move;
            ++moves_count;
            // return status quo again
            cur_mask[p_num] += 1;
            render__free_render(move_render, input_data->str_num);
            free(zeros_dist);
        }
    }
    // ok, added new moves
    qsort(next_compares, input_data->dir_pat_num, sizeof(Z_compare), compare_Z_compares);
    uint32_t cutoff = 0;
    // check whether we reached answer already
    uint32_t ones_cov = input_data->act_col_num - next_compares[0].min_zeros;
    verbose("# Ones coverage: %u; Zeros: %u\n", ones_cov, next_compares[0].min_zeros);
    bool already_ans_true = (ones_cov >= input_data->to_cover);
    if (already_ans_true){
        // we have an answer -> so break execution
        verbose("# Answer branch 5\n");
        free(next_moves);
        free(init_z_dist);
        free(next_compares);
        *end = true;
        *res = true;
        return;
    }
    
    // get and apply cutoff
    bool found = false;
    for (uint32_t i = 1; i < input_data->dir_pat_num; ++i){
        if (compare_Z_compares(&next_compares[i - 1], &next_compares[i]) == -1){
            cutoff = i - 1;
            found = true;
            break;
        }
    }
    if (!found){cutoff = (input_data->dir_pat_num - 1);}
    ++cutoff;

    next_compares = (Z_compare*)realloc(next_compares, cutoff * sizeof(Z_compare));
    qsort(next_compares, cutoff, sizeof(Z_compare), Z_comp_order);

    uint32_t move_id;
    // TODO: re-do filter, pat < prev_pat
    Move *filt_moves = (Move*)malloc(cutoff * sizeof(Move));
    for (uint32_t i = 0; i < cutoff; ++i){
        move_id = next_compares[i].assign_to_move;
        filt_moves[i].pat_id = next_moves[move_id].pat_id;
        filt_moves[i].size = next_moves[move_id].size;
    }

    // finally create initial state
    states[*cur_state].pat_mask = cur_mask;
    states[*cur_state].moves = __copy_moves(filt_moves, cutoff);
    states[*cur_state].moves_num = cutoff;
    states[*cur_state].prev_pat = cur_move.pat_id;
    states[*cur_state].cur_move = 0;

    free(filt_moves);
    free(next_moves);
    free(init_z_dist);
    free(next_compares);
}


// call in-depth search for answer
// initiate the first program state
// and then run state updater to get the answer
bool traverse__run
(uint32_t *zero_mask, uint32_t *full_mask, uint32_t *init_z_dist,
Input_data *input_data, Pattern *patterns)
{
    bool res = false;  // default answer
    uint32_t states_num = input_data->act_col_num * 4;
    verbose("# Search depth %u\n", states_num);
    State *states = (State*)malloc(states_num * sizeof(State));
    uint32_t mask_size = (input_data->dir_pat_num + 1);

    // initiate states with default values
    for (uint32_t i = 1; i < states_num; ++i)
    {
        // states[i].pat_mask = (uint32_t*)calloc(mask_size, sizeof(uint32_t));
        // states[i].moves = (Move*)malloc(MOVES_STEP * sizeof(Move));
        states[i].pat_mask = NULL;
        states[i].moves = NULL;
        states[i].moves_num = 0;
        states[i].cur_move = 0;
        states[i].prev_pat = 0;
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
        uint8_t **move_render = render__draw(patterns, zero_mask, input_data);
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
        zero_mask[p_num] = 0;
        free(zeros_dist);
    }

    // we have initial moves
    qsort(init_compares, input_data->dir_pat_num, sizeof(Z_compare), compare_Z_compares);
    uint32_t cutoff = 0;
    int dir;
    bool found = false;

    // print sorted compares
    // for (uint32_t i = 0; i < input_data->dir_pat_num; ++i){__print_Z_compare(&init_compares[i]);}
    uint32_t ones_cov = input_data->act_col_num - init_compares[0].min_zeros;
    bool already_ans_true = (ones_cov >= input_data->to_cover);
    bool already_ans_false = (init_compares[0].min_zeros_delta == 1);



    assert(!(already_ans_false && already_ans_true));  // should never happen that both are true

    if (!already_ans_true && !already_ans_false)
    {
        // this is ok, out best move is -1
        for (uint32_t i = 1; i < input_data->dir_pat_num; ++i){
            dir = compare_Z_compares(&init_compares[i - 1], &init_compares[i]);
            if (dir == -1){
                cutoff = i - 1;
                found = true;
                break;
            }
        }
        // didn't break --> all are the same (hell)
        if (!found){cutoff = (input_data->dir_pat_num - 1);}
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
    ++cutoff;  // for different loops
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
    states[0].moves = __copy_moves(filt_moves, cutoff);
    for (uint32_t i = 0; i < cutoff; ++i){__print_move(&filt_moves[i]);}
    free(filt_moves);
    states[0].moves_num = cutoff;
    states[0].cur_move = 0;

    for (uint32_t i = 0; i < cutoff; ++i){__print_move(&filt_moves[i]);}
    
    uint32_t cur_state = 0;
    bool end = false;
    Masks_data masks;
    masks.zero_mask = zero_mask;
    masks.full_mask = full_mask;
    masks.mask_size = mask_size;

    // main loop
    for (uint32_t step = 0; step < (states_num - 1); ++step)
    {
        verbose("# Step num %u\n", step);
        __upd_prog_state(states, &masks, &cur_state, &end, &res, input_data, patterns);
        if (end){break;}  // interrupt if something is bad
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
