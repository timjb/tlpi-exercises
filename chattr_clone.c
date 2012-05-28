/* Exercise 15-7: partial implementation of chattr(1) */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include "tlpi_hdr.h"

void usage (const char *);
unsigned int char2flag (char);
void chattr (char, unsigned int, const char *);

void usage (const char *progname) {
  usageErr("%s mode files...\n", progname);
}

unsigned int char2flag (char ch) {
  switch (ch) {
  case 'a': return FS_APPEND_FL;
  case 'c': return FS_COMPR_FL;
  case 'd': return FS_NODUMP_FL;
  case 'e': return FS_EXTENT_FL;
  case 'i': return FS_IMMUTABLE_FL;
  case 'j': return FS_JOURNAL_DATA_FL;
  case 's': return FS_SECRM_FL;
  case 't': return FS_NOTAIL_FL;
  case 'u': return FS_UNRM_FL;
  case 'A': return FS_NOATIME_FL;
  case 'D': return FS_DIRSYNC_FL;
  case 'S': return FS_SYNC_FL;
  case 'T': return FS_TOPDIR_FL;
  default:
    cmdLineErr("unrecognized flag: %c\n", ch);
  }
}

void chattr (char action, unsigned int eattr, const char *pathname) {
  int fd = open(pathname, O_RDONLY);
  if (fd == -1) { errExit("open"); }

  if (action == '+' || action == '-') {
    unsigned int existing_eattr;
    if (ioctl(fd, FS_IOC_GETFLAGS, &existing_eattr) == -1) {
      errExit("ioctl(fd, FS_IOC_GETFLAGS, ...) failed\n");
    }
    if (action == '+') {
      eattr |= existing_eattr;
    } else {
      eattr = existing_eattr & ~eattr;
    }
  }

  if (ioctl(fd, FS_IOC_SETFLAGS, &eattr) == -1) {
    errExit("ioctl(fd, FS_IOC_SETFLAGS, ...) failed\n");
  }

  if (close(fd) == -1) { fprintf(stderr, "close failed\n"); }
}

int main (int argc, char *argv[]) {
  if (argc < 3) { usage(argv[0]); }

  char *mode = argv[1];
  char action = *mode++;
  switch (action) {
  case '+':
  case '-':
  case '=':
    break;
  default: 
    cmdLineErr("expected mode to begin with a +, - or = sign.\n");
  }

  unsigned int eattr = 0;
  while (*mode != '\0') {
    eattr |= char2flag(*mode++);
  }

  for (int i = 2; i < argc; i++) {
    chattr(action, eattr, argv[i]);
  }

  exit(EXIT_SUCCESS);
}