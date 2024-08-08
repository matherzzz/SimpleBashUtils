#ifndef S21_CAT_H
#define S21_CAT_H
#define _GNU_SOURCE

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char reading_flags(int argc, char** argv, char* flags);
void print_file(FILE* file, char* flags);
void print_string(char* line, size_t size, const char* flags,
                  size_t* number_of_line);

#endif  // S21_CAT_H