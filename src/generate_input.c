// Bogdan Kirilenko
// 2019, Dresden
// Generate input for CSP
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

#define OPT_NUM 6
#define MAXCHAR 255

typedef struct
{
    uint16_t str_len;
    uint16_t str_num;
    uint16_t k_;
    uint16_t replicates_num;
    char *dataset_name;
} Input_data;


// show help and exit
void _show_usage_and_quit(char * executable)
{
    fprintf(stderr, "Usage: %s [str_len] [strnum] [k] [replicates_num] [dataset_name]\n", executable);
    exit(1);

}


// read input
void __read_input(Input_data *input_data, char **argv)
{
    input_data->str_len = (uint16_t)strtoul(argv[1], 0L, 10);
    input_data->str_num = (uint16_t)strtoul(argv[2], 0L, 10);
    input_data->k_ = (uint16_t)strtoul(argv[3], 0L, 10);
    input_data->replicates_num = (uint16_t)strtoul(argv[4], 0L, 10);
    input_data->dataset_name = (char*)malloc(MAXCHAR * sizeof(char));
    strcpy(input_data->dataset_name, argv[5]);
}


// generate origin string
uint8_t* __gen_origin_string(uint16_t len)
{
    uint8_t *orig = (uint8_t*)malloc(len * sizeof(uint8_t));
    for (uint16_t i = 0; i < len; ++i){
        orig[i] = rand() % 2;
    }
    return orig;
}


// just print the string
void __print_string(uint8_t *str, uint16_t len)
{
    for (uint16_t i = 0; i < len; ++i){printf("%u", str[i]);}
    printf("\n");
}


void __copy_arr(uint8_t *to, uint8_t *from, uint16_t len)
{
    for (uint16_t i = 0; i < len; ++i){to[i] = from[i];}
}


// enrty point
int main(int argc, char **argv)
{
    if (argc != 6){_show_usage_and_quit(argv[0]);}
    Input_data input_data;
    __read_input(&input_data, argv);
    printf("# Generating input with following parameters:\n");
    printf("# str_len: %u\n", input_data.str_len);
    printf("# str_num: %u\n", input_data.str_num);
    printf("# k_: %u\n", input_data.k_);
    printf("# repl_num: %u\n", input_data.replicates_num);
    printf("# dataset name: %s\n", input_data.dataset_name);
    
    uint16_t *indexes = (uint16_t*)calloc(input_data.k_, sizeof(uint16_t));

    for (uint16_t i = 0; i < input_data.replicates_num; ++i)
    // generate replicate
    {
        uint8_t *origin_string = __gen_origin_string(input_data.str_len);
        __print_string(origin_string, input_data.str_len);
        free(origin_string);
    }
}
