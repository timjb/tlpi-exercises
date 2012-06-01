/* Exercise 19-1 */

#define _BSD_SOURCE

#include <sys/inotify.h>
#include <limits.h>
#include <ftw.h>
#include "tlpi_hdr.h"

#define MAX_WATCH_DESCRIPTORS 8192
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* used to maintain a mapping between watch descriptors and filenames */
char *filenames[MAX_WATCH_DESCRIPTORS];

/* holds the file descriptor returned by inotify_init */
int infd;

void usage (char *);
void add_dir (char *);
int add_tree_walker (const char *, const struct stat *, int, struct FTW *);
void remove_dir (char *);
int remove_tree_walker (const char *, const struct stat *, int, struct FTW *);
void free_watch_descriptor(int);
void handle_event (struct inotify_event *);

void usage (char *program_name) {
  usageErr("%s directory...\n", program_name);
}

void add_dir (char *path) {
  /*
    Traverse the directory bottom-up ignoring symbolical links.
    We do this bottom-up because otherwise we would first call
    inotify_add_watcher on the parent directory, then ntwf would walk the child
    and thereby trigger the first inotify events on the parent directory.
  */
  if (nftw(path, add_tree_walker, 10, FTW_PHYS | FTW_DEPTH) == -1) {
    fprintf(stderr, "nftw failed on %s\n", path);
  }
}

int add_tree_walker (const char *fpath, const struct stat *sb,
                 int typeflags, struct FTW *tfwbuf) {
  if (typeflags & FTW_D) {
    int wd = inotify_add_watch(infd, fpath, IN_ALL_EVENTS | IN_ONLYDIR);
    if (wd == -1) {
      fprintf(stderr, "inotify_add_watch failed for %s\n", fpath);
    }
    char *path = (char *) malloc(strlen(fpath) + 1);
    strcpy(path, fpath);
    filenames[wd] = path;
  }
  return 0;
}

void remove_dir (char *oldpath) {
  for (int i = 0; i < MAX_WATCH_DESCRIPTORS; i++) {
    size_t oldlen = strlen(oldpath);
    if (filenames[i] != NULL && strncmp(oldpath, filenames[i], oldlen) == 0 &&
        (filenames[i][oldlen] == '\0' || filenames[i][oldlen] == '/')) {
      free_watch_descriptor(i);
    }
  }
}

void free_watch_descriptor (int wd) {
  if (inotify_rm_watch(infd, wd) == -1) {
    fprintf(stderr, "inotify_rm_watch failed\n");
  }
  if (filenames[wd] != NULL) {
    free(filenames[wd]);
    filenames[wd] = NULL;
  }
}

void handle_event (struct inotify_event *ine) {
  uint32_t mask = ine->mask;
  char *dir = filenames[ine->wd];
  char full_name[NAME_MAX];
  snprintf(full_name, NAME_MAX, "%s/%s", dir, ine->name);

  if (mask & IN_ACCESS) {
    printf("'%s' was accessed\n", full_name);
  }
  if (mask & IN_ATTRIB) {
    printf("metadata changed on '%s'\n", full_name);
  }
  if (mask & (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)) {
    printf("'%s' was closed\n", full_name);
  }
  if (mask & IN_CREATE) {
    printf("'%s' was created\n", full_name);
  }
  if (mask & IN_MOVED_TO) {
    printf("a file/directory was moved to '%s'\n", full_name);
  }
  if (mask & (IN_CREATE | IN_MOVED_TO)) {
    struct stat stat_buf;
    if (stat(full_name, &stat_buf) == -1) {
      fprintf(stderr, "stat failed\n");
    } else if (S_ISDIR(stat_buf.st_mode)) {
      add_dir(full_name);
    }
  }
  if (mask & IN_DELETE) {
    printf("'%s' was deleted\n", full_name);
  }
  if (mask & IN_DELETE_SELF) {
    printf("directory '%s' was removed\n", dir);
    free_watch_descriptor(ine->wd);
  }
  if (mask & IN_MODIFY) {
    printf("'%s' was modified\n", full_name);
  }
  if (mask & IN_MOVE_SELF) {}
  if (mask & IN_MOVED_FROM) {
    printf("'%s' was moved out of parent directory\n", full_name);
    remove_dir(full_name);
  }
  if (mask & IN_OPEN) {
    printf("'%s' was opened\n", full_name);
  }
  if (mask & IN_Q_OVERFLOW) {
    fprintf(stderr, "inotify queue overflow\n");
  }
  if (mask & IN_UNMOUNT) {
    printf("file system containing '%s' was unmounted\n", dir);
  }
}

int main (int argc, char *argv[]) {
  char buf[BUF_LEN];

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usage(argv[0]);
  }

  infd = inotify_init1(0);
  if (infd == -1) { errExit("inotify_init\n"); }

  /* reset filenames to be an array of NULL pointers */
  memset((void *) filenames, 0, sizeof(char *)*MAX_WATCH_DESCRIPTORS);

  for (int i = 1; i < argc; i++) {
    add_dir(argv[i]);
  }

  while (1) {
    ssize_t bytesRead = read(infd, buf, BUF_LEN);
    if (bytesRead <= 0) {
      errExit("read inotify descriptor\n");
    }
    for (char *p = buf; p < buf + bytesRead;) {
      struct inotify_event *ine = (struct inotify_event *) p;
      handle_event(ine);
      p += sizeof(struct inotify_event) + ine->len;
    }
  }

  exit(EXIT_SUCCESS);
}