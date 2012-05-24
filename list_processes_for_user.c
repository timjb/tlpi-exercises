/* Exercise 12-1 */

#define _BSD_SOURCE

#include <pwd.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

#define STATUS_FILE_SIZE 4000
#define CMDLINE_SIZE 2048

int isNumeric (char *);
uid_t getUIDFromProcStatus(char *, size_t);

int isNumeric (char *str) {
  while (*str != '\0') {
    if (!isdigit(*str)) { return 0; }
    str++;
  }
  return 1;
}

uid_t getUIDFromProcStatus (char *status, size_t n) {
  int sol = 1; /* start of line */
  int i = 0;
  while (i < n) {
    if (sol && strncmp(&status[i], "Uid:", 4) == 0) {
      i += 4;
      while (isspace(status[i])) { i++; }
      return (uid_t) strtol(&status[i], NULL, 10);
    }
    sol = 0;
    if (status[i] == '\n') { sol = 1; }
    i++;
  }
  return -1;
}

int main (int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s login-name\n", argv[0]);
  }

  struct passwd *pw = getpwnam(argv[1]);
  if (pw == NULL) {
    cmdLineErr("%s is not a user", argv[1]);
  }
  uid_t uid = pw->pw_uid;

  DIR *proc = opendir("/proc");
  if (proc == NULL) {
    errExit("opendir(\"/proc\")");
  }

  struct dirent *dir;
  errno = 0;
  while ((dir = readdir(proc)) != NULL) {
    if (dir->d_type == DT_DIR && isNumeric(dir->d_name)) {
      char status_path[64];
      snprintf(status_path, 64, "/proc/%s/status", dir->d_name);
      int status_file = open(status_path, O_RDONLY);
      if (status_file == -1) {
        errno = 0;
        continue;
      }

      char status[STATUS_FILE_SIZE];
      ssize_t bytesRead = read(status_file, status, STATUS_FILE_SIZE);
      if (bytesRead == -1) {
        errno = 0;
        close(status_file);
        continue;
      }

      uid_t user = getUIDFromProcStatus(status, (size_t) bytesRead);
      if (user == -1) {
        fprintf(stderr, "couldn't find uid in %s\n", status_path);
      }

      close(status_file);

      if (user == uid) {
        printf("%s", dir->d_name);

        char cmdline_path[64];
        snprintf(cmdline_path, 64, "/proc/%s/cmdline", dir->d_name);
        int cmdline_file = open(cmdline_path, O_RDONLY);
        if (cmdline_file == -1) {
          errno = 0;
          printf("\n");
          continue;
        }
        char cmdline[CMDLINE_SIZE];
        bytesRead = read(cmdline_file, cmdline, CMDLINE_SIZE);
        if (bytesRead != -1) {
          /* replace '\0's with spaces */
          for (int i = 0; i < bytesRead-1; i++) {
            if (cmdline[i] == '\0') {
              cmdline[i] = ' ';
            }
          }
          printf(": %s", cmdline);
        }
        printf("\n");
        errno = 0;
      }
    }
    errno = 0;
  }
  if (errno != 0) {
    errExit("readdir");
  }
}