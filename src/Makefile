CC = gcc

FLAGS = -Wall -Werror -Wextra

CAT_SRC = cat/s21_cat.c
GREP_SRC = grep/s21_grep.c

STANDARD = -std=c11

CAT_TARGET = ../bin/s21_cat
GREP_TARGET = ../bin/s21_grep

clean_cat:
	rm -f $(CAT_TARGET)

clean_grep:
	rm -f $(GREP_TARGET)

s21_cat:
	$(CC) $(FLAGS) $(STANDARD) $(CAT_SRC) -o $(CAT_TARGET)

build_cat_no_flags:
	$(CC) $(STANDARD) $(CAT_SRC) -o $(CAT_TARGET)

s21_grep:
	$(CC) $(FLAGS) $(STANDARD) $(GREP_SRC) -o $(GREP_TARGET)

build_grep_no_flags:
	$(CC) $(STANDARD) $(GREP_SRC) -o $(GREP_TARGET)

run_cat:
	./$(CAT_TARGET)

run_grep:
	./$(GREP_TARGET)

all: clean_cat clean_grep build_cat_no_flags build_grep_no_flags

