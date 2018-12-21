#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "io.h"
#include "common.h"

void rdonly(const char *filename, struct io_utils *out){
    struct stat buf;
    out->fd = open(filename, O_RDONLY);
    if(out->fd == -1){
        perror("open");
        exit(1);
    }

    if(fstat(out->fd, &buf) == -1){
        perror("fstat");
        exit(1);
    }

    out->size = buf.st_size;
}

void xset(struct io_utils *fh, off_t offset){
    if(fh->size <= offset)
        die("out of bounds\n");

    lseek(fh->fd, offset, SEEK_SET);
}

void xpread(int fd, void *buf, size_t count, off_t offset){
    ssize_t n = pread(fd, buf, count, offset);

    if(n == -1)
        pdie("pread");

    else if((size_t)n != count)
        die("failed to read size: %lu, bytes read = %ld\n", count, n);

}

void xread(int fd, void *buf, size_t count){
    ssize_t n = read(fd, buf, count);

    if(n == -1)
        pdie("read");

    else if((size_t)n != count)
        die("failed to read size: %lu, bytes read = %ld\n", count, n);
}
