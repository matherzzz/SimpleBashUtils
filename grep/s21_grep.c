#include "s21_grep.h"

int main(int argc, char** argv) {
  char flags[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char flag_few_files = 0;
  char** array_of_regex = NULL;
  size_t size_array_of_regex = 0;
  char correct =
      reading_flags(argc, argv, flags, &array_of_regex, &size_array_of_regex);
  if (correct && !size_array_of_regex) {
    array_of_regex = malloc(sizeof(char*));
    *array_of_regex = malloc(sizeof(char) * (strlen(argv[optind]) + 1));
    *array_of_regex = strcpy(*array_of_regex, argv[optind++]);
    size_array_of_regex++;
  }
  if (argc > optind + 1) flag_few_files = 1;
  while (correct && optind < argc) {
    FILE* file = fopen(argv[optind], "r+");
    if (!file) {
      if (!flags[7])
        printf("s21_grep: %s: Failed to open file\n", argv[optind]);
    } else {
      print_matches(file, flags, flag_few_files, argv, array_of_regex,
                    size_array_of_regex);
      fclose(file);
    }
    optind++;
  }
  if (array_of_regex) {
    for (size_t i = 0; i < size_array_of_regex; i++) {
      if (array_of_regex[i]) free(array_of_regex[i]);
    }
    free(array_of_regex);
  }
}

void print_matches(FILE* file, char* flags, char flag_few_files, char** argv,
                   char** array_of_regex, size_t size_array_of_regex) {
  char* line = NULL;
  ssize_t length;
  size_t size_line = 0;
  size_t number_of_matching_lines = 0;
  size_t number_of_lines = 0;
  char flag_l = 0;
  if (flags[1]) flags[1] = REG_ICASE;
  while (!flag_l && (length = getline(&line, &size_line, file)) != -1) {
    regmatch_t* matches = NULL;
    size_t number_of_matches = 0;
    char output_flag = 0;
    number_of_lines++;
    find_matches(&matches, &number_of_matches, line, array_of_regex,
                 size_array_of_regex, flags[9], flags[1], &output_flag);
    qsort((void*)matches, number_of_matches, sizeof(regmatch_t),
          compare_regmatch);
    for (size_t i = 0; i < number_of_matches; i++) {
      if (i == 0 || matches[i].rm_so >= matches[i - 1].rm_eo) {
        if (flags[5]) printf("%zu:", number_of_lines);
        print_match(line, matches[i].rm_so, matches[i].rm_eo);
      }
    }
    if (flags[2]) output_flag = !output_flag;
    if (flags[3] && output_flag) number_of_matching_lines++;
    if (flags[4] && output_flag) flag_l = 1;
    if (output_flag && !flags[4] && !flags[3] && !flags[9]) {
      if (flag_few_files && !flags[6]) printf("%s:", argv[optind]);
      if (flags[5]) printf("%zu:", number_of_lines);
      print_string(line, length);
    }
    if (matches) free(matches);
  }
  if (flags[3]) {
    if (flag_few_files && !flags[6]) printf("%s:", argv[optind]);
    printf("%zu\n", number_of_matching_lines);
  }
  if (flags[4] && flag_l) printf("%s\n", argv[optind]);
  if (line) free(line);
}

int compare_regmatch(const void* r1, const void* r2) {
  regoff_t delta_so = (*(regmatch_t*)r1).rm_so - (*(regmatch_t*)r2).rm_so;
  regoff_t delta_eo = (*(regmatch_t*)r1).rm_eo - (*(regmatch_t*)r2).rm_eo;
  return delta_so ? delta_so : !delta_eo;
}

void add_regex(size_t* number_of_regex, size_t* size_regex,
               char*** array_of_regex, char* regex) {
  if (++(*number_of_regex) == 1) *array_of_regex = malloc(sizeof(char*));
  if (*number_of_regex > *size_regex) {
    *size_regex *= 2;
    *array_of_regex = realloc(*array_of_regex, *size_regex * sizeof(char*));
  }
  (*array_of_regex)[*number_of_regex - 1] =
      malloc(sizeof(char) * (strlen(regex) + 1));
  (*array_of_regex)[*number_of_regex - 1] =
      strcpy((*array_of_regex)[*number_of_regex - 1], regex);
}

void print_match(char* line, regoff_t start, regoff_t end) {
  for (regoff_t j = 0; j < (end - start); j++) {
    printf("%c", line[start + j]);
  }
  printf("\n");
}

void find_matches(regmatch_t** matches, size_t* number_of_matches, char* line,
                  char** array_of_regex, size_t size_array_of_regex,
                  char flag_o, char flag_i, char* output_flag) {
  size_t size_matches = 1;
  regmatch_t pmatch;
  for (size_t i = 0; i < size_array_of_regex; i++) {
    int eflag = 0;
    char* ptr = line;
    regoff_t prev_rm_eo = 0;
    if (!strcmp(array_of_regex[i], "") ||
        (!flag_o && match(line, array_of_regex[i], flag_i, &pmatch, eflag)))
      *output_flag = 1;
    while (flag_o && match(ptr, array_of_regex[i], flag_i, &pmatch, eflag)) {
      ptr += pmatch.rm_eo;
      eflag = REG_NOTBOL;
      if (++(*number_of_matches) == 1) *matches = malloc(sizeof(regmatch_t));
      if (*number_of_matches > size_matches) {
        size_matches *= 2;
        *matches = realloc(*matches, size_matches * sizeof(regmatch_t));
      }
      pmatch.rm_eo += prev_rm_eo;
      pmatch.rm_so += prev_rm_eo;
      prev_rm_eo = pmatch.rm_eo;
      (*matches)[*number_of_matches - 1] = pmatch;
    }
  }
}

char reading_flags(int argc, char** argv, char* flags, char*** array_of_regex,
                   size_t* number_of_regex) {
  int rez = 0;
  char correct = 1;
  size_t buff = 1;
  char flag_empty = 0;
  while (correct &&
         (rez = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (rez) {
      case 'e':
        add_regex(number_of_regex, &buff, array_of_regex, optarg);
        update_flags(strlen(optarg), &correct, &flag_empty, flags[9]);
        break;
      case 'f':
        FILE* file = fopen(optarg, "r+");
        if (!file) {
          correct = 0;
        } else {
          read_regular_expressions(file, number_of_regex, &buff, array_of_regex,
                                   flags[9], &flag_empty, &correct);
          fclose(file);
        }
        break;
      case 'i':
        flags[1] = 1;
        break;
      case 'v':
        flags[2] = 1;
        break;
      case 'c':
        flags[3] = 1;
        break;
      case 'l':
        flags[4] = 1;
        break;
      case 'n':
        flags[5] = 1;
        break;
      case 'h':
        flags[6] = 1;
        break;
      case 's':
        flags[7] = 1;
        break;
      case 'o':
        flags[9] = 1;
        if (flag_empty) correct = 0;
        break;
      case '?':
        correct = 0;
        break;
    }
  }
  if (flags[2] || flags[3] || flags[4]) flags[9] = 0;
  return correct;
}

void read_regular_expressions(FILE* file, size_t* number_of_regex, size_t* size,
                              char*** array_of_regex, char flag_o,
                              char* flag_empty, char* correct) {
  char* regex = NULL;
  size_t size_regex;
  ssize_t length;
  while ((length = getline(&regex, &size_regex, file)) != -1) {
    if (regex[length - 1] == '\n') regex[length - 1] = '\0';
    add_regex(number_of_regex, size, array_of_regex, regex);
    update_flags(strlen(regex), correct, flag_empty, flag_o);
  }
  if (regex) free(regex);
}

void update_flags(size_t cond, char* correct, char* flag_empty, char flag_o) {
  if (!cond) {
    *correct = !flag_o;
    *flag_empty = 1;
  }
}

void print_string(char* str, size_t size) {
  printf("%s", str);
  if (str[size - 1] != '\n') printf("\n");
}

char match(char* string, char* pattern, int flag_icase, regmatch_t* pmatch,
           int eflag) {
  int status;
  regex_t re;
  int nmatch = (pmatch) ? 1 : 0;
  if (!(status = regcomp(&re, pattern, flag_icase)))
    status = regexec(&re, string, nmatch, pmatch, eflag);
  regfree(&re);
  char flag = (status != 0) ? 0 : 1;
  return flag;
}