/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/

typedef struct
{
    uint8_t *pattern_seq;
    uint32_t *occupies;
    uint32_t occupies_num;   // equal to size
    uint32_t *intersects_with;
    uint32_t inters_num;
    uint32_t * not_intersects_with;
    uint32_t non_inters_num;
    uint32_t *positions;
    uint32_t pattern_rev;
    uint32_t id;
} pattern;


typedef struct
{
    uint32_t number;
    uint32_t patterns[2];
} position;


typedef struct
{
    uint32_t pat_num;
    uint32_t pat_id;
} pat_list_search_elem;


uint32_t sum_pattern(uint32_t pattern_len, uint8_t * pattern_seq)
// for a pattern sequence just compute a sum
{
    int sum = 0;
    uint8_t *ptr = pattern_seq;
    while (ptr < &pattern_seq[pattern_len - 1])
    {
        sum += *ptr;
        ptr++;
    }
    return sum;
}

int comp_search_elems(const void *a, const void *b)
// comp. function to compare search elems in qsort
{ 
    pat_list_search_elem *ia = (pat_list_search_elem *)a;
    pat_list_search_elem *ib = (pat_list_search_elem *)b;
    return ia->pat_num - ib->pat_num;
}


uint32_t pat_to_num(uint32_t pattern_len, uint8_t * pattern_seq)
{
    int kt = 0;
    uint32_t answer = 0;
    for (int i = pattern_len - 1, j = 1; i >= 0; i--, j++){
        kt = j * 2;
        answer += pattern_seq[i] * kt;
    }
    return answer;
}

bool intersect_by_occ(uint32_t *occupies_1, uint32_t occ_num_1,
                      uint32_t *occupies_2, uint32_t occ_num_2)
// return true if patterns intersect, false otherwise
{   
    if ((occupies_1[occ_num_1] < occupies_2[0]) || (occupies_1[0] > occupies_2[occ_num_2]))
    {
        return false;
    }
    for (uint32_t i = 0; i < occ_num_1; i++){
        // if smaller that the smallest element in other arr then continue
        if (occupies_1[i] < occupies_2[0]){continue;}
        else if (occupies_1[i] > occupies_2[occ_num_2 - 1]){return false;}
        for (uint32_t j = 0; j <= occ_num_2; j++){
            if (occupies_1[i] == occupies_2[j]){
                return true;
            }
        }
    }
    return false;
}
