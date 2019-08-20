// Bogdan Kirilenko
// 2019, Dresden
// Walking over hypercube
#ifndef TRAVERSE_H
#define TRAVERSE_H
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


typedef int8_t sign;
#define plus 1
#define zero 0
#define minus -1

typedef struct{
    uint32_t pat_id;
    int8_t size;
} Move;

typedef struct{
    uint32_t *pat_mask;
    Move *moves;
    uint32_t prev_pat;
    sign prev_p_sign;
    uint32_t moves_num;
    uint32_t cur_move;
} State;

typedef struct{
    uint32_t min_zeros;
    int8_t min_zeros_delta;
    uint32_t minus_;
    uint32_t plus_;
    uint32_t assign_to_move;
    uint32_t assign_to_pat;
} Z_compare;

typedef struct{
    uint32_t *zero_mask;
    uint32_t *full_mask;
    uint32_t mask_size;
} Masks_data;

bool traverse__run
(uint32_t *zero_mask, uint32_t *full_mask, uint32_t *init_z_dist, Input_data *input_data, Pattern *patterns);

#endif // !TRAVERSE_H
