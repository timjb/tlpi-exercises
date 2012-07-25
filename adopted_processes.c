/* Exercise 26-1: Write a program to verify that when a child’s parent terminates, a call to getppid()
 returns 1 (the process ID of init). */

#include "tlpi_hdr.h"
#include <unistd.h>

int main (int argc, char *argv[]) {
  setbuf(stdout, NULL);

  pid_t pid;
  switch (pid = fork()) {
  case -1:
    errExit("fork\n");
    break;
  case 0:
    printf("child started with PID %ld and parent %ld\n", (long) getpid(), (long) getppid());
    sleep(3);
    printf("child's parent: %ld\n", (long) getppid());
    exit(0);
  default:
    sleep(1);
    printf("parent exiting\n");
    exit(0);
  }
}