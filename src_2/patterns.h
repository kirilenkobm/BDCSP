#ifndef PATTERNS_H
#define PATTERNS_H
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
#include "read_input.h"

void invert_pattern(uint8_t *pattern, uint32_t size);

bool all_eq(uint8_t *col, uint32_t size);

uint8_t **get_patterns(Input_data input_data, uint32_t *patterns_num);

#endif // !PATTERNS_H
