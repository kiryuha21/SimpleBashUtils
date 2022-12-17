#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "s21_cat.h"

char* apply_flag(char* src, char flag) {
    char* return_value = NULL;

    if (flag == 'v') {
        return_value = two_sym_replacement(src, v_flag_validator, '^', v_flag_second_value);
    } else if (flag == 'b') {
        return_value = number_lines(src, 0);
    } else if (flag == 'n') {
        return_value = number_lines(src, 1);
    } else if (flag == 's') {
        return_value = apply_s_flag(src);
    } else if (flag == 'E') {
        return_value = two_sym_replacement(src, e_flag_validator, '$', e_flag_second_value);
    } else if (flag == 'T') {
        return_value = two_sym_replacement(src, t_flag_validator, '^', t_flag_second_value);
    } else if (flag == 't' || flag == 'e') {
        char* temp = two_sym_replacement(src, v_flag_validator, '^', v_flag_second_value);
        if (flag == 't') {
            return_value = two_sym_replacement(temp, t_flag_validator, '^', t_flag_second_value);
        } else {
            return_value = two_sym_replacement(temp, e_flag_validator, '$', e_flag_second_value);
        }
        free(temp);
    }

    return return_value;
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

char** optimize_flags(char** flags, int* flags_count) {
    int b = 0, e = 0, n = 0, s = 0, t = 0, v = 0;

    char* first_arg = flags[0];
    char* last_arg = flags[*flags_count - 1];

    for (int i = 1; i < *flags_count - 1; ++i) {
        size_t combo_len = strlen(flags[i]);
        for (size_t j = 1; j < combo_len; ++j) {
            if (flags[i][j] == 'b') {
                b = 1;
            } else if (flags[i][j] == 'e') {
                e = 1;
            } else if (flags[i][j] == 'n') {
                n = 1;
            } else if (flags[i][j] == 's') {
                s = 1;
            } else if (flags[i][j] == 't') {
                t = 1;
            } else if (flags[i][j] == 'v') {
                v = 1;
            }
        }
    }

    *flags_count = b + e + n + s + t + v + 2;
    if (b && n) {
        --(*flags_count);
    }
    char** new_flags = (char**) calloc(sizeof(char*), *flags_count);

    new_flags[0] = first_arg;

    int last_ind = 1;
    if (s) {
        new_flags[last_ind] = "-s";
        ++last_ind;
    }
    if (t) {
        new_flags[last_ind] = "-t";
        ++last_ind;
    }
    if (b) {
        new_flags[last_ind] = "-b";
        ++last_ind;
    }
    if (e) {
        new_flags[last_ind] = "-e";
        ++last_ind;
    }
    if (v) {
        new_flags[last_ind] = "-v";
        ++last_ind;
    } else if (n) {
        new_flags[last_ind] = "-n";
        ++last_ind;
    }

    new_flags[last_ind] = last_arg;

    return new_flags;
}

char* apply_s_flag(char* src) {
    size_t len = strlen(src);
    int changes_count = 0;
    if (src[0] == '\n' && src[1] == '\n') {
        ++changes_count;
    }
    for (size_t i = 1; i < len - 1; ++i) {
        if (src[i] == '\n' && src[i + 1] == '\n' && src[i - 1] == '\n') {
            ++changes_count;
        }
    }

    size_t new_len = len - changes_count;
    char* modified = (char*) calloc(sizeof(char), new_len + 1);

    size_t src_index = 0;
    if (src[0] == '\n' && src[1] == '\n') {
        ++src_index;
    }

    for (size_t res_index = 0; src_index < len; ++res_index, ++src_index) {
        if (src[src_index] == '\n') {
            while (src[src_index + 1] == '\n' && src[src_index - 1] == '\n') {
                ++src_index;
            }
            modified[res_index] = '\n';
        } else {
            modified[res_index] = src[src_index];
        }
    }

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

char* two_sym_replacement(char* src, int (*validator)(char), char first_val, char (*second_val)(char ch)) {
    int changes_count = 0;
    size_t len = strlen(src);
    for (size_t i = 0; i < len; ++i) {
        if (validator(src[i])) {
            ++changes_count;
        }
    }

    char* return_value = NULL;
    size_t new_len = len + changes_count;

    char* modified = (char *) calloc(sizeof(char), new_len + 1);
    for (size_t src_index = 0, res_index = 0; src_index < len; ++src_index, ++res_index) {
        if (validator(src[src_index])) {
            modified[res_index] = first_val;
            ++res_index;
            modified[res_index] = second_val(src[src_index]);
        } else {
            modified[res_index] = src[src_index];
        }
    }
    return_value = modified;

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

char* number_lines(char* src, int with_blank) {
    char* return_value = NULL;

    if (strcmp(src, "") == 0) {
        return_value = src;
    } else {
        size_t len = strlen(src);
        int changes_count = 0;

        if (with_blank || src[0] != '\n') {
            changes_count += 7;
        }

        for (size_t i = 0; i < len - 1; ++i) {
            if (src[i] == '\n') {
                if (with_blank || src[i + 1] != '\n') {
                    changes_count += 7;
                }
            }
        }

        size_t new_len = len + changes_count;
        char* modified = (char*) calloc(sizeof(char), new_len + 1);

        int counter = 1;
        size_t res_index = 0;
        if (with_blank || src[0] != '\n') {
            insert_counter_beginning(modified, "%6d\t\n", 0, counter);
            res_index += 7;
            ++counter;
        }

        for (size_t src_index = 0; src_index < len; ++src_index, ++res_index) {
            if (src[src_index] == '\n' && (src[src_index + 1] != '\0' && (with_blank || src[src_index + 1] != '\n'))) {
                insert_counter_beginning(modified, "\n%6d\t", res_index, counter);
                res_index += 7;
                ++counter;
            } else {
                modified[res_index] = src[src_index];
            }
        }
        return_value = modified;
    }
    return return_value;
}

int main(int argc, char** argv) {
    if (argc < 1) {
        puts("wrong arguments number");
    } else {
        FILE *file = fopen(argv[argc - 1], "r");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            rewind(file);

            char* file_content = calloc(sizeof(char), fsize + 1);
            fread(file_content, fsize, 1, file);
            fclose(file);

            shorten_flags(argv, argc - 1);
            argv = optimize_flags(argv, &argc);

            for (int i = 1; i < argc - 1; ++i) {
                size_t combo_flags_len = strlen(argv[i]);
                for (size_t j = 1; j < combo_flags_len; ++j) {
                    char* modified = apply_flag(file_content, argv[i][j]);
                    free(file_content);
                    file_content = modified;
                }
            }
            printf("%s", file_content);
            free(file_content);
            free(argv);
        }
    }


    return 0;
}
