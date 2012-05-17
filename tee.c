/* Exercise 4-1 */

#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main (int argc, char *argv[]) {
  Boolean append = FALSE;

  int opt;
  while ((opt = getopt(argc, argv, "a")) != -1) {
    if ((unsigned char) opt == 'a') { append = TRUE; }
  }

  if (optind >= argc) {
    usageErr("%s [-a] file\n", argv[0]);
  }

  int fd = open(
    argv[optind],
    O_CREAT | O_WRONLY | (append ? O_APPEND : O_TRUNC),
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH /* rw-rw-rw- */
  );
  if (fd == -1) { errExit("opening file %s", argv[optind]); }

  ssize_t numRead;
  char buf[BUF_SIZE];
  while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (write(fd, buf, numRead) != numRead) {
      errExit("couldn't write whole buffer!");
    }
    if (write(STDOUT_FILENO, buf, numRead) != numRead) {
      errExit("couldn't write whole buffer!");
    }
  }
  if (numRead == -1) {
    errExit("read");
  }
  if (close(fd) == -1) {
    errExit("close input");
  }
}