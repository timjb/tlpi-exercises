/* Exercise 26-3: Replace the use of waitpid() with waitid() in Listing 26-3 (child_status.c). The call to
printWaitStatus() will need to be replaced by code that prints relevant fields from
the siginfo_t structure returned by waitid(). */

/**********************************************************************\
*                Copyright (C) Michael Kerrisk, 2010.                  *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty. See the file COPYING for details.                      *
\**********************************************************************/

/* child_status.c

   Demonstrate the use of wait() and the W* macros for analyzing the child
   status returned by wait()

   Usage: child_status [exit-status]

   If "exit-status" is supplied, then the child immediately exits with this
   status. If no command-line argument is supplied then the child loops waiting
   for signals that either cause it to stop or to terminate - both conditions
   can be detected and differentiated by the parent. The parent process
   repeatedly waits on the child until it detects that the child either exited
   normally or was killed by a signal.
*/

#define _GNU_SOURCE /* get strsignal() */

#include <sys/wait.h>
#include "print_wait_status.h"          /* Declares printWaitStatus() */
#include "tlpi_hdr.h"
#include <string.h>

int
main(int argc, char *argv[])
{
    siginfo_t childInfo;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [exit-status]\n", argv[0]);

    switch (fork()) {
    case -1: errExit("fork");

    case 0:             /* Child: either exits immediately with given
                           status or loops waiting for signals */
        printf("Child started with PID = %ld\n", (long) getpid());
        if (argc > 1)                   /* Status supplied on command line? */
            exit(getInt(argv[1], 0, "exit-status"));
        else                            /* Otherwise, wait for signals */
            for (;;)
                pause();
        exit(EXIT_FAILURE);             /* Not reached, but good practice */

    default:            /* Parent: repeatedly wait on child until it
                           either exits or is terminated by a signal */
        for (;;) {
            if (waitid(P_ALL, 0, &childInfo, WEXITED | WSTOPPED | WCONTINUED) == -1) {
                errExit("waitid\n");
            }

            /* Print status in hex, and as separate decimal bytes */

            printf("waitpid() returned: PID=%ld; UID=%ld; status=%d\n",
                    (long) childInfo.si_pid, (long) childInfo.si_uid,
                    childInfo.si_status);
            
            if (childInfo.si_code == CLD_EXITED) {
              printf("child exited with code %d\n", childInfo.si_status);
            } else if (childInfo.si_code == CLD_KILLED) {
              printf("child killed with signal %d (%s)\n", childInfo.si_status, strsignal(childInfo.si_code));
            } else if (childInfo.si_code == CLD_STOPPED) {
              printf("child stopped with signal %d (%s)\n", childInfo.si_status, strsignal(childInfo.si_code));
            } else if (childInfo.si_code == CLD_CONTINUED) {
              printf("child continued with signal %d (%s)\n", childInfo.si_status, strsignal(childInfo.si_code));
            }

            if (childInfo.si_code == CLD_EXITED || childInfo.si_code == CLD_KILLED)
                exit(EXIT_SUCCESS);
        }
    }
}
