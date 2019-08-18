// Bogdan Kirilenko
// 2019, Dresden
// Walking over hypercube
#ifndef TRAVERSE_H
#define TRAVERSE_H
#include "patterns.h"
#include "render.h"


typedef struct{
    uint32_t pat_id;
    int8_t size;
} Move;

typedef struct{
    uint32_t *pat_mask;
    Move *moves;
    uint32_t moves_num;
    uint32_t cur_move;
} State;


#endif // !TRAVERSE_H
