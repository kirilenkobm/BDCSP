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
#include <unistd.h>
#include <limits.h>
#include "patterns.h"


void _show_usage_and_quit(char * executable);

void verbose(const char * restrict format, ...);

void free_all();

uint32_t min_of_three(uint32_t *a, uint32_t *b, uint32_t *c);

void get_init_density_range(uint32_t *inf_cov, uint32_t *sup_cov, Input_data *input_data,
                            Dir_Rev *dir_rev_index, Pattern *patterns);

int main(int argc, char ** argv);

#endif // !CSP_H
