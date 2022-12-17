#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "s21_cat.h"

void prepare_for_fill(char** first, char** second) {
    if (*first != NULL) {
        free(*first);
    }
    *first = *second;
}

char* apply_flags(Arguments* args, char* buffer, size_t* len) {
    char* return_value = NULL;

    if (args->s_flag) {
        return_value = apply_s_flag(buffer, len);
        prepare_for_fill(&buffer, &return_value);
    }
    if (args->t_flag) {
        return_value = two_sym_replacement(buffer, len, t_flag_validator, '^', t_flag_second_value);
        prepare_for_fill(&buffer, &return_value);
    }
    if (args->b_flag) {
        return_value = number_lines(buffer, len, 0);
        prepare_for_fill(&buffer, &return_value);
    }
    if (args->e_flag) {
        return_value = two_sym_replacement(buffer, len, e_flag_validator, '$', e_flag_second_value);
        prepare_for_fill(&buffer, &return_value);
    }
    if (args->v_flag) {
        return_value = two_sym_replacement(buffer, len, v_flag_validator, '^', v_flag_second_value);
        prepare_for_fill(&buffer, &return_value);
    }
    if (args->n_flag) {
        return_value = number_lines(buffer, len, 1);
        prepare_for_fill(&buffer, &return_value);
    }

    return buffer;
}

void shorten_flags(char** flags, int flags_count) {
    for (int i = 1; i < flags_count; ++i) {
        if (strcmp(flags[i], "--number-nonblank") == 0) {
            flags[i] = "-b";
        } else if (strcmp(flags[i], "--number") == 0) {
            flags[i] = "-n";
        } else if (strcmp(flags[i], "--squeeze-blank") == 0) {
            flags[i] = "-s";
        }
    }
}

char* apply_s_flag(const char* src, size_t* len) {
    int changes_count = 0;
    if (src[0] == '\n' && src[1] == '\n') {
        ++changes_count;
    }
    for (size_t i = 1; i < *len - 1; ++i) {
        if (src[i] == '\n' && src[i + 1] == '\n' && src[i - 1] == '\n') {
            ++changes_count;
        }
    }

    size_t new_len = *len - changes_count;
    char* modified = (char*) calloc(sizeof(char), new_len + 1);

    size_t src_index = 0;
    if (src[0] == '\n' && src[1] == '\n') {
        ++src_index;
    }

    for (size_t res_index = 0; src_index < *len; ++res_index, ++src_index) {
        if (src[src_index] == '\n') {
            while (src[src_index + 1] == '\n' && src[src_index - 1] == '\n') {
                ++src_index;
            }
            modified[res_index] = '\n';
        } else {
            modified[res_index] = src[src_index];
        }
    }
    *len = new_len;

    return modified;
}

int t_flag_validator(char ch) { return ch == '\t'; }
char t_flag_second_value(char) { return 'I'; }

int e_flag_validator(char ch) { return ch == '\n'; }
char e_flag_second_value(char) { return '\n'; }

int v_flag_validator(char ch) {
    int return_value = 0;
    if (((ch >= 0 && ch <= 31) || ch == 127) &&
        (ch != '\n' && ch != '\t')) {
        return_value = 1;
    }
    return return_value;
}

char v_flag_second_value(char ch) {
    char return_value;
    if (ch >= 0 && ch <= 31) {
        return_value = (char) (ch + 64);
    } else {
        return_value = '?';
    }
    return return_value;
}

char* two_sym_replacement(char* src, size_t* len, int (*validator)(char), char first_val, char (*second_val)(char ch)) {
    int changes_count = 0;
    for (size_t i = 0; i < *len; ++i) {
        if (validator(src[i])) {
            ++changes_count;
        }
    }

    char* return_value = NULL;
    size_t new_len = *len + changes_count;

    char* modified = (char *) calloc(sizeof(char), new_len + 1);
    for (size_t src_index = 0, res_index = 0; src_index < *len; ++src_index, ++res_index) {
        if (validator(src[src_index])) {
            modified[res_index] = first_val;
            ++res_index;
            modified[res_index] = second_val(src[src_index]);
        } else {
            modified[res_index] = src[src_index];
        }
    }
    return_value = modified;
    *len = new_len;

    return return_value;
}

void insert_counter_beginning(char* src, const char* format, size_t ind, int num) {
    char* new_substr = (char*) calloc(sizeof(char), 9);
    sprintf(new_substr, format, num);
    for (int i = 0; i < 8; ++i) {
        src[ind + i] = new_substr[i];
    }
    free(new_substr);
}

char* number_lines(char* src, size_t* len, int with_blank) {
    char* return_value = NULL;

    if (strcmp(src, "") == 0) {
        return_value = src;
    } else {
        int changes_count = 0;

        if (with_blank || src[0] != '\n') {
            changes_count += 7;
        }

        for (size_t i = 0; i < *len - 1; ++i) {
            if (src[i] == '\n') {
                if (with_blank || src[i + 1] != '\n') {
                    changes_count += 7;
                }
            }
        }

        size_t new_len = *len + changes_count;
        char* modified = (char*) calloc(sizeof(char), new_len + 1);

        int counter = 1;
        size_t res_index = 0;
        if (with_blank || src[0] != '\n') {
            insert_counter_beginning(modified, "%6d\t\n", 0, counter);
            res_index += 7;
            ++counter;
        }

        for (size_t src_index = 0; src_index < *len; ++src_index, ++res_index) {
            if (src[src_index] == '\n' && (src[src_index + 1] != '\0' && (with_blank || src[src_index + 1] != '\n'))) {
                insert_counter_beginning(modified, "\n%6d\t", res_index, counter);
                res_index += 7;
                ++counter;
            } else {
                modified[res_index] = src[src_index];
            }
        }
        return_value = modified;
        *len = new_len;
    }
    return return_value;
}

void parseArguments(Arguments* args, char** argv, int argc) {
    args->b_flag = 0;
    args->e_flag = 0;
    args->n_flag = 0;
    args->s_flag = 0;
    args->t_flag = 0;
    args->v_flag = 0;
    args->files_count = 0;
    args->broken_file = 0;
    args->files = NULL;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            size_t len = strlen(argv[i]);
            for (size_t j = 1; j < len; ++j) {
                if (argv[i][j] == 'b') {
                    args->b_flag = 1;
                }
                if (argv[i][j] == 'e' || argv[i][j] == 'E') {
                    args->e_flag = 1;
                }
                if (argv[i][j] == 'n') {
                    args->n_flag = 1;
                }
                if (argv[i][j] == 's') {
                    args->s_flag = 1;
                }
                if (argv[i][j] == 't' || argv[i][j] == 'T') {
                    args->t_flag = 1;
                }
                if (argv[i][j] == 'v' || argv[i][j] == 't' || argv[i][j] == 'e') {
                    args->v_flag = 1;
                }
            }
        } else {
            ++(args->files_count);
            args->files = realloc(args->files, args->files_count);
            args->files[args->files_count - 1] = argv[i];
        }
    }

    if (args->n_flag && args->b_flag) {
        args->n_flag = 0;
    }
}

char* fill_buffer(Arguments* args, char* buffer, size_t* len) {
    for (int i = 0; i < args->files_count; ++i) {
        FILE* file = fopen(args->files[i], "r");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            size_t fsize = (size_t) ftell(file);
            rewind(file);

            buffer = realloc(buffer, *len + fsize);
            for (size_t j = *len; j < fsize + *len; ++j) {
                buffer[j] = (char) fgetc(file);
            }
            *len += fsize;

            fclose(file);
        } else {
            args->broken_file = 1;
        }
    }

    return buffer;
}

int main(int argc, char** argv) {
    if (argc < 1) {
        puts("wrong arguments number");
    } else {
        Arguments args;
        shorten_flags(argv, argc);
        parseArguments(&args, argv, argc);

        size_t len = 0;
        char* buffer = NULL;
        buffer = fill_buffer(&args, buffer, &len);
        if (!args.broken_file) {
            char* modified = apply_flags(&args, buffer, &len);
            printf("%s", modified);
        }
    }

    return 0;
}
