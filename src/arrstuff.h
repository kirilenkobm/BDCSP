// Bogdan Kirilenko
// 2019, Dresden
// Some simple with arrays
#ifndef ARRSTUFF_H
#define ARRSTUFF_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t arr_sum(uint32_t *arr, uint32_t size);

uint32_t arr_max(uint32_t *arr, uint32_t size);

uint32_t *arr_1D_uint32_copy(uint32_t *array, uint32_t size);

uint8_t **arr_2D_uint8_copy(uint8_t **arr, uint32_t arrs_num, uint32_t elem_num);

void arr_2D_uint8_swap_lines(uint8_t **arr, uint32_t one, uint32_t two);

bool arr_uint8_are_the_same(uint8_t *arr_1, uint8_t *arr_2, uint32_t arr_size);

bool arr_uint32_are_the_same(uint32_t *arr_1, uint32_t *arr_2, uint32_t arr_size);

bool arr_uint8_all_eq(uint8_t *arr, uint32_t arr_size);

void arr_1D_uint32_print(uint32_t *arr, uint32_t arr_size);

void arr_1D_uint32_copy_from_to
(uint32_t *to, uint32_t *from, uint32_t size);

void arr_2D_uint8_print(uint8_t **arr, uint32_t rows, uint32_t cols);

#endif
