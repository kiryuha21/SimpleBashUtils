#ifndef C3_SIMPLEBASHUTILS_0_S21_CAT_H
#define C3_SIMPLEBASHUTILS_0_S21_CAT_H

#include <stdlib.h>

typedef struct Arguments {
  int b_flag;
  int e_flag;
  int n_flag;
  int s_flag;
  int t_flag;
  int v_flag;

  char** files;
  int files_count;
  int broken_file;
} Arguments;

char* apply_flags(Arguments* args, char* buffer, size_t* len);
char* apply_s_flag(const char* src, size_t* len);
char* number_lines(char* src, size_t* len, int with_blank);

// main func with callbacks
char* two_sym_replacement(char* src, size_t* len, int (*validator)(char ch),
                          char first_val, char (*second_val)(char ch));

// v flag callbacks
int v_flag_validator(char ch);
char v_flag_second_value(char ch);

// t flag callbacks
int t_flag_validator(char ch);
char t_flag_second_value(char);

// e flag callbacks
int e_flag_validator(char ch);
char e_flag_second_value(char);

#endif  // C3_SIMPLEBASHUTILS_0_S21_CAT_H
