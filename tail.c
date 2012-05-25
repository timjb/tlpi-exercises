/*
  Exercise 13-5: implement tail efficiently

  deficencies:
  * can fail when the file is modified concurrently
  * assumes that a file ends with '\n'
*/

#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main (int argc, char *argv[]) {
  int n = 10;

  int opt;
  while ((opt = getopt(argc, argv, "n:")) != -1) {
    if (opt == 'n') {
      n = getLong(optarg, GN_GT_0, "n");
    }
  }

  n++; /* account for \n at the end of the file */

  if (optind == argc) {
    usageErr("%s [-n num] file\n", argv[0]);
  }

  int fd = open(argv[optind], O_RDONLY);
  if (fd == -1) { errExit("open(\"%s\", O_RDONLY)\n", argv[optind]); }

  char buf[BUF_SIZE];
  ssize_t numRead, numWritten;

  off_t cur = lseek(fd, 0, SEEK_END);;
  while (cur > 0) {
    size_t toRead;
    if (cur < BUF_SIZE) {
      toRead = (size_t) cur;
      cur = 0;
    } else {
      toRead = BUF_SIZE; 
      cur = cur - BUF_SIZE;
    }
    if (lseek(fd, cur, SEEK_SET) == -1) {
      errExit("lseek\n");
    }

    numRead = read(fd, buf, toRead);
    if (numRead != toRead) { errExit("couldn't read whole file\n"); }
    for (int i = numRead - 1; i >= 0; i--) {
      if (buf[i] == '\n' && --n == 0) {
        numWritten = write(STDOUT_FILENO, &buf[i+1], numRead - (i+1));
        if (numWritten != numRead - (i+1)) { errExit("write"); }
        goto after_outer_loop;
      }
    }
    if (cur == 0) {
      numWritten = write(STDOUT_FILENO, buf, toRead);
      if (numWritten != toRead) { errExit("write"); }
    }
  }

  after_outer_loop:

  // read rest of file and print it to stdout
  while ((numRead = read(fd, buf, BUF_SIZE)) > 0) {
    numWritten = write(STDOUT_FILENO, buf, (size_t) numRead);
    if (numWritten != numRead) { errExit("couldn't write whole buffer"); }
  }
  if (numRead == -1) { errExit("read"); }

  exit(EXIT_SUCCESS);
}