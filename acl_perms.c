/* Exercise 17-1 */

#include <acl/libacl.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"
#include "ugid_functions.h"

void usage (char *);
int maskPermset (acl_permset_t, acl_permset_t);
int handleError(char *, int);
void printPerms (acl_permset_t);

void usage (char *prog_name) {
  usageErr("%s (u|g) (user|group) file\n", prog_name);
}

int maskPermset (acl_permset_t perms, acl_permset_t mask) {
  int a;

  a = acl_get_perm(mask, ACL_READ);
  if (a == -1) { return -1; }
  if (a == 0) {
    if (acl_delete_perm(perms, ACL_READ) == -1) {
      return -1;
    }
  }

  a = acl_get_perm(mask, ACL_WRITE);
  if (a == -1) { return -1; }
  if (a == 0) {
    if (acl_delete_perm(perms, ACL_WRITE) == -1) {
      return -1;
    }
  }

  a = acl_get_perm(mask, ACL_EXECUTE);
  if (a == -1) { return -1; }
  if (a == 0) {
    if (acl_delete_perm(perms, ACL_EXECUTE) == -1) {
      return -1;
    }
  }

  return 0;
}

int handleError(char *str, int a) {
  if (a == -1) {
    errExit(str);
  }
  return a;
}

void printPerms (acl_permset_t permset) {
  printf(
    "%c%c%c",
    (handleError("acl_get_perm", acl_get_perm(permset, ACL_READ))    ? 'r' : '-'),
    (handleError("acl_get_perm", acl_get_perm(permset, ACL_WRITE))   ? 'w' : '-'),
    (handleError("acl_get_perm", acl_get_perm(permset, ACL_EXECUTE)) ? 'x' : '-')
  );
}

int main (int argc, char *argv[]) {
  if (argc != 4) { usage(argv[0]); }

  char mode;
  if (strcmp(argv[1], "u") == 0) { mode = 'u'; }
  else if (strcmp(argv[1], "g") == 0) { mode = 'g'; }
  else { usage(argv[0]); }

  uid_t uid;
  gid_t gid;
  if (mode == 'u') {
    uid = userIdFromName(argv[2]);
    if (uid == -1) {
      errExit("couldn't find user %s\n", argv[2]);
    }
  } else {
    // mode == 'g'
    gid = groupIdFromName(argv[2]);
    if (gid == -1) {
      errExit("couldn't find group %s\n", argv[2]);
    }
  }

  char *filepath = argv[3];

  struct stat stats;
  if (stat(filepath, &stats) == -1) { errExit("stat\n"); }

  acl_t acl = acl_get_file(filepath, ACL_TYPE_ACCESS);
  if (acl == NULL) { errExit("acl_get_file\n"); }

  acl_entry_t entry;
  acl_tag_t tag;
  int entryId;

  int mask_found = 0;
  acl_entry_t mask;
  for (entryId = ACL_FIRST_ENTRY; ; entryId = ACL_NEXT_ENTRY) {
    if (acl_get_entry(acl, entryId, &entry) == 1) {
      break;
    }
    if ((tag = acl_get_tag_type(entry, &tag)) == -1) {
      errExit("acl_get_tag_type\n");
    }
    if (tag == ACL_MASK) {
      mask_found = 1;
      mask = entry;
      break;
    }
  }

  acl_entry_t needle;
  uid_t *uid_p;
  gid_t *gid_p;
  for (entryId = ACL_FIRST_ENTRY; ; entryId = ACL_NEXT_ENTRY) {
    if (acl_get_entry(acl, entryId, &entry) != 1) {
      errExit(
        "couldn't find an entry for the specified %s\n",
        mode == 'u' ? "user" : "group"
      );
    }
    if (acl_get_tag_type(entry, &tag) == -1) {
      errExit("acl_get_tag_type\n");
    }

    if (mode == 'u') {
      if (uid == stats.st_uid && tag == ACL_USER_OBJ) {
        needle = entry;
        break;
      }
      if (tag != ACL_USER) { continue; }
      uid_p = acl_get_qualifier(entry);
      if (uid_p == NULL) { errExit("acl_get_qualifier\n"); }
      if (*uid_p == uid) {
        needle = entry;
        break;
      }
    }

    if (mode == 'g') {
      if (gid == stats.st_gid && tag == ACL_GROUP_OBJ) {
        needle = entry;
        break;
      }
      if (tag != ACL_GROUP) { continue; }
      gid_p = acl_get_qualifier(entry);
      if (gid_p == NULL) { errExit("acl_get_qualifier\n"); }
      if (*gid_p == gid) {
        needle = entry;
        break;
      }
    }
  }

  acl_permset_t needle_perms;
  if (acl_get_permset(needle, &needle_perms) == -1) {
    errExit("acl_get_permset\n");
  }
  printPerms(needle_perms);
  printf("\n");
  if (mask_found && !(mode == 'u' && uid == stats.st_uid && tag == ACL_USER_OBJ)) {
    acl_permset_t mask_perms;
    if (acl_get_permset(mask, &mask_perms) == -1) {
      errExit("acl_get_permset\n");
    }
    printf("Effective permissions: ");
    if (maskPermset(needle_perms, mask_perms) == -1) {
      errExit("maskPermset\n");
    }
    printPerms(needle_perms);
    printf("\n");
  }

  exit(EXIT_SUCCESS);
}