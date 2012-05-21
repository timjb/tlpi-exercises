/* Exercise 8-2: implement getpwnam */

#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include "tlpi_hdr.h"

#define MAX_PASSWD_ENTRY_LEN 100

struct passwd pwnam;
char pwnam_name[MAX_PASSWD_ENTRY_LEN];
char pwnam_passwd[MAX_PASSWD_ENTRY_LEN];
char pwnam_gecos[MAX_PASSWD_ENTRY_LEN];
char pwnam_dir[MAX_PASSWD_ENTRY_LEN];
char pwnam_shell[MAX_PASSWD_ENTRY_LEN];

struct passwd *my_getpwnam(const char *);

struct passwd *my_getpwnam(const char *name) {
  if (name == NULL || *name == '\0') { return NULL; }

  struct passwd *pw;
  while ((pw = getpwent()) != NULL) {
    if (strcmp(pw->pw_name, name) == 0) {
      setpwent();
      pwnam = *pw;
      strncpy(pwnam_name, pw->pw_name, MAX_PASSWD_ENTRY_LEN);
      pwnam.pw_name = pwnam_name;
      strncpy(pwnam_passwd, pw->pw_passwd, MAX_PASSWD_ENTRY_LEN);
      pwnam.pw_passwd = pwnam_passwd;
      strncpy(pwnam_gecos, pw->pw_gecos, MAX_PASSWD_ENTRY_LEN);
      pwnam.pw_gecos = pwnam_gecos;
      strncpy(pwnam_dir, pw->pw_dir, MAX_PASSWD_ENTRY_LEN);
      pwnam.pw_dir = pwnam_dir;
      strncpy(pwnam_shell, pw->pw_shell, MAX_PASSWD_ENTRY_LEN);
      pwnam.pw_shell = pwnam_shell;
      return &pwnam;
    }
  }
  endpwent();
  return NULL;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s login-name\n", argv[0]);
  }

  errno = 0;
  struct passwd *pw = my_getpwnam(argv[1]);
  if (pw == NULL) {
    if (errno != 0) {
      errExit("my_getpwnam\n");
    } else {
      cmdLineErr("user %s doesn't exist\n", argv[1]);
    }
  }

  printf("Login name:         %s\n", pw->pw_name);
  printf("Encrypted password: %s\n", pw->pw_passwd);
  printf("User ID:            %ld\n", (long) pw->pw_uid);
  printf("Group ID:           %ld\n", (long) pw->pw_gid);
  printf("Comment:            %s\n", pw->pw_gecos);
  printf("Working directory:  %s\n", pw->pw_dir);
  printf("Login shell:        %s\n", pw->pw_shell);

  exit(EXIT_SUCCESS);
}