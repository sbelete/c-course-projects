#ifndef __FILE_IO_H
#define __FILE_IO_H

#include <stdlib.h>

int get_file_buf(char *filename, char **buf, size_t *size);

void close_file_buf(void);

#endif
