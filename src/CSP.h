/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/
#ifndef CSP_H
#define CSP_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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

bool solve_CSP(uint32_t str_num, uint32_t str_len, uint32_t k_, uint32_t pat_num, uint32_t pos_num,
              uint8_t *patterns_1D_arr, uint32_t *pat_to_pos_1D, uint32_t *pat_to_pos_num,
              uint32_t *pat_to_pos_starts, uint32_t *pos_1D_arr, uint32_t *pos_to_pat);

#endif // !CSP_H 
