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
    int result = -1;
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

    for (int i = 1; i < argc && res == SUCCESS; ++i) {
        if (argv[i][0] == '-') {
            size_t len = strlen(argv[i]);
            for (size_t j = 1; j < len && res == SUCCESS; ++j) {
                if (argv[i][j] == 'e' && j == len - 1 && i < argc - 1) {
                    ++i;
                    push_back(&args->templates, argv[i]);
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
                        push_back(&args->regex_files, argv[i]);
                    } else {
                        res = ERR;
                    }

                } else if (argv[i][j] == 'o') {
                    args->o_flag = 1;
                }
            }
        } else {
            if (args->templates.size == 0) {
                push_back(&args->templates, argv[i]);
            } else {
                push_back(&args->files, argv[i]);
            }
        }
    }

    if (args->c_flag && args->e_flag) {
        args->e_flag = 0;
    }

    return res;
}

char* make_template(StringVector* templates) {
    size_t size = 0;
    size_t first_len = strlen(templates->strings[0]);

    size += first_len;
    for (int i = 1; i < templates->size; ++i) {
        size += strlen(templates->strings[i]) + 1;
    }

    char* result = (char*) calloc(sizeof(char), size + 1);
    char* temp = result;

    strcpy(result, templates->strings[0]);
    temp += first_len;

    for (int i = 1; i < templates->size; ++i) {
        *temp = '|';
        ++temp;
        strcpy(temp, templates->strings[i]);
        temp += strlen(templates->strings[i]);
    }

    return result;
}

void output_line(StringVector* acc, StringVector* files, char* line, char* filename) {
    size_t line_len = strlen(line);
    if (line[line_len - 1] == '\n') {
        line[line_len - 1] = '\0';
    }

    if (files->size > 1) {
        push_back(acc, filename);
        push_back(acc, ":");
    }
    push_back(acc, line);
    push_back(acc, "\n");
}

void output_count(StringVector* acc, StringVector* files, char*, char* filename) {
    if (acc->size == 0) {
        for (int i = 0; i < files->size; ++i) {
            if (files->size > 1) {
                push_back(acc, files->strings[i]);
                push_back(acc, ":");
            }
            push_back(acc, "0");
            push_back(acc, "\n");
        }
    }

    int ind = get_index_of(acc, filename);
    int num = atoi(acc->strings[ind + 2]);

    char buff[10];
    sprintf(buff, "%d", num + 1);

    replace_at(acc, ind + 2, buff);
}

void basic_search(Arguments* args, void(*match_action)(StringVector*, StringVector*, char*, char*)) {
    char* combo_template = make_template(&args->templates);

    regex_t reg;
    regcomp(&reg, combo_template, args->i_flag ? REG_ICASE : 0);

    int res = SUCCESS;
    for (int i = 0; i < args->files.size && res == SUCCESS; ++i) {
        FILE* file = fopen(args->files.strings[i], "r");
        if (file != NULL) {
            char* line = NULL;
            size_t len = 0;

            StringVector accumulator;
            init_vector(&accumulator);

            while (getline(&line, &len, file) != -1) {
                int reg_res = regexec(&reg, line, 0, NULL, 0);
                if ((reg_res == 0 && !args->v_flag) || args->v_flag) {
                    match_action(&accumulator, &args->files, line, args->files.strings[i]);
                }
            }
            fclose(file);
            for (int j = 0; j < accumulator.size; ++j) {
                printf("%s", accumulator.strings[j]);
            }

            clear(&accumulator);
        } else {
            res = ERR;
        }
    }

    free(combo_template);
    regfree(&reg);
}

void apply_flags(Arguments* args) {
    if (args->e_flag) {
        basic_search(args, output_line);
    } else if (args->c_flag) {
        basic_search(args, output_count);
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
