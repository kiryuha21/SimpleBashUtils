CC = gcc

FLAGS = -Wall -Werror -Wextra

SRC = s21_cat.c

STANDARD = -std=c11

TARGET = s21_cat

clean:
	rm $(TARGET)

build:
	$(CC) $(FLAGS) $(STANDARD) $(SRC) -o $(TARGET)

build-no-flags:
	$(CC) $(STANDARD) $(SRC) -o $(TARGET)

run:
	./$(TARGET)

all: build-no-flags run

