/* Exercise 22-1 */

#include "tlpi_hdr.h"
#include <signal.h>

void cont_handler (int);

void cont_handler (int signal) {
  printf("caught SIGCONT\n"); /* UNSAFE */
}

int main (int argc, char *argv[]) {
  struct sigaction cont_action;
  sigemptyset(&cont_action.sa_mask);
  cont_action.sa_flags = 0;
  cont_action.sa_handler = cont_handler;
  if (sigaction(SIGCONT, &cont_action, NULL) == -1) {
    errExit("sigaction\n");
  }

  sigset_t blocked;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGCONT);
  printf("blocking SIGCONT\n");
  if (sigprocmask(SIG_BLOCK, &blocked, NULL) == -1) {
    errExit("sigprocmask\n");
  }

  int sleepTime = 15;
  printf("sleeping for %d seconds\n", sleepTime);
  sleep(sleepTime);

  printf("unblocking SIGCONT\n");
  if (sigprocmask(SIG_UNBLOCK, &blocked, NULL) == -1) {
    errExit("sigprocmask\n");
  }

  exit(EXIT_SUCCESS);
}