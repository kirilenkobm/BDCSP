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
#include "read_input.h"
#include "patterns.h"

uint8_t **render__draw(Pattern *patterns, uint32_t *mask, Input_data *input_data);

uint32_t *render__get_zeros(uint8_t **init_render_data, uint32_t rows, uint32_t cols);

uint32_t render__get_min_zeros_amount(uint8_t **init_render_data, uint32_t rows, uint32_t cols);

void render__free_render(uint8_t **arr, uint32_t depth);

#endif // !RENDER_H
