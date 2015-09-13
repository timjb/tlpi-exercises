/* Exercise 5-2 */

#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s filename", argv[0]);
  }

  int fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
  if (fd == -1) { errExit("open"); }
  char s[] = "abcdefghi";
  char t[] = "jklmnopqr";
  if (write(fd, s, strlen(s)) != strlen(s)) { errExit("write 1"); }
  if (lseek(fd, 0, SEEK_SET) == -1) { errExit("seeking"); }
  if (write(fd, t, strlen(t)) != strlen(t)) { errExit("write 2"); }
  if (close(fd) == -1) { errExit("close output"); }
  exit(EXIT_SUCCESS);
}
