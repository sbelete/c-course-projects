#ifndef __WORDS_H
#define __WORDS_H
#include<stdlib.h>

#define MAX_WORD_SIZE 64
#define BUFFER_SIZE 1024

int run_words(char *file_buffer, size_t file_len, char *word_buffer);

#endif
