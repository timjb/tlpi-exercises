/* Exercise 23-1: Implement alarm() using setitimer() */

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include "tlpi_hdr.h"

unsigned int my_alarm (unsigned int);
void handler (int);

unsigned int my_alarm (unsigned int secs) {
  struct itimerval new, old;
  new.it_value.tv_sec = (time_t) secs;
  new.it_value.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &new, &old);
  return (unsigned int) old.it_value.tv_sec;
}

void handler (int sig) {
  printf("Got alarm\n"); /* UNSAFE */
}

int main (int argc, char *argv[]) {
  struct sigaction alarm_action;
  sigemptyset(&alarm_action.sa_mask);
  alarm_action.sa_flags = 0;
  alarm_action.sa_handler = handler;
  if (sigaction(SIGALRM, &alarm_action, NULL) == -1) {
    errExit("sigaction");
  }

  my_alarm(3);
  printf("%u\n", my_alarm(3));
  pause();
  my_alarm(2);
  pause();
  my_alarm(1);
  pause();

  exit(EXIT_SUCCESS);
}