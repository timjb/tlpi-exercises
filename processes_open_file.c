/* Exercise 12-3: list processes that have a particular file opened (requires sudo) */

#define _BSD_SOURCE

#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include "tlpi_hdr.h"

int isNumeric (char *);

int isNumeric (char *str) {
  while (*str != '\0') {
    if (!isdigit(*str)) { return 0; }
    str++;
  }
  return 1;
}

int main (int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file\n", argv[0]);
  }

  char *file = argv[1];
  size_t file_len = strlen(file) + 1;
  char *other_file = (char *) malloc(file_len);
  if (other_file == NULL) { errExit("malloc\n"); }

  DIR *proc = opendir("/proc");
  if (proc == NULL) { errExit("opendir(\"/proc\")\n"); }

  struct dirent *proc_ent;
  errno = 0;
  while ((proc_ent = readdir(proc)) != NULL) {
    if (!(proc_ent->d_type == DT_DIR && isNumeric(proc_ent->d_name))) { continue; }

    char fd_path[32];
    snprintf(fd_path, 32, "/proc/%s/fd", proc_ent->d_name);
    DIR *fd_dir = opendir(fd_path);
    if (fd_dir == NULL) { errExit("opendir(\"%s\")\n", fd_path); }
    struct dirent *fd_ent;
    while ((fd_ent = readdir(fd_dir)) != NULL) {
      if (fd_ent->d_type != DT_LNK) { continue; }

      char full_fd_path[48];
      snprintf(full_fd_path, 48, "%s/%s", fd_path, fd_ent->d_name);

      ssize_t bytesRead = readlink(full_fd_path, other_file, file_len);
      if (bytesRead == -1) { errExit("readlink(\"%s\")\n", full_fd_path); }
      if (bytesRead < file_len) {
        other_file[bytesRead] = '\0';
      }
      if (strcmp(file, other_file) == 0) {
        /* print pid of process that has the file open */
        printf("%s\n", proc_ent->d_name);
      }
    }
  }
  if (errno != 0) { /* TODO */ }

  exit(EXIT_SUCCESS);
}