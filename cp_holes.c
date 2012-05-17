/* Exercise 4-2 */

#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main (int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s old-file new-file\n", argv[0]);
  }

  int inputFd = open(argv[1], O_RDONLY);
  if (inputFd == -1) {
    errExit("opening file %s", argv[1]);
  }

  int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;      /* rw-rw-rw- */
  int outputFd = open(argv[2], openFlags, filePerms);
  if (outputFd == -1) {
    errExit("opening file %s", argv[2]);
  }

  ssize_t numRead;
  char buf[BUF_SIZE];
  while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
    int i = 0, j;
    while (i < numRead) {
      for (j = i; j < numRead; j++) {
        if (buf[j] == '\0') { break; }
      }
      size_t nonzeroBytes = (size_t) j-i;
      if (nonzeroBytes > 0) {
        if (write(outputFd, &buf[i], nonzeroBytes) != nonzeroBytes) {
          fatal("couldn't write whole buffer");
        }
      }

      for (i = j; j < numRead; j++) {
        if (buf[j] != '\0') { break; }
      }
      off_t zeroBytes = (off_t) j-i;
      if (zeroBytes > 0) {
        if (lseek(outputFd, zeroBytes, SEEK_CUR) == -1) {
          fatal("couldn't write hole");
        }
      }
      i = j;
    }
  }
  if (numRead == -1) { errExit("read"); }

  if (close(inputFd) == -1) { errExit("close input"); }
  if (close(outputFd) == -1) { errExit("close output"); }

  exit(EXIT_SUCCESS);
}