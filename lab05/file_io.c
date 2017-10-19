#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "file_io.h"

static int filefd = 0;

int get_file_buf(char *filename, char **buf, size_t *size) {
  struct stat ss;
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }
  filefd = fd;

  if (fstat(fd, &ss) == -1) {
    perror("fstat");
    return -1;
  }

  size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
  size_t extra_room = page_size - (ss.st_size & (page_size-1));
  if (!extra_room) extra_room = page_size;

  *size = ss.st_size + extra_room;
  char *tmpbuf = mmap(0, *size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (tmpbuf == MAP_FAILED) {
    perror("mmap");
	return -1;
  }

  *buf = tmpbuf;

  return 0;
}

void close_file_buf() {
  close(filefd);
}
