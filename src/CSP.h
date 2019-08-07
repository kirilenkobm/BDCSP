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

void _show_usage_and_quit(char * executable);

void verbose(const char * restrict format, ...);

void free_all(uint32_t str_len, uint32_t str_num, uint32_t patterns_num);

int main(int argc, char ** argv);

#endif // !CSP_H
