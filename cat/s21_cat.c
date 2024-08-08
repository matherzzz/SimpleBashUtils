#include "s21_cat.h"

int main(int argc, char** argv) {
  char flags[6] = {0, 0, 0, 0, 0, 0};
  char correct = reading_flags(argc, argv, flags);
  while (correct && optind <= argc) {
    FILE* file = (optind == argc || !strcmp("-", argv[optind]))
                     ? stdin
                     : fopen(argv[optind], "r+");
    if (!file) {
      printf("s21_cat: %s: Failed to open file\n", argv[optind]);
    } else {
      print_file(file, flags);
      fclose(file);
    }
    optind += (optind + 1 == argc) ? 2 : 1;
  }
  return 0;
}

void print_file(FILE* file, char* flags) {
  char* line = NULL;
  size_t number_of_line = 0;
  char number_of_empty_line = 0;
  ssize_t length;
  size_t size = 0;
  while ((length = getline(&line, &size, file)) != -1) {
    number_of_empty_line =
        (flags[3] && length == 1) ? number_of_empty_line + 1 : 0;
    if (number_of_empty_line >= 2) continue;
    print_string(line, length, flags, &number_of_line);
  }
  if (line) free(line);
}

void print_string(char* line, size_t size, const char* flags,
                  size_t* number_of_line) {
  if ((flags[0] && size > 1) || flags[2]) {
    (*number_of_line)++;
    printf("%6zu\t", *number_of_line);
  }
  for (size_t i = 0; i < size - 1; i++) {
    if ((flags[4] && line[i] == '\t') ||
        (flags[5] && ((line[i] >= 0 && line[i] <= 31) || line[i] == 127) &&
         line[i] != '\t' && line[i] != '\n')) {
      printf("^");
      line[i] = (line[i] + 64) % 128;
    }
    printf("%c", line[i]);
  }
  if (flags[1] && line[size - 1] == '\n') {
    if (flags[0] && size == 1) printf("      \t");
    printf("$");
  }
  printf("%c", line[size - 1]);
}

char reading_flags(int argc, char** argv, char* flags) {
  int rez = 0;
  char correct = 1;
  const struct option long_opt[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                    {"number", no_argument, NULL, 'n'},
                                    {"squeeze-blank", no_argument, NULL, 's'},
                                    {NULL, 0, NULL, 0}};
  while ((rez = getopt_long(argc, argv, "+benstvTE", long_opt, NULL)) != -1) {
    switch (rez) {
      case 'b':
        flags[0] = 1;
        break;
      case 'e':
        flags[1] = 1;
        flags[5] = 1;
        break;
      case 'E':
        flags[1] = 1;
        break;
      case 'T':
        flags[4] = 1;
        break;
      case 'n':
        flags[2] = 1;
        break;
      case 's':
        flags[3] = 1;
        break;
      case 't':
        flags[4] = 1;
        flags[5] = 1;
        break;
      case 'v':
        flags[5] = 1;
        break;
      case '?':
        correct = 0;
        break;
    }
  }
  if (flags[0]) flags[2] = 0;
  return correct;
}