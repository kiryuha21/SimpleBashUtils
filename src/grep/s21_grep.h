#ifndef C3_SIMPLEBASHUTILS_0_S21_GREP_H
#define C3_SIMPLEBASHUTILS_0_S21_GREP_H

#define ERR     (-1)
#define SUCCESS   1

typedef struct StringVector {
    int size;
    char** strings;
} StringVector;

void init_vector(StringVector* vec);
void push_back(StringVector* vec, char* string);
void clear(StringVector* vec);

typedef struct Arguments {
    int e_flag;
    int i_flag;
    int v_flag;
    int c_flag;
    int l_flag;
    int n_flag;
    int h_flag;
    int s_flag;
    int f_flag;
    int o_flag;

    StringVector files;
    char* template;
    char* regex_file;
} Arguments;

#endif //C3_SIMPLEBASHUTILS_0_S21_GREP_H
