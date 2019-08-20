// Bogdan Kirilenko
// 2019, Dresden
// Some simple with arrays
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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


// compare two patterns
bool arr_uint8_are_the_same(uint8_t *arr_1, uint8_t *arr_2, uint32_t arr_size)
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
void arr_1D_uint32_print(uint32_t *arr, uint32_t arr_size){
    for (uint32_t i = 0; i < arr_size; ++i){printf("%u ", arr[i]);}
    printf("\n");
}
