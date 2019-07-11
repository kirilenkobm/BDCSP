typedef struct{
    int * pattern_seq;
    int * occupies;
    int * intersects_with;
    int * not_intersects_with;
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
