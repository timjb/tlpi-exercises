/* Exercise 21-1: Implement abort() */

#include <signal.h>
#include "tlpi_hdr.h"

void my_abort (void);
void handler (int);

void my_abort (void) {
  raise(SIGABRT);

  /* reset handling of SIGABRT */
  struct sigaction action;
  action.sa_handler = SIG_DFL;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGABRT, &action, NULL);

  raise(SIGABRT);
}

void handler (int s) {
  fprintf(stderr, "SIGABRT handler\n"); /* UNSAFE */
}

int main (int argc, char *argv[]) {
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGABRT, &action, NULL);

  fprintf(stderr, "aborting...\n");
  my_abort();
  fprintf(stderr, "abort didn't succeed!\n");
  exit(EXIT_SUCCESS);
}