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

#define CHUNK 5
#define ALLOC_INIT 5
Num_q *num_count;
uint32_t uniq_num;
uint32_t *f_max;
uint32_t *f_min;
uint32_t *f_max_acc;
uint32_t *f_min_acc;

// comparing function for qsort
int cmpfunc (const void * a, const void * b)
{
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


// check if current value still can be used
uint32_t check_current(Num_q *path, uint32_t cur_ind)
{
    uint32_t used = path[cur_ind].quantity;
    uint32_t available = num_count[cur_ind].quantity;
    assert(available >= used);
    if (available == used)
    {
        // we cannot use this elem -> this is over
        // if 0 -> all are over, so there is no way
        cur_ind++;
        return cur_ind;
    }
    // we still have this number
    return cur_ind;
}


// create empty counter
Num_q *_get_zero_num_q()
{
    Num_q *res = (Num_q*)malloc(uniq_num * sizeof(Num_q));
    for (uint32_t i = 0; i < uniq_num; i++)
    {
        res[i].number = num_count[i].number;
        res[i].quantity = 0;
    }
    return res;
}


// compute array sum
uint32_t arr_sum(uint32_t *arr, uint32_t up_to)
{
    if (up_to == 0){return 0;}
    uint32_t res = 0;
    for (uint32_t i = 0; i < up_to; i++){res += arr[i];}
    return res;
}


// add elements to the counter
void add_to_zero_counter(Num_q *counter, uint32_t *arr, uint32_t arr_size)
{
    uint32_t counter_ind = 0;
    uint32_t arr_ind = 0;
    while (arr_ind < arr_size){
    // both are sorted
        assert(arr[arr_ind] <= counter[counter_ind].number);
        if (arr[arr_ind] == counter[counter_ind].number){
            counter[counter_ind].quantity++;
            arr_ind++;
        } else if (arr[arr_ind] < counter[counter_ind].number){
            counter_ind++;
        }
    }
}


// binary search
uint32_t _elem_search(int64_t l, int64_t r, uint32_t w)
{
    if (r >= l)
    {
        __int128_t mid = l + (r - l) / 2;
        if (num_count[mid].number == w){
            return mid;
        } else if (num_count[mid].number < w){
            return _elem_search(l, mid - 1, w);
        } else {
            return _elem_search(mid + 1, r, w);
        }
    }
    return uniq_num;
}


// get path for the subset size given
uint32_t *get_path(uint32_t sub_size, uint32_t *prev_path, uint32_t prev_p_len,
                   uint32_t _cur_val, uint32_t _cur_ind, uint32_t req_sum,
                   uint32_t arr_size)
{
    // initiate values
    uint32_t *path = (uint32_t*)calloc(sub_size + CHUNK, sizeof(uint32_t));
    Num_q *path_count = _get_zero_num_q();
    uint32_t pos_left;  // don't change this
    uint32_t _l_f_arr_size = arr_size;  // to keep local
    uint32_t path_len = prev_p_len;
    uint32_t cur_val = _cur_val;
    uint32_t cur_ind = _cur_ind;
    uint32_t inter_val = 0;

    if (prev_path != NULL){
        // add existing part to answer
        for (uint32_t i = 0; i < prev_p_len; i++){path[i] = prev_path[i];}
        pos_left = sub_size - prev_p_len;
        // add exitsing values to counter
        add_to_zero_counter(path_count, prev_path, prev_p_len);
    } else {pos_left = sub_size;}

    uint32_t *_f_max = (uint32_t*)malloc(_l_f_arr_size * sizeof(uint32_t));
    for (uint32_t i = 0; i < _l_f_arr_size; i++){_f_max[i] = f_max[i];}

    // values I need insude
    uint32_t prev_sum = 0;
    int64_t delta = 0;  // might be negative!
    uint32_t sup = 0;
    uint32_t inf = 0;
    uint32_t points_left = 0;
    
    // uint32_t delta_ind;
    // uint32_t delta_spent;
    // uint32_t delta_avail;

    // the main loop, trying to add the next element
    for (uint32_t i = 0; i < pos_left; i++)
    {   
        bool passed = false;
        prev_sum = arr_sum(path, path_len);
        cur_ind = check_current(path_count, cur_ind);
        cur_val = num_count[cur_ind].number;

        while (!passed)
        {
            // no values left
            if (cur_val == 0){
                free(_f_max);
                return path;
            }
            // get intermediate values
            inter_val = prev_sum + cur_val;
            delta = (int64_t)req_sum - inter_val;
            points_left = pos_left - (i + 1);
            sup = f_max_acc[points_left];
            inf = f_min_acc[points_left];
            // uint32_t to_inf = prev_sum - inf;
            // uint32_t to_inf_ind = _elem_search(0, (int64_t)uniq_num, to_inf);
            // if (to_inf_ind != uniq_num)
            // we can fall down to fmin and that's it
            // {
            //     uint32_t to_inf_spent = path_count[to_inf_ind].quantity;
            //     uint32_t to_inf_avail= num_count[to_inf_ind].quantity;
            //     // printf("to_inf: %llu ind: %llu \n", to_inf, to_inf_ind);
            //     // printf("Used: %llu available: %llu\n", to_inf_spent, to_inf_avail);
            //     if (to_inf_avail > to_inf_spent){printf("TODO THIS PART DINF EXISTS\n");}
            // }
            // need to be carefull -> comparing signed vs unsigned
            if ((delta > 0) && (uint32_t)delta > sup){
                // unreachable
                break;
            } else if ((delta < 0) || (uint32_t)delta < inf){
                // printf("Delta %lld inf %llu\n", delta, inf);
                cur_ind++;
                cur_val = num_count[cur_ind].number;
                continue;
            }
            // ok, value passed, let's add it
            passed = true;
            path[path_len] = cur_val;
            path_count[cur_ind].quantity++;
            path_len++;
            
            // not applicable in this task:
            // delta_ind = _elem_search(0, (int64_t)uniq_num, (uint32_t)delta);
            // delta_spent = path_count[delta_ind].quantity;
            // delta_avail = num_count[delta_ind].quantity;
            // if 0 > not found actually
            // but I don't like conglomeration of if's
            // if ((delta_avail > 0) && (delta_spent < delta_avail))
            // // yes, it is available
            // {
            //     verbose("Delta %lld is in the dataset\n", delta);
            //     path[path_len] = delta;
            //     return path;
            // }
        }
    }
    free(_f_max);
    free(path_count);
    return path;
}


// make grid
Point *make_grid(Pattern *patterns, uint32_t pat_num, uint32_t max_comb_size, uint32_t level_size)
{
    Point *grid = (Point*)malloc((max_comb_size - 1) * sizeof(Point));
    f_max = (uint32_t*)malloc(pat_num * sizeof(uint32_t));
    f_min = (uint32_t*)malloc(pat_num * sizeof(uint32_t));
    f_min[0] = 0;
    f_max[0] = 0;
    for (uint32_t i = 1; i < pat_num; ++i){f_min[i] = patterns[i].size;}
    qsort(f_min, pat_num, sizeof(uint32_t), cmpfunc);
    for (uint32_t i = 1; i < pat_num; ++i){f_max[i] = f_min[pat_num - i];}
    f_max_acc = accumulate_sum(f_max, pat_num);
    f_min_acc = accumulate_sum(f_min, pat_num);

    uniq_num = 0;
    num_count = count_elements(f_max, pat_num, &uniq_num);
    verbose("# Level size: %u\n", level_size);
    verbose("# %u unique sizes in the dataset\n", uniq_num);
    uint32_t point_id = 0;
    uint32_t cur_ind = 0;
    uint32_t cur_val = 0;
    uint32_t alloc_times = 0;
    uint32_t cmb_ptr = 0;

    for (uint32_t subset_size = 2; subset_size < (max_comb_size + 1); ++subset_size)
    {
        point_id = subset_size - 2;
        grid[point_id].lvl = 1;
        grid[point_id].density = (double)1 / subset_size;
        grid[point_id].comb_size = subset_size;
        grid[point_id].comb_num = 0;
        alloc_times = 0;

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
            continue;
        }
        // subset size > 2
        // readlly need to solve subset sum problem
        cur_ind = 0;
        cur_val = num_count[cur_ind].number;
        uint32_t *first_path = get_path(subset_size, NULL, 0, cur_val, cur_ind, level_size, pat_num);
        uint32_t path_sum = arr_sum(first_path, subset_size);
        if (path_sum != level_size){
            free(first_path);
            grid[point_id].comb_num = 0;
            grid[point_id].combinations = NULL;
            continue;
        }
        grid[point_id].comb_num = 1;
        alloc_times = ALLOC_INIT;
        grid[point_id].combinations = (uint32_t*)calloc(subset_size * alloc_times, sizeof(uint32_t));
        for (uint32_t i = 0; i < subset_size; ++i){grid[point_id].combinations[i] = first_path[i];}
        cmb_ptr = subset_size;

        uint32_t p = 0;
        uint32_t pointed;
        uint32_t pointed_ind;
        uint32_t lower;
        uint32_t lower_ind;
        bool possible;
        // find extra paths
        for (uint64_t p_ = (subset_size - 1); p_ > 0; p_--)
        {
            p = p_ - 1;  // if >= then it is an infinite loop
            pointed = first_path[p];
            if (pointed == 0) {continue;}
            pointed_ind = _elem_search(0, (int64_t)uniq_num, pointed);
            possible = true;
            while (possible)
            // decrease while decreseable
            {
                lower_ind = pointed_ind + 1;
                lower = num_count[lower_ind].number;
                if (lower == 0) {possible = false; break;}
                if (lower == 0){possible = false; break;}
                uint32_t *try_path = (uint32_t*)calloc(subset_size + CHUNK, sizeof(uint32_t));
                for (uint32_t i = 0; i < p; i++){try_path[i] = first_path[i];}
                try_path[p] = lower;
                uint32_t *try_res = get_path(subset_size, try_path, p + 1, lower, lower_ind, 
                                             level_size, pat_num);
                pointed = lower;
                pointed_ind = lower_ind;
                // do not forget!
                free(try_path);
                uint64_t try_res_sum = arr_sum(try_res, subset_size);
                if (try_res_sum == level_size){
                    for (uint32_t i = 0; i < subset_size; ++i){
                        grid[point_id].combinations[cmb_ptr + i] = try_res[i];
                    }
                    grid[point_id].comb_num += 1;
                    cmb_ptr += subset_size;
                }
                if (grid[point_id].comb_num >= alloc_times - 1){
                    alloc_times += ALLOC_INIT;
                    grid[point_id].combinations = (uint32_t*)realloc(grid[point_id].combinations,
                                                                     alloc_times * subset_size * sizeof(uint32_t));
                }
                free(try_res);
            }
        }
        free(first_path);
    }

    free(num_count);
    free(f_max);
    free(f_min);
    free(f_max_acc);
    free(f_min_acc);
    return grid;
}
