/* Exercise 6-2: deliberately broken longjmp */

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

static jmp_buf jmp_env;

void a (void);
void b (void);

void a (void) {
  if (setjmp(jmp_env) == 0) {
    printf("first time setjmp\n");
  } else {
    printf("jumping with longjmp\n");
  }
}

void b (void) {
  longjmp(jmp_env, 1);
}

int main (int argc, char *argv[]) {
  printf("before a()\n");
  a();
  printf("before b()\n");
  b();
  printf("after b()\n");
  exit(EXIT_SUCCESS);
}