/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/
#ifndef PATTERNS_H
#define PATTERNS_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


struct pattern
{
    uint8_t *pattern_seq;
    uint32_t *occupies;
    uint32_t occupies_num;   // equal to size
    uint32_t *intersects_with;
    uint32_t inters_num;
    uint32_t * not_intersects_with;
    uint32_t non_inters_num;
    uint32_t *positions;
    uint32_t pattern_rev;
    uint32_t id;
};


struct position
{
    uint32_t number;
    uint32_t patterns[2];
};


struct pat_list_search_elem
{
    uint32_t pat_num;
    uint32_t pat_id;
};

typedef struct pattern Pattern;
typedef struct position Position;
typedef struct pat_list_search_elem Pat_list_search_elem;

uint64_t *get_max_pat_num(uint32_t lvl_size);

uint64_t C_n_k(uint32_t n, uint32_t k);

uint32_t pat_to_num(uint32_t pattern_len, uint8_t * pattern_seq);

uint32_t sum_pattern(uint32_t pattern_len, uint8_t * pattern_seq); 

int comp_search_elems(const void *a, const void *b);

bool intersect_by_occ(uint32_t *occupies_1, uint32_t occ_num_1,
                      uint32_t *occupies_2, uint32_t occ_num_2);

uint32_t pattern_seq_to_id(Pat_list_search_elem *pat_search_list, int l, int r, uint32_t x);

#endif // !PATTERNS_H
