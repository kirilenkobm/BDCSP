// Bogdan Kirilenko
// 2019, Dresden
// Some simple with arrays
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "arrstuff.h"


// just array sum
uint32_t arr_sum(uint32_t *arr, uint32_t size)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < size; ++i){ans += arr[i];}
    return ans;
}


// max of array
uint32_t arr_max(uint32_t *arr, uint32_t size)
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < size; ++i){
        if (arr[i] > ans){ans = arr[i];}
    }
    return ans;
}

// max of array
uint32_t arr_min(uint32_t *arr, uint32_t size)
{
    uint32_t ans = UINT32_MAX;
    for (uint32_t i = 0; i < size; ++i){
        if (arr[i] < ans){ans = arr[i];}
    }
    return ans;
}


// copy 1D array of uint32_t
uint32_t *arr_1D_uint32_copy(uint32_t *array, uint32_t size)
{
    uint32_t *res = (uint32_t*)malloc(size * sizeof(uint32_t));
    for (uint32_t i = 0; i < size; ++i) {res[i] = array[i];}
    return res;
}


// copy 2D uint8_t array
uint8_t **arr_2D_uint8_copy(uint8_t **arr, uint32_t arrs_num, uint32_t elem_num)
{
    uint8_t **copy = (uint8_t**)malloc(arrs_num * sizeof(uint8_t*));
    for (uint32_t i = 0; i < arrs_num; ++i) {copy[i] = (uint8_t*)calloc(elem_num, sizeof(uint8_t));}
    for (uint32_t i = 0; i < arrs_num; ++i){
        for (uint32_t j = 0; j < elem_num; ++j) {copy[i][j] = arr[i][j];}
    }
    return copy;
}


void arr_1D_uint32_copy_from_to
(uint32_t *to, uint32_t *from, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i){to[i] = from[i];}
}

// swap rows in uint8_t array
void arr_2D_uint8_swap_lines(uint8_t **arr, uint32_t one, uint32_t two)
{
    uint8_t *temp = arr[one];
    arr[one] = arr[two];
    arr[two] = temp;
}


// compare two patterns
bool arr_uint8_are_the_same(uint8_t *arr_1, uint8_t *arr_2, uint32_t arr_size)
{
    for (uint32_t i = 0; i < arr_size; ++i){
        if (arr_1[i] != arr_2[i]){return false;}
    }
    return true;
}


// compare two patterns
bool arr_uint32_are_the_same(uint32_t *arr_1, uint32_t *arr_2, uint32_t arr_size)
{
    for (uint32_t i = 0; i < arr_size; ++i){
        if (arr_1[i] != arr_2[i]){return false;}
    }
    return true;
}


// check if all numbers are the same
bool arr_uint8_all_eq(uint8_t *arr, uint32_t arr_size)
{
    for (uint32_t i = 1; i < arr_size; ++i){
        if (arr[i] != arr[i - 1]){return false;}
    }
    return true;
}


// just print an 1D array
void arr_1D_uint8_print(uint8_t *arr, uint32_t arr_size){
    for (uint32_t i = 0; i < arr_size; ++i){printf("%u ", arr[i]);}
    printf("\n");
}


// just print an 1D array
void arr_1D_uint32_print(uint32_t *arr, uint32_t arr_size){
    for (uint32_t i = 0; i < arr_size; ++i){printf("%u ", arr[i]);}
    printf("\n");
}


// print 2D render
void arr_2D_uint8_print(uint8_t **arr, uint32_t rows, uint32_t cols)
{
    for (uint32_t row = 0; row < rows; ++row){
        printf("# ");
        for (uint32_t col = 0; col < cols; ++col){
            printf("%d ", arr[row][col]);
        }
        printf("\n");
    }
}
