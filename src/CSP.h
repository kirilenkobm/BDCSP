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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool solve_CSP(uint32_t str_num, uint32_t str_len, uint32_t k_, uint32_t pat_num, uint32_t pos_num,
              uint8_t *patterns_1D_arr, uint32_t *pat_to_pos_1D, uint32_t *pat_to_pos_num,
              uint32_t *pat_to_pos_starts, uint32_t *pos_1D_arr, uint32_t *pos_to_pat);

#endif // !CSP_H 
