CC = gcc

FLAGS = -Wall -Werror -Wextra

GREP_SRC = s21_grep.c

STANDARD = -std=c11

GREP_TARGET = s21_grep

all: clean_grep s21_grep

clean_grep:
	rm -f $(GREP_TARGET)

s21_grep:
	$(CC) $(FLAGS) $(STANDARD) $(GREP_SRC) -D _GNU_SOURCE -o $(GREP_TARGET)

build_grep_no_flags:
	$(CC) $(STANDARD) $(GREP_SRC) -o $(GREP_TARGET)

run_grep:
	./$(GREP_TARGET)
