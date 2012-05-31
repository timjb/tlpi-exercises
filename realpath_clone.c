/* Exercise 18-3: implement realpath */

#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

char *my_realpath_relative (const char *, char *);
char *my_realpath(const char *, char *);

char *my_realpath_relative (const char *path, char *relative_path) {
  if (path[0] == '/') {
    relative_path[0] = '/';
    relative_path[1] = '\0';
  }

  int i = 0, j = 0;
  while (path[i] != '\0') {
    while (path[i] == '/') { i++; }
    j = i;
    while (path[j] != '/' && path[j] != '\0') {
      j++;
    }
    if (j > i) {
      if (j - i == 1 && path[i] == '.') {
        /* do nothing */
      } else if (j - i == 2 && path[i] == '.' && path[i+1] == '.') {
        /* go one directory up */
        char *dirn = dirname(relative_path);
        strncpy(relative_path, dirn, PATH_MAX);
      } else {
        if (relative_path[strlen(relative_path)-1] != '/') {
          strncat(relative_path, "/", PATH_MAX);
        }
        size_t relative_path_len = strlen(relative_path);
        strncat(relative_path, &path[i], j-i+1);
        struct stat stat_buf;
        if (lstat(relative_path, &stat_buf) == -1) { return NULL; }
        if (S_ISLNK(stat_buf.st_mode)) {
          char buf[PATH_MAX];
          ssize_t len = readlink(relative_path, buf, PATH_MAX);
          if (len == -1) { return NULL; }
          relative_path[relative_path_len] = '\0';
          if (my_realpath_relative(buf, relative_path) == NULL) {
            return NULL;
          }
        }
      }
    }
    i = j;
  }

  return relative_path;
}

char *my_realpath(const char *path, char *resolved_path) {
  if (path == NULL) {
    errno = EINVAL;
    return NULL;
  }

  if (resolved_path == NULL) {
    resolved_path = (char *) malloc(PATH_MAX);
  }

  if (path[0] != '/') {
    if (getcwd(resolved_path, PATH_MAX) == NULL) {
      return NULL;
    }
    strncat(resolved_path, "/", PATH_MAX);
  }

  return my_realpath_relative(path, resolved_path);
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s path", argv[0]);
  }
  char *path = argv[1];
  char resolved_path[PATH_MAX];
  if (my_realpath(path, resolved_path) == resolved_path) {
    printf("%s", resolved_path);
  } else {
    errExit("my_realpath\n");
  }
  exit(EXIT_SUCCESS);
}