#ifndef CSP_H
#define CSP_H
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
#include "patterns.h"


void _show_usage_and_quit(char * executable);

void verbose(const char * format, ...);

void v_verbose(const char * format, ...);

void free_all();

uint32_t min_of_three(uint32_t *a, uint32_t *b, uint32_t *c);

void get_init_density_range
(uint32_t *inf_cov, uint32_t *sup_cov, Input_data *input_data, Pattern *patterns);

uint32_t arr_sum(uint32_t *arr, uint32_t size);

uint32_t arr_max(uint32_t *arr, uint32_t size);

int main(int argc, char ** argv);

#endif // !CSP_H
