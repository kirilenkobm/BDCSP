/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/

typedef struct{
    int * pattern_seq;
    int * occupies;
    int occupies_num;
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


int sum_pattern(int pattern_len, int * pattern_seq){
    int sum = 0;
    for (int i = 0; i < pattern_len; i++) {sum += pattern_seq[i];}
    return sum;
}

bool intersect_by_occ(int *occupies_1, int occ_num_1, int *occupies_2, int occ_num_2)
// return true if patterns intersect, false otherwise
{   
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
