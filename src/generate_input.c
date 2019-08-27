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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
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

struct stat st = {0};

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


// copy arr of uint8_t
void __copy_uint8_(uint8_t *to, uint8_t *from, uint16_t len)
{
    for (uint16_t i = 0; i < len; ++i){to[i] = from[i];}
}


// shuffle an array
void _shuffle(uint16_t *arr, uint16_t size)
{
    uint16_t i;
    uint16_t j;
    uint16_t t;
    srand(time(NULL));
    for (i = 0; i < size - 1; i++) 
    {
        j = i + rand() / (RAND_MAX / (size - i) + 1);
        t = arr[j];
        arr[j] = arr[i];
        arr[i] = t;
    }
}


// copy arr of uint16_t
void _copy_uint16_(uint16_t *to, uint16_t *from, uint16_t amount)
{
    for (uint16_t i = 0; i < amount; ++i){to[i] = from[i];}
} 


// write 2D arr to file
void _write_to_file(char *file_path, uint8_t **arr, uint16_t rows, uint16_t cols)
{
    FILE *f;
    f = fopen(file_path, "w");
    for (uint32_t row = 0; row < rows; ++row){
        for (uint32_t col = 0; col < cols; ++col){
            fprintf(f, "%d", arr[row][col]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}


// swap 1 and 0
void _swap_(uint8_t *num) {*num = 1 - *num;}

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
    char input_dir[MAXCHAR];
    strcpy(input_dir, "tests/input_files/");
    if (stat(input_dir, &st) == -1) {
        mkdir(input_dir, 0700);
    }
    strcat(input_dir, input_data.dataset_name);
    // create input dir if not exists
    if (stat(input_dir, &st) == -1) {
        mkdir(input_dir, 0700);
    }
    
    uint16_t *indexes = (uint16_t*)malloc(input_data.str_len * sizeof(uint16_t));
    uint16_t *first_k = (uint16_t*)calloc(input_data.k_, sizeof(uint16_t));

    for (uint32_t i = 0; i < input_data.str_len; ++i){indexes[i] = i;}


    for (uint16_t rep_num = 0; rep_num < input_data.replicates_num; ++rep_num)
    // generate replicate
    {
        // we need origin string here
        uint8_t *origin_string = __gen_origin_string(input_data.str_len);
        // need str_num random strings originated from origin string
        uint8_t **strings = (uint8_t**)malloc(input_data.str_num * sizeof(uint8_t*));
        for (uint16_t i = 0; i < input_data.str_num; ++i){
            strings[i] = (uint8_t*)calloc(input_data.str_len, sizeof(uint8_t));
        }

        for (uint16_t str_num = 0; str_num < input_data.str_num; ++str_num)
        {
            // get local copy of the origin string
            __copy_uint8_(strings[str_num], origin_string, input_data.str_len);
            // get indexes to swap
            _shuffle(indexes, input_data.str_len);
            _copy_uint16_(first_k, indexes, input_data.k_);
            for (uint16_t n = 0; n < input_data.k_; ++n){
                _swap_(&strings[str_num][first_k[n]]);
            }
        }

        // now save strings to the file
        char filename[16];
        sprintf(filename, "/%u.in.txt", rep_num);
        char file_path[MAXCHAR];
        strcpy(file_path, input_dir);
        strcat(file_path, filename);
        _write_to_file(file_path, strings, input_data.str_num, input_data.str_len);

        // TODO: maybe allocate it only once?
        free(origin_string);
    }

    free(indexes);
    free(first_k);
}
