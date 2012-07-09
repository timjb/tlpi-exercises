/* Exercise 22-2 */

#define _GNU_SOURCE

#include "tlpi_hdr.h"
#include <signal.h>
#include <string.h>

void handler (int, siginfo_t *, void *);

static volatile int gotSigint = 0;

void handler (int signal, siginfo_t *si, void *ctx) {
  if (signal == SIGINT) {
    gotSigint = 1;
    return;
  }
  /* UNSAFE */
  printf("caught %d (%s)\n", signal, strsignal(signal));
}

int main (int argc, char *argv[]) {
  printf("%s: pid is %ld\n", argv[0], (long) getpid());

  struct sigaction action;
  sigfillset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = handler;
  for (int n = 1; n < NSIG; n++) {
    if (n == SIGTSTP || n == SIGQUIT) { continue; }
    if (sigaction(n, &action, NULL) == -1) {
      /*errExit("sigaction\n");*/
    }
  }

  sigset_t blocked, prevMask;
  sigfillset(&blocked);
  sigdelset(&blocked, SIGINT);
  sigdelset(&blocked, SIGTERM);
  printf("blocking all signals\n");
  if (sigprocmask(SIG_SETMASK, &blocked, &prevMask) == -1) {
    errExit("sigprocmask\n");
  }

  int sleepTime = 15;
  printf("sleeping for %d seconds\n", sleepTime);
  sleep(sleepTime);

  printf("unblocking all signals\n");
  if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) {
    errExit("sigprocmask\n");
  }

  while (!gotSigint) {
    pause();
  }

  exit(EXIT_SUCCESS);
}