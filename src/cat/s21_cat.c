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
    } else if (flag == 'e') {
        return_value = two_sym_replacement(src, e_flag_validator, '$', e_flag_second_value);
    } else if (flag == 'n') {
        return_value = number_lines(src, 1);
    } else if (flag == 's') {
        return_value = apply_s_flag(src);
    } else if (flag == 't') {
        return_value = two_sym_replacement(src, t_flag_validator, '^', t_flag_second_value);
    }

    return return_value;
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
    char* modified = (char*) calloc(sizeof(char), new_len);

    int src_index = 0;
    if (src[0] == '\n' && src[1] == '\n') {
        ++src_index;
    }

    for (int res_index = 0; src_index < len - 1; ++res_index, ++src_index) {
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
char t_flag_second_value(char ch) { return 'I'; }

int e_flag_validator(char ch) { return ch == '\n'; }
char e_flag_second_value(char ch) { return '\n'; }

int v_flag_validator(char ch) {
    int return_value = 0;
    if ((ch >= 0 && ch <= 31 || ch == 127) &&
        (ch != '\n' && ch != '\f' && ch != '\t')) {
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
    if (new_len == len) {
        return_value = src;
    } else {
        char* modified = (char *) calloc(sizeof(char), new_len + 1);
        for (size_t src_index = 0, res_index = 0; src_index < len - 1; ++src_index, ++res_index) {
            if (validator(src[src_index])) {
                modified[res_index] = first_val;
                ++res_index;
                modified[res_index] = second_val(src[src_index]);
            } else {
                modified[res_index] = src[src_index];
            }
        }
        return_value = modified;
    }

    return return_value;
}

void insert_counter_beginning(char* src, const char* format, size_t ind, int num) {
    char* new_substr = (char*) calloc(sizeof(char), 9);
    sprintf(new_substr, format, num);
    strcpy(&src[ind], new_substr);
    free(new_substr);
}

char* number_lines(char* src, int with_blank) {
    char* return_value = NULL;

    if (strcmp(src, "") == 0) {
        return_value = src;
    } else {
        size_t len = strlen(src);
        int changes_count = 0;
        for (int i = 0; i < len - 1; ++i) {
            if (src[i] == '\n') {
                if (with_blank || src[i + 1] != '\n' && i != 0) {
                    changes_count += 7;
                }
            }
        }

        size_t new_len = len + changes_count;
        char* modified = (char*) calloc(sizeof(char), new_len);

        int counter = 1;
        size_t res_index = 0;
        if (with_blank || src[0] != '\n') {
            insert_counter_beginning(modified, "%6d\t\n", 0, counter);
            res_index += 7;
            ++counter;
        }

        for (size_t src_index = 0; src_index < len - 1; ++src_index, ++res_index) {
            if (src[src_index] == '\n') {
                if (with_blank || src[src_index] != '\n') {
                    insert_counter_beginning(modified, "\n%6d\t", res_index, counter);
                    res_index += 7;
                    ++counter;
                }
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
        if (file == NULL) {
            puts("file opening error");
        } else {
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            rewind(file);

            char* file_content = calloc(sizeof(char), fsize + 1);
            fread(file_content, fsize, 1, file);
            fclose(file);

            for (int i = 1; i < argc - 1; ++i) {
                size_t combo_flags_len = strlen(argv[i]);
                for (size_t j = 1; j < combo_flags_len; ++j) {
                    char* modified = apply_flag(file_content, argv[i][j]);
                    free(file_content);
                    file_content = modified;
                }
            }
            puts(file_content);
            free(file_content);
        }
    }


    return 0;
}
