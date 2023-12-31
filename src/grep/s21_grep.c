#include "s21_grep.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strip_end(char** str) {
  size_t len = strlen(*str);
  if ((*str)[len - 1] == '\n') {
    (*str)[len - 1] = '\0';
  }
}

void allocate_and_copy(char** dst, char** src) {
  *dst = (char*)calloc(sizeof(char), strlen(*src) + 1);
  strcpy(*dst, *src);
}

void init_arguments(Arguments* args) {
  args->e_flag = 1;

  args->i_flag = 0;
  args->v_flag = 0;
  args->c_flag = 0;
  args->l_flag = 0;
  args->n_flag = 0;
  args->h_flag = 0;
  args->s_flag = 0;
  args->f_flag = 0;
  args->o_flag = 0;

  init_vector(&args->files);
  init_vector(&args->templates);
  init_vector(&args->regex_files);
}

void init_vector(StringVector* vec) {
  vec->size = 0;
  vec->strings = NULL;
}

void push_back(StringVector* vec, char* string) {
  vec->size += 1;
  vec->strings = realloc(vec->strings, sizeof(char*) * (vec->size));
  allocate_and_copy(&vec->strings[vec->size - 1], &string);
}

void clear(StringVector* vec) {
  if (vec->strings != NULL) {
    for (int i = 0; i < vec->size; ++i) {
      free(vec->strings[i]);
    }
    free(vec->strings);
  }
}

int get_index_of(StringVector* vec, char* str) {
  int result = NOT_FOUND;
  for (int i = 0; i < vec->size; ++i) {
    if (strcmp(vec->strings[i], str) == 0) {
      result = i;
    }
  }
  return result;
}

void replace_at(StringVector* vec, int ind, char* str) {
  free(vec->strings[ind]);
  vec->strings[ind] = calloc(sizeof(char), strlen(str) + 1);
  strcpy(vec->strings[ind], str);
}

void free_args(Arguments* args) {
  clear(&args->files);
  clear(&args->regex_files);
  clear(&args->templates);
}

int parse_arguments(Arguments* args, char** argv, int argc) {
  int res = SUCCESS;

  int templates_present = 0;
  for (int i = 1; i < argc && !templates_present; ++i) {
    if (argv[i][0] == '-') {
      size_t len = strlen(argv[i]);
      for (size_t j = 1; j < len; ++j) {
        if (argv[i][j] == 'e' || argv[i][j] == 'f') {
          templates_present = 1;
        }
      }
    }
  }

  for (int i = 1; i < argc && res == SUCCESS; ++i) {
    if (argv[i][0] == '-') {
      size_t len = strlen(argv[i]);
      for (size_t j = 1; j < len && res == SUCCESS; ++j) {
        if (argv[i][j] == 'e') {
          if (j == len - 1 && i < argc - 1) {
            ++i;
            push_back(&args->templates, argv[i]);
          } else if (j != len - 1) {
            push_back(&args->templates, argv[i] + j + 1);
            j = len - 1;
          }
        } else if (argv[i][j] == 'i') {
          args->i_flag = 1;
        } else if (argv[i][j] == 'v') {
          args->v_flag = 1;
        } else if (argv[i][j] == 'c') {
          args->c_flag = 1;
        } else if (argv[i][j] == 'l') {
          args->l_flag = 1;
        } else if (argv[i][j] == 'n') {
          args->n_flag = 1;
        } else if (argv[i][j] == 'h') {
          args->h_flag = 1;
        } else if (argv[i][j] == 's') {
          args->s_flag = 1;
        } else if (argv[i][j] == 'f') {
          args->f_flag = 1;

          if (j == len - 1 && i < argc - 1) {
            ++i;
            push_back(&args->regex_files, argv[i]);
          } else {
            res = ERR;
          }

        } else if (argv[i][j] == 'o') {
          args->o_flag = 1;
        }
      }
    } else {
      if (templates_present) {
        push_back(&args->files, argv[i]);
      } else {
        push_back(&args->templates, argv[i]);
        templates_present = 1;
      }
    }
  }

  if (args->c_flag) {
    args->o_flag = 0;
  }

  return res;
}

int is_escape_sym(char* ch) {
  char next_sym = *(ch + 1);
  return *ch == '(' || *ch == ')' ||
         (*ch == '\\' && next_sym != '}' && next_sym != '{');
}

int count_escapes_in_string(char* string) {
  int count = 0;
  size_t len = strlen(string);
  for (size_t i = 0; i < len; ++i) {
    if (is_escape_sym(&string[i])) {
      ++count;
    }
  }
  return count;
}

int count_escapes_in_vector(StringVector* templates) {
  int count = 0;
  for (int i = 0; i < templates->size; ++i) {
    count += count_escapes_in_string(templates->strings[i]);
  }
  return count;
}

void copy_with_escapes(char* dst, char* src) {
  for (; *src != '\0'; ++dst, ++src) {
    if (is_escape_sym(src)) {
      *dst = '\\';
      ++dst;
    }
    *dst = *src;
  }
}

char* make_regex(StringVector* templates) {
  size_t size = 0;
  size_t first_len = strlen(templates->strings[0]);

  size += first_len;
  for (int i = 1; i < templates->size; ++i) {
    size += strlen(templates->strings[i]) + 1;
  }
  if (templates->size > 1) {
    size += 2;  // for ()
  }
  size += count_escapes_in_vector(templates);

  char* result = (char*)calloc(sizeof(char), size + 1);
  char* temp = result;

  if (templates->size > 1) {
    *temp = '(';
    ++temp;
  }

  copy_with_escapes(temp, templates->strings[0]);
  temp += first_len + count_escapes_in_string(templates->strings[0]);

  for (int i = 1; i < templates->size; ++i) {
    *temp = '|';
    ++temp;
    copy_with_escapes(temp, templates->strings[i]);
    temp += strlen(templates->strings[i]) +
            count_escapes_in_string(templates->strings[i]);
  }

  if (templates->size > 1) {
    *temp = ')';
  }

  return result;
}

void init_line_info(LineInfo* info, char* filename, char* full_line,
                    char* matching_line, int line_number) {
  info->filename = filename;
  info->full_line = full_line;
  info->matching_line = matching_line;
  info->line_number = line_number;
}

void push_filename(Arguments* args, StringVector* vec, char* filename) {
  if (args->files.size > 1 && !args->h_flag) {
    push_back(vec, filename);
    push_back(vec, ":");
  }
}

void output_line(StringVector* acc, Arguments* args, LineInfo* info) {
  strip_end(&info->full_line);

  push_filename(args, acc, info->filename);
  if (args->n_flag) {
    char num_buff[10];
    sprintf(num_buff, "%d", info->line_number);
    push_back(acc, num_buff);
    push_back(acc, ":");
  }
  if (args->o_flag) {
    push_back(acc, info->matching_line);
  } else {
    push_back(acc, info->full_line);
  }
  push_back(acc, "\n");
}

void output_count(StringVector* acc, Arguments* args, LineInfo* info) {
  int current_index = (args->files.size > 1 && !args->h_flag) ? 2 : 0;

  if (acc->size == 0) {
    push_filename(args, acc, info->filename);
    push_back(acc, "0");
    push_back(acc, "\n");
  }

  int num = atoi(acc->strings[current_index]);

  char buff[10];
  sprintf(buff, "%d", num + 1);

  replace_at(acc, current_index, buff);
}

void output_files(StringVector* acc, Arguments*, LineInfo* info) {
  if (get_index_of(acc, info->filename) == NOT_FOUND) {
    push_back(acc, info->filename);
    push_back(acc, "\n");
  }
}

void main_search(Arguments* args,
                 void (*match_action)(StringVector*, Arguments*, LineInfo*)) {
  char* combo_template = make_regex(&args->templates);

  regex_t reg;
  regmatch_t match_buffer[1];
  regoff_t match_len;

  int cflags = REG_EXTENDED | REG_NEWLINE;
  if (args->i_flag) {
    cflags |= REG_ICASE;
  }
  regcomp(&reg, combo_template, cflags);  // i flag

  for (int i = 0; i < args->files.size; ++i) {
    FILE* file = fopen(args->files.strings[i], "r");
    if (file != NULL) {
      char* line = NULL;
      size_t len = 0;

      StringVector accumulator;
      init_vector(&accumulator);

      for (int j = 1; getline(&line, &len, file) != -1; ++j) {
        char* temp_line = line;
        int matches_left = 1;

        while (matches_left) {
          int reg_res = regexec(&reg, temp_line, ARRAY_SIZE(match_buffer),
                                match_buffer, 0);

          if ((reg_res == 0 && !args->v_flag) ||
              (reg_res != 0 && args->v_flag)) {  // v flag
            char* pure_match = NULL;
            if (!args->v_flag) {
              match_len = match_buffer[0].rm_eo - match_buffer[0].rm_so;
              pure_match = calloc(sizeof(char), match_len + 1);
              sprintf(pure_match, "%.*s", (int)match_len,
                      temp_line + match_buffer[0].rm_so);
            } else {
              pure_match = line;
            }

            LineInfo info;
            init_line_info(&info, args->files.strings[i], line, pure_match, j);
            match_action(&accumulator, args, &info);

            temp_line += match_buffer[0].rm_eo;

            if (!args->v_flag) {
              free(pure_match);
            }
            if (args->v_flag || !args->o_flag) {
              matches_left = 0;
            }
          } else {
            matches_left = 0;
          }
        }
        free(line);
        line = NULL;
      }
      free(line);
      fclose(file);

      if (!args->l_flag && args->c_flag && accumulator.size == 0) {
        push_filename(args, &accumulator, args->files.strings[i]);
        push_back(&accumulator, "0");
        push_back(&accumulator, "\n");
      }
      for (int j = 0; j < accumulator.size; ++j) {
        printf("%s", accumulator.strings[j]);
      }

      clear(&accumulator);
    }
  }

  free(combo_template);
  regfree(&reg);
}

void apply_flags(Arguments* args) {
  if (args->templates.size != 0) {
    if (args->l_flag) {
      main_search(args, output_files);
    } else if (args->c_flag) {
      main_search(args, output_count);
    } else if (args->e_flag && !(args->o_flag && args->v_flag)) {
      main_search(args, output_line);
    }
  }
}

void process_regex_files(Arguments* args) {
  for (int i = 0; i < args->regex_files.size; ++i) {
    FILE* file = fopen(args->regex_files.strings[i], "r");
    if (file != NULL) {
      char* line = NULL;
      size_t len;
      while (getline(&line, &len, file) != -1) {
        strip_end(&line);
        push_back(&args->templates, line);
      }
      fclose(file);
      free(line);
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 1) {
    puts("wrong arguments number");
  } else {
    Arguments args;
    init_arguments(&args);
    if (parse_arguments(&args, argv, argc) == SUCCESS) {
      process_regex_files(&args);
      apply_flags(&args);
    }
    free_args(&args);
  }
}
