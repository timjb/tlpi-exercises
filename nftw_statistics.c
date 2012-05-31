/* Exercise 18-7: walk a directory tree and gather various statistics */

#define _BSD_SOURCE /* file type test macros */

#include <ftw.h>
#include "tlpi_hdr.h"

int walker (const char *, const struct stat *, int, struct FTW *);

int max_depth = 0;
int directories = 0;
int unreadable_directories = 0;
int normal_files = 0;
int symbolic_links = 0;
int broken_links = 0;
int other_files = 0;
int stat_failed = 0;

int walker (const char *fpath, const struct stat *sb,
            int typeflag, struct FTW *ftwbuf) {
  max_depth = max(max_depth, ftwbuf->level);

  if (sb == NULL) {
    stat_failed++;
    return 0;
  }

  if (S_ISDIR(sb->st_mode)) {
    directories++;
    if (typeflag & FTW_DNR) { unreadable_directories++; }
  } else if (S_ISREG(sb->st_mode)) {
    normal_files++;
  } else if (S_ISLNK(sb->st_mode)) {
    symbolic_links++;
    if (typeflag & FTW_SL) { broken_links++; }
  } else {
    other_files++;
  }

  return 0;
}

int main (int argc, char *argv[]) {
  if (nftw(".", walker, 10, FTW_PHYS) != 0) {
    errExit("tree walk failed\n");
  }
  printf("Maximum depth:  %d\n", max_depth);
  printf("Directories:    %d\n", directories);
  printf(" - unreadable:  %d\n", unreadable_directories);
  printf("Regular files:  %d\n", normal_files);
  printf("Symbolic links: %d\n", symbolic_links);
  printf(" - broken:      %d\n", broken_links);
  printf("Other files:    %d\n", other_files);
  printf("Stat failed:    %d\n", stat_failed);
  exit(EXIT_SUCCESS);
}