/* Exercise 15-4: implement eaccess (execute this program with sudo) */

#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include "tlpi_hdr.h"

#define ROOT_UID 0

#ifndef OTHER_UID
#define OTHER_UID 1000
#endif

int my_eaccess (const char *, int);
int setresuid_checked (uid_t, uid_t, uid_t);

int my_eaccess (const char *pathname, int mode) {
  uid_t ruid = getuid();
  uid_t euid = geteuid();
  setresuid(euid, -1, -1);
  gid_t rgid = getgid();
  gid_t egid = getegid();
  setresgid(egid, -1, -1);

  int a = access(pathname, mode);

  setresuid(ruid, -1, -1);
  setresgid(rgid, -1, -1);

  return a;
}

int setresuid_checked  (uid_t ruid, uid_t euid, uid_t suid) {
  if (setresuid(ruid, euid, suid) == -1) {
    fatal("coulnd't set real, effective and saved user ids to %ld, %ld and "
          "%ld respectively.\n", (long) ruid, (long) euid, (long) suid);
  }
  return 0;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s testfile\n", argv[0]);
  }

  char *filename = argv[1];

  setresuid_checked(ROOT_UID, ROOT_UID, ROOT_UID);

  mode_t mode = S_IRUSR | S_IWUSR;
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
  if (fd == -1) {
    fatal("couln't create file %s\n", filename);
  }
  close(fd);
  int access_mode = W_OK | R_OK;

  assert(access(filename, access_mode) == 0);
  assert(my_eaccess(filename, access_mode) == 0);

  setresuid_checked(OTHER_UID, OTHER_UID, -1);
  assert(access(filename, access_mode) == -1);
  assert(my_eaccess(filename, access_mode) == -1);
  
  setresuid_checked(ROOT_UID, OTHER_UID, -1);
  assert(access(filename, access_mode) == 0);
  assert(my_eaccess(filename, access_mode) == -1);
  
  setresuid_checked(OTHER_UID, ROOT_UID, -1);
  assert(access(filename, access_mode) == -1);
  assert(my_eaccess(filename, access_mode) == 0);

  exit(EXIT_SUCCESS);
}