/* Exercise 20-3 (part 1): verify effect of SA_RESETHAND flag for sigaction  */

#include <signal.h>
#include "tlpi_hdr.h"

void handler (int);

int counter = 0;

void handler (int signal) {
  counter++;
}

int main (int argc, char *argv[]) {
  struct sigaction sigint_action;
  sigint_action.sa_handler = handler;
  sigemptyset(&sigint_action.sa_mask);
  sigint_action.sa_flags = 0;

  /* uncomment to see effect of SA_RESETHAND */
  sigint_action.sa_flags |= SA_RESETHAND;

  if (sigaction(SIGINT, &sigint_action, NULL) == -1) {
    errExit("sigaction\n");
  }

  for (int i = 0; i < 5; i++) {
    printf("counter: %d\n", counter);
    if (raise(SIGINT) == -1) {
      errExit("raise\n");
    }
  }
  printf("counter: %d\n", counter);

  exit(EXIT_SUCCESS);
}