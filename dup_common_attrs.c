/* Exercise 5-5 */

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include "tlpi_hdr.h"

off_t get_offset (int);

off_t get_offset (int fd) {
  off_t o = lseek(fd, 0, SEEK_CUR);
  if (o == -1) { errExit("get offset"); }
  return o;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s testfile\n", argv[0]);
  }

  int flags = O_RDWR | O_CREAT | O_TRUNC;
  mode_t filemode = S_IWUSR | S_IRUSR;
  int fd = open(argv[1], flags, filemode);
  if (fd == -1) { errExit("open testfile"); }
  int d = dup(fd);
  assert(d != -1);
  assert(fd != d);
  assert(fd != d);
  assert(get_offset(fd) == 0);
  assert(get_offset(d) == 0);
  char text[] = "lorem ipsum dolor sit amet";
  size_t text_len = strlen(text);
  if (write(fd, text, text_len) == -1) { errExit("writing text"); }
  assert(get_offset(fd) == text_len);
  assert(get_offset(d) == text_len);
  int f = fcntl(fd, F_GETFL);
  assert(f == O_RDWR); /* O_CREAT and O_TRUNC are apparently excluded after opening the file */
  assert(fcntl(d, F_GETFL) == O_RDWR);
  int new_flags = O_RDWR | O_APPEND;
  if (fcntl(fd, F_SETFL, new_flags) == -1) { errExit("setting file mode"); }
  assert(fcntl(fd, F_GETFL) == new_flags);
  assert(fcntl(d, F_GETFL) == new_flags);
}