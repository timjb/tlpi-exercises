/* Exercise 20-2: show that SIG_IGN makes the process ignore a signal */

#define _GNU_SOURCE

#include "tlpi_hdr.h"
#include "signal_functions.c"
#include <signal.h>
#include <limits.h>

int main (int argc, char *argv[]) {
  sigset_t sigint_set;
  sigemptyset(&sigint_set);
  sigaddset(&sigint_set, SIGINT);

  /* temporarily disable SIGINT delivery */
  if (sigprocmask(SIG_BLOCK, &sigint_set, NULL) == -1) {
    errExit("sigprocmask\n");
  }

  /* send SIGINT -- uncomment to see the effect */
  if (raise(SIGINT) != 0) {
    errExit("raise\n");
  }

  /* print pending signals */
  sigset_t pending;
  if (sigpending(&pending) == -1) {
    errExit("sigpending\n");
  }
  printSigset(stdout, "pending signal: ", &pending);

  /* ignore SIGINT */
  struct sigaction ign_action;
  ign_action.sa_handler = SIG_IGN;
  sigemptyset(&ign_action.sa_mask);
  ign_action.sa_flags = 0;
  if (sigaction (SIGINT, &ign_action, NULL) == -1) {
    errExit("sigaction\n");
  }

  /* reenable SIGINT */
  if (sigprocmask(SIG_UNBLOCK, &sigint_set, NULL) == -1) {
    errExit("sigprocmask\n");
  }

  /* sleep for one second */
  sleep(1);

  printf("Still running, not terminated by SIGINT.\n");

  exit(EXIT_SUCCESS);
}