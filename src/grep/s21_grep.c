#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "s21_grep.h"

void allocate_and_copy(char** dst, char** src) {
    *dst = (char*) calloc(sizeof(char), strlen(*src) + 1);
    strcpy(*dst, *src);
}

void init_arguments(Arguments* args) {
    args->e_flag = 0;
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

    args->regex_file = NULL;
    args->template = NULL;
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

void free_args(Arguments* args) {
    clear(&args->files);
    if (args->regex_file != NULL) {
        free(args->regex_file);
    }
}

int parse_arguments(Arguments* args, char** argv, int argc) {
    int res = SUCCESS;

    for (int i = 1; i < argc && res == SUCCESS; ++i) {
        if (argv[i][0] == '-') {
            size_t len = strlen(argv[i]);
            for (size_t j = 1; j < len && res == SUCCESS; ++j) {
                if (argv[i][j] == 'e') {
                    args->e_flag = 1;
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

                    if (j == len - 1 && i < argc - 2) {
                        ++i;
                        allocate_and_copy(&args->regex_file, &argv[i]);
                    } else {
                        res = ERR;
                    }

                } else if (argv[i][j] == 'o') {
                    args->o_flag = 1;
                }
            }
        } else {
            allocate_and_copy(&args->template, &argv[i]);
            for (int j = i + 1; j < argc; ++j) {
                push_back(&args->files, argv[j]);
            }
            i = argc;
        }
    }

    return res;
}

void basic_search(const char* template, StringVector* files) {
    regex_t reg;
    regcomp(&reg, template, 0);

    int res = SUCCESS;
    for (int i = 0; i < files->size && res == SUCCESS; ++i) {
        FILE* file = fopen(files->strings[i], "r");
        if (file != NULL) {
            char* line = NULL;
            size_t len = 0;

            while (getline(&line, &len, file) != -1) {
                int reg_res = regexec(&reg, line, 0, NULL, 0);
                if (reg_res == 0) {
                    size_t line_len = strlen(line);
                    if (line[line_len - 1] == '\n') {
                        line[line_len - 1] = '\0';
                    }

                    if (files->size > 1) {
                        printf("%s:", files->strings[i]);
                    }

                    printf("%s\n", line);
                }
            }
            fclose(file);
        } else {
            res = ERR;
        }
    }

    regfree(&reg);
}

void apply_flags(Arguments* args) {
    int flag_applied = 0;

    if (args->e_flag) {
        basic_search(args->template, &args->files);
        flag_applied = 1;
    }

    if (!flag_applied) {
        basic_search(args->template, &args->files);
    }
}

int main(int argc, char** argv) {
    if (argc < 1) {
        puts("wrong arguments number");
    } else {
        Arguments args;
        init_arguments(&args);
        if (parse_arguments(&args, argv, argc) == SUCCESS) {
            apply_flags(&args);
        }
        free_args(&args);
    }
}
