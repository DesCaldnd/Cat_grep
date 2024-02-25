#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef unsigned char uc;
typedef unsigned int ui;

int get_flag_type(char *str, uc *state);
int isDir(const char *fileName);

void work(FILE *in, uc state);

ui need_nonblank(uc state);
ui need_number(uc state);
ui need_endl(uc state);
ui need_squeeze(uc state);
ui need_tabs(uc state);

int main(int argc, char *argv[]) {
  size_t filecount = 0;
  uc state = 0;

  for (int i = 1; i < argc; ++i) {
    if (get_flag_type(argv[i], &state) == 0) {
      ++filecount;
    }
  }

  if (filecount == 0) {
    work(stdin, state);
  } else {
    for (int i = 1; i < argc; ++i) {
      if (get_flag_type(argv[i], &state) == 0) {
        if (isDir(argv[i])) {
          printf("Is a directory\n");
          continue;
        }
        FILE *in = fopen(argv[i], "r");
        if (in == NULL) {
          printf("No such file or directory\n");
          continue;
        }

        work(in, state);

        fclose(in);
      }
    }
  }
  return 0;
}

int isDir(const char *fileName) {
  struct stat path;

  stat(fileName, &path);

  return S_ISDIR(path.st_mode);
}

int get_flag_type(char *str, uc *state) {
  int ret = 1;

  if (strcmp(str, "-b") == 0 || strcmp(str, "--number-nonblank") == 0) {
    *state |= 1u << 3;
  } else if (strcmp(str, "-e") == 0 || strcmp(str, "-E") == 0) {
    *state |= 1u;
  } else if (strcmp(str, "-n") == 0 || strcmp(str, "--number") == 0) {
    *state |= 1u << 3;
    *state |= 1u << 4;
  } else if (strcmp(str, "-s") == 0 || strcmp(str, "--squeeze-blank") == 0) {
    *state |= 1u << 1;
  } else if (strcmp(str, "-t") == 0 || strcmp(str, "-T") == 0) {
    *state |= 1u << 2;
  } else
    ret = 0;

  return ret;
}

ui need_nonblank(uc state) { return state & (1u << 3); }

ui need_number(uc state) { return state & (1u << 4); }

ui need_endl(uc state) { return state & (1u); }

ui need_squeeze(uc state) { return state & (1u << 1); }

ui need_tabs(uc state) { return state & (1u << 2); }

void work(FILE *in, uc state) {
  char c = getc(in);
  static size_t string_num = 1;
  int has_char = 0;

  while (!feof(in)) {
    if (c == '\r') goto next;
    if (!has_char) {
      if ((c != '\n' && need_nonblank(state)) ||
          (need_number(state) && !need_squeeze(state))) {
        printf("%6zu  ", string_num++);
      }
      if (c == '\n' && need_squeeze(state)) goto next;
    }
    has_char = 1;
    if (c == '\t' && need_tabs(state))
      printf("^I\t");
    else if (c == '\n') {
      if (need_endl(state)) putchar('$');
      putchar('\n');
      has_char = 0;
    } else {
      putchar(c);
    }

  next:
    c = getc(in);
  }
}
