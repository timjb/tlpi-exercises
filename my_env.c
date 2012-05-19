/* Exercise 6-3 */

#include <stdlib.h>
#include <assert.h>
#include "tlpi_hdr.h"

extern char **environ;

int my_setenv (const char *, const char *, int);
int my_unsetenv (const char *);

int my_setenv (const char *name, const char *value, int overwrite) {
  if (name == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (overwrite || getenv(name) == NULL) {
    size_t len = strlen(name) + 1 + strlen(value) + 1;
    char *env_entry = (char *) malloc(len);
    if (env_entry == NULL) {
      errno = ENOMEM;
      return -1;
    }
    strcpy(env_entry, name);
    env_entry[strlen(name)] = '=';
    strcpy(&env_entry[strlen(name) + 1], value);
    int r = putenv(env_entry);
    return r == 0 ? 0 : -1;
  }
  return 0;
}

int my_unsetenv (const char *name) {
  if (name == NULL) {
    errno = EINVAL;
    return -1;
  }

  size_t n = strlen(name);
  int i = 0, j = 0;
  while (environ[j] != NULL) {
    if (strncmp(environ[j], name, n) == 0 && environ[j][n] == '=') {
      j++;
    } else {
      if (j != i) {
        environ[i] = environ[j];
      }
      i++;
      j++;
    }
  }
  environ[i] = NULL;
  return 0;
}

int main (int argc, char *argv[]) {
  char name[] = "GREET";
  char value[] = "bonjour";

  assert(getenv(name) == NULL);
  assert(my_setenv(NULL, value, 0) == -1);
  assert(my_setenv(name, value, 0) == 0);
  assert(strcmp(getenv(name), "bonjour") == 0);
  value[0] = 'B';
  value[1] = 'O';
  assert(strcmp(getenv(name), "bonjour") == 0);
  assert(my_setenv(name, "guten tag", 0) == 0);
  assert(strcmp(getenv(name), "bonjour") == 0);
  assert(my_setenv(name, "guten tag", 1) == 0);
  assert(strcmp(getenv(name), "guten tag") == 0);
  assert(my_unsetenv(NULL) == -1);
  assert(my_unsetenv(name) == 0);
  assert(my_setenv("lorem", "ipsum", 0) == 0);
  assert(getenv(name) == NULL);
  assert(strcmp(getenv("lorem"), "ipsum") == 0);

  exit(EXIT_SUCCESS);
}