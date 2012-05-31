/* Exercise 18-5: implement getcwd using a hacky workaround */

/* issue: doesn't work across file system boundaries */

#define _BSD_SOURCE

#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "tlpi_hdr.h"

char *my_getcwd (char *);

char *my_getcwd (char *buf) {
  struct stat stat_buf;
  char tmp_buf[PATH_MAX];
  char curr_dir[PATH_MAX] = ".";

  buf[0] = '\0';
  while (1) {
    if (stat(curr_dir, &stat_buf) == -1) {
      return NULL;
    }

    strncat(curr_dir, "/..", PATH_MAX);
    DIR *parent = opendir(curr_dir);
    if (parent == NULL) { return NULL; }
    struct dirent *entry;
    while ((entry = readdir(parent)) != NULL) {
      if (entry->d_ino == stat_buf.st_ino) {
        break;
      }
    }
    if (entry == NULL) { return NULL; }

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      break;
    }

    strncpy(tmp_buf, buf, PATH_MAX);
    snprintf(buf, PATH_MAX, "/%s%s", entry->d_name, tmp_buf);
  }

  return buf;
}

int main (int argc, char *argv[]) {
  char pwd[PATH_MAX];
  if (my_getcwd(pwd) == NULL) {
    exit(EXIT_FAILURE);
  }
  printf("%s", pwd);
  exit(EXIT_SUCCESS);
}