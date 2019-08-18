// Bogdan Kirilenko
// 2019, Dresden
// Render sequences
#ifndef RENDER_H
#define RENDER_H
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
#include "read_input.h"
#include "patterns.h"

uint8_t **render__draw(Pattern *patterns, uint32_t *mask, Input_data *input_data);

void render__show_arr(uint8_t **arr, uint32_t rows, uint32_t cols);

uint32_t *render__get_zeros(uint8_t **init_render_data, uint32_t rows, uint32_t cols);

uint32_t render__get_min_zeros_amount(uint8_t **init_render_data, uint32_t rows, uint32_t cols);

#endif // !RENDER_H
