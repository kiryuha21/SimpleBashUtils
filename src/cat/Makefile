CC = gcc

FLAGS = -Wall -Werror -Wextra

CAT_SRC = s21_cat.c

STANDARD = -std=c11

CAT_TARGET = s21_cat

all: clean_cat s21_cat

clean_cat:
	rm -f $(CAT_TARGET)

s21_cat:
	$(CC) $(FLAGS) $(STANDARD) $(CAT_SRC) -o $(CAT_TARGET)

build_cat_no_flags:
	$(CC) $(STANDARD) $(CAT_SRC) -o $(CAT_TARGET)

run_cat:
	./$(CAT_TARGET)
