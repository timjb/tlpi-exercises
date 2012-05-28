/* Exercise 16-1: implement a setfattr-like program that can set extended attributes */

#include <unistd.h>
#include <attr/xattr.h>
#include "tlpi_hdr.h"

void usage (char *);

void usage (char *program_name) {
  usageErr("%s -n key -v value pathname\n"
           "%s -x key pathname\n",
           program_name, program_name);
}

int main (int argc, char *argv[]) {
  char mode = '\0';
  char *key;
  char *value = "";

  int opt;
  while ((opt = getopt(argc, argv, "n:x:v:")) != -1) {
    switch (opt) {
      case 'n': mode = 'n'; key = optarg; break;
      case 'x': mode = 'x'; key = optarg; break;
      case 'v': value = optarg; break;
      case '?': usage(argv[0]);
    }
  }

  if (mode == '\0' || optind >= argc) { usage(argv[0]); }

  if (mode == 'n') {
    if (setxattr(argv[optind], key, (void *) value, strlen(value), 0) == -1) {
      errExit("setxattr\n");
    }
  } else {
    if (removexattr(argv[optind], key) == -1) {
      errExit("removexattr\n");
    }
  }

  exit(EXIT_SUCCESS);
}