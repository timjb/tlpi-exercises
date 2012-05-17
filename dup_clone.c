/* Exercise 5-4 */

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include "tlpi_hdr.h"

/* guess */
#define OPEN_MAX 256

int my_dup (int);
int my_dup2 (int, int);
off_t get_offset (int); 

int my_dup (int oldfd) {
  return fcntl(oldfd, F_DUPFD, 0);
}

int my_dup2 (int oldfd, int newfd) {
  // based on the information from `man -s2 dup2`

  if (newfd < 0 || newfd >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }

  if (fcntl(oldfd, F_GETFL) == -1) {
    return -1;
  }

  if (oldfd == newfd) {
    return oldfd;
  }

  close(newfd);
  return fcntl(oldfd, F_DUPFD, newfd);
}

off_t get_offset (int fd) {
  off_t o = lseek(fd, 0, SEEK_CUR);
  if (o == -1) { errExit("getting offset"); }
  return o;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s testfile\n", argv[0]);
  }

  int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
  if (fd == -1) { errExit("open testfile"); }

  assert(my_dup(-1) == -1);
  assert(my_dup(42) == -1);

  int dup = my_dup(fd);
  assert(dup != -1);
  assert(dup != fd);
  assert(get_offset(fd) == 0);
  assert(get_offset(dup) == 0);
  char text[] = "abc";
  size_t text_len = strlen(text);
  if (write(dup, text, text_len) != (ssize_t) text_len) { errExit("writing 'abc'"); }
  assert(get_offset(fd) == (off_t) text_len);
  assert(get_offset(dup) == (off_t) text_len);

  assert(my_dup2(42, 42) == -1);
  assert(my_dup2(STDOUT_FILENO, STDOUT_FILENO) == STDOUT_FILENO);
  assert(my_dup2(fd, -1) == -1);
  assert(my_dup2(fd, OPEN_MAX) == -1);
  assert(my_dup2(42, STDOUT_FILENO) == -1);

  dup = my_dup2(fd, STDOUT_FILENO);
  assert(dup == STDOUT_FILENO);
  char greeting[] = "Hello World!";
  size_t combined_len = text_len + strlen(greeting);
  fprintf(stderr, "this should be the only output of this program\n");
  printf(greeting);
  fflush(stdout);
  assert(get_offset(dup) == combined_len);
  assert(get_offset(fd)  == combined_len);

  exit(EXIT_SUCCESS);
}