#ifndef C3_SIMPLEBASHUTILS_0_S21_GREP_H
#define C3_SIMPLEBASHUTILS_0_S21_GREP_H

#define NOT_FOUND (-1)
#define ERR       (-1)
#define SUCCESS     1

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

typedef struct StringVector {
    int size;
    char** strings;
} StringVector;

void init_vector(StringVector* vec);
void push_back(StringVector* vec, char* string);
int get_index_of(StringVector* vec, char* str);
void replace_at(StringVector* vec, int ind, char* str);
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
    StringVector templates;
    StringVector regex_files;
} Arguments;

typedef struct LineInfo {
    char* filename;
    char* full_line;
    char* matching_line;
    int line_number;
} LineInfo;

void init_line_info(LineInfo* info, char* filename, char* full_line, char* matching_line, int line_number);

#endif //C3_SIMPLEBASHUTILS_0_S21_GREP_H
