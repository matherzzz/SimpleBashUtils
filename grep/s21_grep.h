#ifndef S21_GREP_H
#define S21_GREP_H
#define _GNU_SOURCE

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char reading_flags(int argc, char** argv, char* opt, char*** e_regex,
                   size_t* size_e);
char match(char* string, char* pattern, int flag_icase, regmatch_t* pmatch,
           int eflag);
int compare_regmatch(const void* r1, const void* r2);
void print_string(char* str, size_t size);
void print_matches(FILE* file, char* flags, char flag_few_files, char** argv,
                   char** regex, size_t size_regex);
void add_regex(size_t* number_of_regex, size_t* size_regex,
               char*** array_of_regex, char* regex);
void print_match(char* line, regoff_t start, regoff_t end);
void read_regular_expressions(FILE* file, size_t* number_of_regex, size_t* size,
                              char*** array_of_regex, char flag_o,
                              char* flag_empty, char* correct);
void update_flags(size_t cond, char* correct, char* flag_empty, char flag_o);
void find_matches(regmatch_t** matches, size_t* number_of_matches, char* line,
                  char** array_of_regex, size_t size_array_of_regex,
                  char flag_o, char flag_i, char* output_flag);

#endif  // S21_GREP_H