/* Exercise 15-6: implement `chmod a+rX` using stat(2) and chmod(2) */

#include <sys/stat.h>
#include "tlpi_hdr.h"

void chmod_a_plus_rX (const char *);

void chmod_a_plus_rX (const char *filename) {
  struct stat stats;
  if (stat(filename, &stats) == -1) {
    fatal("couldn't get file stats\n");
  }

  mode_t curr_mode = stats.st_mode;
  mode_t new_mode = curr_mode | S_IRUSR | S_IRGRP | S_IROTH;

  if (S_ISDIR(curr_mode) || curr_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
    new_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
  }

  if (chmod(filename, new_mode) == -1) {
    fatal("couln't set new file permissions\n");
  }
}

int main (int argc, char *argv[]) {
  if (argc < 2) {
    usageErr("%s file\n", argv[0]);
  }

  for (int i = 1; i < argc; i++) {
    chmod_a_plus_rX(argv[i]);
  }

  exit(EXIT_SUCCESS);
}