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
#include <stdbool.h>
#include <stdlib.h>
#include "patterns.h"


uint32_t pat_to_num(uint32_t pattern_len, uint8_t * pattern_seq)
// convert pattern sequence to a characteristic number
{
    int kt = 0;
    uint32_t answer = 0;
    for (int i = pattern_len - 1, j = 1; i >= 0; i--, j++){
        kt = j * 2;
        answer += pattern_seq[i] * kt;
    }
    return answer;
}


uint32_t sum_pattern(uint32_t pattern_len, uint8_t * pattern_seq)
// for a pattern sequence just compute a sum
{
    int sum = 0;
    uint8_t *ptr = pattern_seq;
    while (ptr < &pattern_seq[pattern_len])
    {
        sum += *ptr;
        ptr++;
    }
    return sum;
}

int comp_search_elems(const void *a, const void *b)
// function to compare search elems in qsort
{ 
    Pat_list_search_elem *ia = (Pat_list_search_elem *)a;
    Pat_list_search_elem *ib = (Pat_list_search_elem *)b;
    return ia->pat_num - ib->pat_num;
}


bool intersect_by_occ(uint32_t *occupies_1, uint32_t occ_num_1,
                      uint32_t *occupies_2, uint32_t occ_num_2)
// return true if patterns intersect, false otherwise
{   
    if ((occupies_1[occ_num_1] < occupies_2[0]) || (occupies_1[0] > occupies_2[occ_num_2]))
    {
        return false;
    }
    for (uint32_t i = 0; i < occ_num_1; i++){
        // if smaller that the smallest element in other arr then continue
        if (occupies_1[i] < occupies_2[0]){continue;}
        else if (occupies_1[i] > occupies_2[occ_num_2 - 1]){return false;}
        for (uint32_t j = 0; j <= occ_num_2; j++){
            if (occupies_1[i] == occupies_2[j]){
                return true;
            }
        }
    }
    return false;
}


uint32_t pattern_seq_to_id(Pat_list_search_elem *pat_search_list, int l, int r, uint32_t x)
// binary search, find pattern id by characteristic number of a pattern
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

uint64_t C_n_k(uint32_t n, uint32_t k)
// compute combinations
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    uint64_t result = n;
    for(uint32_t i = 2; i <= k; ++i)
    {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}


uint64_t *get_max_pat_num(uint32_t lvl_size)
// compute maximal number of patterns for this lvl size
{
    uint64_t *comb_size_num = (uint64_t*)calloc(lvl_size, sizeof(uint64_t));
    for (uint32_t k = 1; k < lvl_size; k++)
    // how many times can I select k size patterns with a given lvl size?
    {
        // n! / k!(n - k)!
        uint64_t pat_num_at_lvl = C_n_k(lvl_size, k);
        comb_size_num[k] = pat_num_at_lvl;

    }
    return comb_size_num;
}
