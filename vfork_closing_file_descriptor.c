/* Exercise 24-2: Write a program to demonstrate that after a vfork(), the child process can close a
file descriptor (e.g., descriptor 0) without affecting the corresponding file descriptor
in the parent. */

#include "tlpi_hdr.h"
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[]) {
  setbuf(stdout, NULL);

  printf("opening temp file\n");
  char temp_template[] = "vfork_testXXXXXX";
  int temp_fd = mkstemp(temp_template);
  if (temp_fd == -1) {
    errExit("mkstemp\n");
  }
  printf("seeking to position 10\n");
  if (lseek(temp_fd, 10, SEEK_CUR) == -1) {
    errExit("lseek\n");
  }

  pid_t child_pid;
  switch (child_pid = vfork()) {
  case -1:
    errExit("vfork\n");
    break;
  case 0: /* child process */
    printf("child process about to close temp file\n");
    if (close(temp_fd) == -1) {
      errExit("close\n");
    }
    _exit(0);
    break;
  default: /* parent process */
    wait(NULL);
    printf("child exited\n");
    off_t offset = lseek(temp_fd, 0, SEEK_CUR);
    if (offset == -1) {
      errExit("lseek\n");
    }
    printf("current file position: %lld\n", (long long) offset);
    exit(EXIT_SUCCESS);
    break;
  }
}