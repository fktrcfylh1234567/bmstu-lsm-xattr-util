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

void add_gid(char *path, int gid) {
  char *attr;
  int *data;
  int ret = EXIT_FAILURE;
  ssize_t size;

  size = getxattr(path, SEC_LABEL, NULL, 0);
  if (size < 0) {
    perror("[-] getxattr failed");
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

void set_xattr(char *path) {
  char *attr;
  int *data;
  int ret = EXIT_FAILURE;
  int count = 3;
  int buff_size = count * sizeof(int) + 1;

  attr = malloc(buff_size);
  if (attr == NULL) {
    printf("No memory.\n");
    return;
  }

  data = calloc(count, sizeof(int));
  if (data == NULL) {
    printf("No memory.\n");
    return;
  }

  data[0] = 1000;
  data[1] = 2000;
  data[2] = 3000;

  memcpy(attr, data, buff_size - 1);
  attr[buff_size] = '\0';

  ret = setxattr(path, SEC_LABEL, attr, buff_size, 0);

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

  if (strcmp(argv[1], "set") == 0) {
    set_xattr(argv[2]);
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
