all: s21_grep

s21_grep: s21_grep.c
	gcc -std=c11 -Wall -Werror -Wextra s21_grep.c -o ./s21_grep

rebuild: clean all

clean:
	rm -rf ./s21_grep