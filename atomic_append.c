/* Exercise 5-3 */

#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main (int argc, char *argv[]) {
  if (argc < 3 || argc > 4) {
    usageErr("%s filename num-bytes [x]", argv[0]);
  }

  long n = getLong(argv[2], GN_NONNEG | GN_ANY_BASE, "num-bytes");
  Boolean x = argc == 4 && strcmp(argv[3], "x") == 0;
  int flags = O_WRONLY | O_CREAT;
  if (!x) { flags |= O_APPEND; }
  int fd = open(argv[1], flags, S_IWUSR | S_IRUSR);
  if (fd == -1) { errExit("open"); }

  while (n-- > 0) {
    if (x) {
      if (lseek(fd, 0, SEEK_END) == -1) { errExit("seek"); }
    }
    if (write(fd, "a", 1) == -1) {
      errExit("write byte a");
    }
  }
}