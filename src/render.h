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

void render__show__first(Pattern *patterns, Input_data *input_data);

#endif // !RENDER_H
