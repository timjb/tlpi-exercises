/* Exercise 12-2 */

#define _BSD_SOURCE

#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define STATUS_FILE_SIZE 4000
#define CMDLINE_SIZE 1024

int isNumeric (char *);
pid_t getMaxPid (void);
pid_t getPPidFromProcStatus (char *, size_t);
void gatherProcessInfo (pid_t);
void printTree (int, pid_t);

struct p_info {
  pid_t p_ppid;
  char p_cmdline[CMDLINE_SIZE];
} **processes_by_pid;

pid_t *processes;
size_t n_processes = 0;

int isNumeric (char *str) {
  while (*str != '\0') {
    if (!isdigit(*str)) { return 0; }
    str++;
  }
  return 1;
}

pid_t getMaxPid (void) {
  int fd = open("/proc/sys/kernel/pid_max", O_RDONLY);
  if (fd == -1) { errExit("open(\"/proc/sys/kernel/pid_max\")\n"); }
  #define PID_STR_LEN 16
  char pid_max_str[PID_STR_LEN];
  ssize_t bytesRead = read(fd, pid_max_str, PID_STR_LEN);
  if (bytesRead == -1) {
    errExit("reading /proc/sys/kernel/pid_max");
  }
  return (pid_t) strtol(pid_max_str, NULL, 10);
}

pid_t getPPidFromProcStatus (char *status, size_t n) {
  int sol = 1; /* start of line */
  int i = 0;
  while (i < n) {
    if (sol && strncmp(&status[i], "PPid:", 5) == 0) {
      i += 5;
      while (isspace(status[i])) { i++; }
      return (uid_t) strtol(&status[i], NULL, 10);
    }
    sol = 0;
    if (status[i] == '\n') { sol = 1; }
    i++;
  }
  return -1;
}

void gatherProcessInfo (pid_t max_pid) {
  DIR *proc = opendir("/proc");
  if (proc == NULL) { errExit("opendir(\"/proc\")\n"); }

  struct dirent *dir;
  errno = 0;
  while ((dir = readdir(proc)) != NULL) {
    if (!(dir->d_type == DT_DIR && isNumeric(dir->d_name))) { continue; }

    pid_t pid = (pid_t) strtol(dir->d_name, NULL, 10);
    if (pid > max_pid) {
      /* max_pid got increased during the running time of this program and
         there's a process that has a higher pid than the former limit */
      continue;
    }

    char status_path[64];
    snprintf(status_path, 64, "/proc/%s/status", dir->d_name);
    int status_file = open(status_path, O_RDONLY);
    if (status_file == -1) {
      errno = 0;
      continue;
    }

    char status[STATUS_FILE_SIZE];
    ssize_t bytesRead = read(status_file, status, STATUS_FILE_SIZE);
    close(status_file);
    if (bytesRead == -1) {
      errno = 0;
      continue;
    }

    struct p_info *pi = (struct p_info *) malloc(sizeof(struct p_info));
    if (pi == NULL) { errExit("malloc"); }
    pi->p_ppid = getPPidFromProcStatus(status, (size_t) bytesRead);

    char cmdline_path[64];
    snprintf(cmdline_path, 64, "/proc/%s/cmdline", dir->d_name);
    int cmdline_file = open(cmdline_path, O_RDONLY);
    if (cmdline_file == -1) {
      errno = 0;
      continue;
    }

    char *cmdline = pi->p_cmdline;
    bytesRead = read(cmdline_file, cmdline, CMDLINE_SIZE);
    close(cmdline_file);
    if (bytesRead == -1) {
      errno = 0;
      continue;
    }

    /* replace '\0's with spaces in cmdline */
    for (int j = 0; j < bytesRead-1; j++) {
      if (cmdline[j] == '\0') {
        cmdline[j] = ' ';
      }
    }

    processes_by_pid[pid] = pi;
    processes[n_processes++] = pid;
  }
  /*if (errno != 0) {}*/
}

void printTree (int level, pid_t pid) {
  struct p_info *pi = processes_by_pid[pid];
  if (pi == NULL) { return; }
  for (int i = 0; i < level; i++) { printf("    "); }
  printf("%ld %.80s\n", (long) pid, pi->p_cmdline);
  for (size_t i = 0; i < n_processes; i++) {
    struct p_info *other = processes_by_pid[processes[i]];
    if (other->p_ppid == pid) {
      /* is a child of current process */
      printTree(level + 1, processes[i]);
    }
  }
  //printf("%ld", (long) )
}

int main (int argc, char *argv[]) {
  pid_t max_pid = getMaxPid();
  processes_by_pid = (struct p_info **) calloc((size_t) max_pid + 1, sizeof(struct p_info *));
  if (processes_by_pid == NULL) { errExit("calloc\n"); }
  processes = (pid_t *) malloc(sizeof(pid_t *) * (max_pid + 1));
  if (processes == NULL) { errExit("malloc\n"); }
  gatherProcessInfo(max_pid);
  printTree(0, 1);

  exit(EXIT_SUCCESS);
}