#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: mmap <source filepath> <destination file path>\n");
        exit(1);
    }
    
    int src_fd, dst_fd;
    void *src, *dst;
    size_t filesize;
    off_t offset = 0;

    // TODO: Fill in the function outline below.

    // (1) Open the files passed to the program and assign
    //     file descriptors to them.
    src_fd = open(argv[1], O_RDWR);
    dst_fd = open(argv[2], O_RDWR | O_CREAT| O_TRUNC, 0666);
    if(src_fd == -1 || dst_fd == -1){
        perror("open");
        exit(1);
    }
    // (2) Find the size of source file.
    filesize = lseek(src_fd, 0, SEEK_END);

    if(filesize == -1) {
        perror("lseek");
        close(src_fd);
        close(dst_fd);
        exit(1);
    }
    // (3) Make sure that the destination file is the same size as source file.
    ftruncate(dst_fd, filesize);
    // (4) Map virtual addresses to source and destination file descriptors.
    src = mmap(0, filesize, PROT_READ, MAP_SHARED, src_fd, offset);
    dst = mmap(0, filesize, PROT_WRITE, MAP_SHARED, dst_fd, offset);

    if(dst == MAP_FAILED || src == MAP_FAILED) {
        perror("map");
        close(src_fd);
        close(dst_fd);
        exit(1);
    }
    // (5) Copy pages from source to destination addresses.
    memcpy(dst, src, filesize);
    // (6) Unmap virtual addresses and close file descriptors.
    if(munmap(dst, filesize) == -1 || munmap(src, filesize) == -1) {
        perror("munmap");
        close(src_fd);
        close(dst_fd);
        exit(1);
    }

    close(src_fd);
    close(dst_fd);
    // To test, run ./mmapcopy cit.jpg <output_file_path> and then
    // cs033_file_equals cit.jpg <output_file_path>
    return 0;
}
