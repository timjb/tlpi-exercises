/*
  Exercise 7-2: implement malloc and free

  this is probably horrible c code.

  deficencies:

  * it doesn't increase the page break by huge sizes to avoid too many system calls
  * it doesn't lower the page break when memory gets freed
  * it requires space to store pointers to the last and next block even for non-free blocks
  * performance
*/

#define _SVID_SOURCE

#include <assert.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

extern char end;

void *my_malloc (size_t);
void my_free(void *);
void print_blk_list(void);

struct blk {
  size_t size;
  struct blk *prev;
  struct blk *next;
};

struct blk *first = NULL;
struct blk *last = NULL;

void print_blk_list (void) {
  struct blk *curr = first;
  while (curr != NULL) {
    printf("%10p %lld, ", (void *) curr, (long long) curr->size);
    curr = curr->next;
  }
  printf("\n");
}

void *my_malloc (size_t size) {
  size_t required_size = size + sizeof(struct blk);
  
  struct blk *curr = first;
  while (curr != NULL && curr->size < required_size) {
    curr = curr->next;
  }

  if (curr == NULL) {
    void *new = sbrk((intptr_t) required_size);
    if (new == (void *) -1) { return NULL; }
    struct blk *new_blk = (struct blk *) new;
    new_blk->size = required_size;
    return (void *) (new_blk + 1);
  }

  if (curr == first) { first = first->next; }
  else { curr->prev->next = curr->next; }

  if (curr == last) { last = last->prev; }
  else { curr->next->prev = curr->prev; }

  if (curr->size > required_size + sizeof(struct blk)) {
    struct blk *left = (struct blk *) (((char *) curr) + required_size);
    left->size = curr->size - required_size;
    curr->size = required_size;
    my_free((char *) (left + 1));
  }

  return (void *) (curr + 1);
}

void my_free (void *ptr) {
  print_blk_list();
  struct blk *blk_ptr = ((struct blk *) ptr) - 1;

  if (first == NULL) {
    first = last = blk_ptr;
    return;
  }

  if (blk_ptr < first) {
    blk_ptr->prev = NULL;
    if (((char *) blk_ptr) + blk_ptr->size == (char *) first) {
      blk_ptr->size += first->size;
      blk_ptr->next = first->next;
    } else {
      first->prev = blk_ptr;
      blk_ptr->next = first;
    }
    first = blk_ptr;
    return;
  }

  if (blk_ptr > last) {
    if (((char *) last) + last->size == (char *) blk_ptr) {
      last->size += blk_ptr->size;
    } else {
      blk_ptr->next = NULL;
      blk_ptr->prev = last;
      last->next = blk_ptr;
      last = blk_ptr;
    }
    return;
  }

  struct blk *curr = first;
  while (curr < blk_ptr) {
    curr = curr->next;
  }

  struct blk *before = curr->prev;
  if (((char *) before) + before->size == (char *) blk_ptr) {
    before->size += blk_ptr->size;
    blk_ptr = before;
  } else {
    blk_ptr->prev = before;
    before->next = blk_ptr;
  }

  if (((char *) blk_ptr) + blk_ptr->size == (char *) curr) {
    blk_ptr->size += curr->size;
    blk_ptr->next = curr->next;
    curr->next->prev = blk_ptr;
  } else {
    blk_ptr->next = curr;
    curr->prev = blk_ptr;
  }
}

#define MAX_ALLOCS 1000000

int main (int argc, char *argv[]) {
  /* copied from free_and_sbrk.c -- licensed by Michael Kerrisk under the GPLv3 */

  char *ptr[MAX_ALLOCS];
  int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

  printf("\n");

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
  }

  numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");
  if (numAllocs > MAX_ALLOCS) {
    cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);
  }

  blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

  freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
  freeMin =  (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
  freeMax =  (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

  if (freeMax > numAllocs) {
    cmdLineErr("free-max > num-allocs\n");
  }

  printf("Initial program break:          %10p\n", sbrk(0));

  printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
  for (j = 0; j < numAllocs; j++) {
    ptr[j] = my_malloc(blockSize);
    if (ptr[j] == NULL) {
      errExit("malloc");
    }
    printf("%10p\n", sbrk(0));
  }

  printf("Program break is now:           %10p\n", sbrk(0));

  printf("Freeing blocks from %d to %d in steps of %d\n",
              freeMin, freeMax, freeStep);
  for (j = freeMin - 1; j < freeMax; j += freeStep) {
    my_free(ptr[j]);
  }

  printf("After my_free(), program break is: %10p\n", sbrk(0));

  exit(EXIT_SUCCESS);
}