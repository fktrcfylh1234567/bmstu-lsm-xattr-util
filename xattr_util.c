#define _GNU_SOURCE

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/xattr.h>
#include <unistd.h>

#define SEC_LABEL "user.bmstu_exe"
//#define SEC_LABEL "security.bmstu_exe"

void set_gid(char *path, int gid) {
  char *attr;
  int data[1];
  int ret = EXIT_FAILURE;

  attr = malloc(sizeof(int) + 1);
  if (attr == NULL) {
    printf("No memory.\n");
    return;
  }

  data[0] = gid;

  memcpy(attr, data, sizeof(int));
  attr[sizeof(int) + 1] = '\0';

  ret = setxattr(path, SEC_LABEL, attr, sizeof(int) + 1, 0);

  if (ret < 0) {
    perror("[-] setxattr failed");
  }
}

void add_gid(char *path, int gid) {
  char *attr;
  int *data;
  int ret = EXIT_FAILURE;
  ssize_t size;

  size = getxattr(path, SEC_LABEL, NULL, 0);
  if (size < 0) {
    set_gid(path, gid);
    return;
  }

  attr = malloc(size + sizeof(int));
  if (attr == NULL) {
    printf("No memory.\n");
    return;
  }

  ret = getxattr(path, SEC_LABEL, attr, size);
  if (ret < 0) {
    perror("[-] getxattr failed");
    return;
  }

  data = malloc(size - 1 + sizeof(int));
  if (data == NULL) {
    printf("No memory.\n");
    return;
  }

  memcpy(data, attr, size - 1);

  data[(size - 1) / sizeof(int)] = gid;

  memcpy(attr, data, size - 1 + sizeof(int));
  attr[size + sizeof(int)] = '\0';

  ret = setxattr(path, SEC_LABEL, attr, size + sizeof(int), 0);

  if (ret < 0) {
    perror("[-] setxattr failed");
  }
}

void read_xattr(char *path) {
  char *attr;
  int *data;
  int ret = EXIT_FAILURE;
  ssize_t size;

  size = getxattr(path, SEC_LABEL, NULL, 0);
  if (size < 0) {
    perror("[-] getxattr failed");
    return;
  }

  attr = malloc(size);
  if (attr == NULL) {
    printf("No memory.\n");
    return;
  }

  ret = getxattr(path, SEC_LABEL, attr, size);
  if (ret < 0) {
    perror("[-] getxattr failed");
    return;
  }

  data = malloc(size - 1);
  if (data == NULL) {
    printf("No memory.\n");
    return;
  }

  memcpy(data, attr, size - 1);

  for (int i = 0; i < (size - 1) / sizeof(int); i++) {
    printf("%d ", data[i]);
  }

  printf("\n");
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Invalid arguments specified.\n");
    printf("Usage:\n");
    printf("xattr_util read <file>\n");
    printf("xattr_util add <gid> <file>\n");
    printf("xattr_util remove <gid> <file>\n");
    return 0;
  }

  if (strcmp(argv[1], "read") == 0) {
    read_xattr(argv[2]);
  }

  if (strcmp(argv[1], "add") == 0) {
    if (argv[3] == NULL) {
      printf("Usage: xattr_util add <gid> <file>\n");
      return 0;
    }

    add_gid(argv[3], atoi(argv[2]));
  }

  return 0;
}
