#include <stdio.h>
#include "file_io.h"
#include "ttable.h"
#include "words.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return 1;
    }

    char word_buffer[BUFFER_SIZE];

    char *file_buf;
	size_t file_size;

    if (get_file_buf(argv[1], &file_buf, &file_size) < 0) {
        fprintf(stderr, "File %s not found.\n", argv[1]);
        return 1;
    }

    ttable_init();
    run_words(file_buf, file_size, word_buffer);
    ttable_list();
    ttable_destroy();

    close_file_buf();
}
