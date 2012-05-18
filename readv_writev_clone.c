/* Exercise 5-7 */

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/uio.h>
#include "tlpi_hdr.h"

size_t count_iovec (const struct iovec *, int);
ssize_t my_readv (int, const struct iovec *, int);
ssize_t my_writev (int, const struct iovec *, int);

size_t count_iovec (const struct iovec *iov, int iovcnt) {
  size_t count = 0;
  for (int i = 0; i < iovcnt; i++) {
    count += iov[i].iov_len;
  }
  return count;
}

ssize_t my_readv (int fd, const struct iovec *iov, int iovcnt) {
  size_t count = count_iovec(iov, iovcnt);

  char *buf = (char *) malloc(count);
  ssize_t bytesRead = read(fd, buf, count);
  if (bytesRead == -1) {
    free((void *) buf);
    return -1;
  }

  size_t index = 0;
  for (int i = 0; i < iovcnt && index < bytesRead; i++) {
    size_t iovlen = min(iov[i].iov_len, bytesRead - index);
    memcpy(iov[i].iov_base, &buf[index], iovlen);
    index += iovlen;
  }

  free((void *) buf);
  return bytesRead;
}

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt) {
  size_t count = count_iovec(iov, iovcnt);

  char *buf = (char *) malloc(count);

  size_t index = 0;
  for (int i = 0; i < iovcnt; i++) {
    memcpy(&buf[index], iov[i].iov_base, iov[i].iov_len);
    index += iov[i].iov_len;
  }

  ssize_t bytesWritten = write(fd, buf, count);
  free(buf);
  return bytesWritten;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    usageErr("%s testfile\n", argv[0]);
  }

  int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
  if (fd == -1) { errExit("open testfile"); }

  struct iovec text[3];
  char *lorem = "lorem\n";
  text[0].iov_len = 6;
  text[0].iov_base = lorem;
  char *ipsum = "ipsum\n";
  text[1].iov_len = 6;
  text[1].iov_base = ipsum;
  char *dolor = "dolor\n";
  text[2].iov_len = 6;
  text[2].iov_base = dolor;

  char *total_text = "lorem\nipsum\ndolor\n";

  ssize_t bytesWritten = my_writev(fd, text, 3);
  if (bytesWritten == -1) { errExit("my_writev"); }
  if (lseek(fd, 0, SEEK_SET) == -1) { errExit("lseek"); }
  char buf[18];
  if (read(fd, buf, 18) != 18) { errExit("read"); }
  assert(memcmp(buf, total_text, 18) == 0);

  struct iovec read_vec[4];
  char a[2];
  read_vec[0].iov_len = 2;
  read_vec[0].iov_base = a;
  char b[3];
  read_vec[1].iov_len = 3;
  read_vec[1].iov_base = b;
  char c[5];
  read_vec[2].iov_len = 5;
  read_vec[2].iov_base = c;
  char d[8];
  read_vec[3].iov_len = 8;
  read_vec[3].iov_base = d;

  if (lseek(fd, 0, SEEK_SET) == -1) { errExit("lseek"); }
  if (my_readv(fd, read_vec, 4) != 18) { errExit("my_readv"); }
  assert(memcmp(a, "lo", 2) == 0);
  assert(memcmp(b, "rem", 3) == 0);
  assert(memcmp(c, "\nipsu", 5) == 0);
  assert(memcmp(d, "m\ndolor\n", 8) == 0);

  exit(EXIT_SUCCESS);
}