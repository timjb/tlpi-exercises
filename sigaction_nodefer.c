/* Exercise 20-3 (part 2): verify effect of NO_DEFER flag for sigaction  */

#include <signal.h>
#include "tlpi_hdr.h"

void handler (int);

int nesting_level;
int exited;

void handler (int signal) {
  if (exited || nesting_level > 5) { return; }
  nesting_level++;
  if (raise(SIGINT) == -1) {
    errExit("raise\n");
  }
  exited = 1;
}

int main (int argc, char *argv[]) {
  printf("Without SA_NODEFER\n");

  struct sigaction sigint_action;
  sigint_action.sa_handler = handler;
  sigemptyset(&sigint_action.sa_mask);
  sigint_action.sa_flags = 0;

  if (sigaction(SIGINT, &sigint_action, NULL) == -1) {
    errExit("sigaction\n");
  }

  exited = 0;
  nesting_level = 0;
  if (raise(SIGINT) == -1) {
    errExit("raise\n");
  }
  printf("  nesting_level: %d\n", nesting_level);

  printf("With SA_NODEFER:\n");

  sigint_action.sa_flags |= SA_NODEFER;

  if (sigaction(SIGINT, &sigint_action, NULL) == -1) {
    errExit("sigaction\n");
  }

  exited = 0;
  nesting_level = 0;
  if (raise(SIGINT) == -1) {
    errExit("raise\n");
  }
  printf("  nesting_level: %d\n", nesting_level);

  exit(EXIT_SUCCESS);
}