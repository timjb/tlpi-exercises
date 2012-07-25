/* Exercise 26-2: Suppose that we have three processes related as grandparent, parent, and child,
and that the grandparent doesn’t immediately perform a wait() after the parent
 exits, so that the parent becomes a zombie. When do you expect the grandchild to
be adopted by init (so that getppid() in the grandchild returns 1): after the parent
terminates or after the grandparent does a wait()? Write a program to verify your
answer. */

#include "tlpi_hdr.h"
#include <unistd.h>

int main (int argc, char *argv[]) {
  setbuf(stdout, NULL);

  pid_t parent, child;
  switch (parent = fork()) {
  case -1:
    errExit("fork (1)\n");
    break;
  case 0:
    switch (child = fork()) {
    case -1:
      errExit("fork (2)\n");
    case 0: /* child */
      printf("child PPID: %ld\n", (long) getppid());
      sleep(2);
      printf("child PPID: %ld\n", (long) getppid());
      break;
    default: /* parent */
      sleep(1);
      printf("parent exiting -- going into zombie state\n");
    }
    break;
  default: /* grandparent */
    sleep(3);
    printf("grandparent exiting\n");
    break;
  }
}