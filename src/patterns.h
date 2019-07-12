/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/

typedef struct{
    uint8_t *pattern_seq;
    int *occupies;
    int occupies_num;   // equal to size
    int * intersects_with;
    int inters_num;
    int * not_intersects_with;
    int non_inters_num;
    int * positions;
    int id;
} pattern;

typedef struct{
    int number;
    int * patterns;
} position;

typedef struct{
    int pat_num;
    int pat_id;
} pat_list_search_elem;

uint32_t sum_pattern(uint32_t pattern_len, uint8_t * pattern_seq){
    int sum = 0;
    for (int i = 0; i < pattern_len; i++) {sum += pattern_seq[i];}
    return sum;
}

int comp_search_elems(const void *a, const void *b) 
{ 
    pat_list_search_elem *ia = (pat_list_search_elem *)a;
    pat_list_search_elem *ib = (pat_list_search_elem *)b;
    return ia->pat_num - ib->pat_num;
}


uint32_t pat_to_num(uint32_t pattern_len, uint8_t * pattern_seq){
    int kt = 0;
    uint32_t answer = 0;
    for (int i = pattern_len - 1, j = 1; i >= 0; i--, j++){
        kt = j * 2;
        answer += pattern_seq[i] * kt;
    }
    return answer;
}

bool intersect_by_occ(int *occupies_1, int occ_num_1, int *occupies_2, int occ_num_2)
// return true if patterns intersect, false otherwise
{   
    if (occupies_1[occ_num_1] < occupies_2[0] || occupies_1[0] > occupies_2[occ_num_2]){
        return false;}
    for (int i = 0; i < occ_num_1; i++){
        // if smaller that the smallest element in other arr then continue
        if (occupies_1[i] < occupies_2[0]){continue;}
        else if (occupies_1[i] > occupies_2[occ_num_2 - 1]){return false;}
        for (int j = 0; j <= occ_num_2; j++){
            if (occupies_1[i] == occupies_2[j]){
                return true;
            }
        }
    }
    return false;
}
