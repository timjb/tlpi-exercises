/* Exercise 9-4: implement initgroups */

#define _BSD_SOURCE

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include "tlpi_hdr.h"

int my_initgroups(const char *, gid_t);

int my_initgroups(const char *user, gid_t group) {
  size_t i = 0;
  gid_t groups[NGROUPS_MAX + 1];

  struct group *gr;
  while ((gr = getgrent()) != NULL) {
    char **members = gr->gr_mem;
    while (*members != NULL) {
      if (strcmp(user, *members) == 0) {
        groups[i++] = gr->gr_gid;
        break;
      }
      members++;
    }
  }
  endgrent();

  groups[i++] = group;

  return setgroups(i, groups);
}

int main (int argc, char *argv[]) {
  if (my_initgroups("tim", 42) == -1) {
    errExit("my_initgroups");
  }

  gid_t grouplist[NGROUPS_MAX + 1];
  int groups = getgroups(NGROUPS_MAX + 1, grouplist);
  if (groups == -1) {
    errExit("getgroups");
  }

  printf("Groups: ");
  int first = 1;
  for (int i = 0; i < groups; i++) {
    if (!first) { printf(", "); }
    first = 0;
    printf("%ld", (long) grouplist[i]);
  }
  printf("\n");

  exit(EXIT_SUCCESS);
}