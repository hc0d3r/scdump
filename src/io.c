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

    else if(n != count)
        die("failed to read size: %lu, bytes read = %ld\n", count, n);

}

void xread(int fd, void *buf, size_t count){
    ssize_t n = read(fd, buf, count);

    if(n == -1)
        pdie("read");

    else if(n != count)
        die("failed to read size: %lu, bytes read = %ld\n", count, n);
}

void datadump(struct io_utils *fh, off_t offset, off_t len, int fdout, int raw){
    static const char htable[]="0123456789abcdef";
    unsigned char buf[1024];
    char hdump[4097];
    char *ptr = hdump;

    size_t total = 0, size, i;
    ssize_t n;


    xset(fh, offset);

    while(total != len){
        size = (total+1024 > len) ? len-total : 1024;

        n = read(fh->fd, buf, size);
        if(n == -1)
            pdie("read");

        if(raw)
            write(fdout, buf, n);
        else {
            for(i=0; i<n; i++){
                *ptr++ = '\\';
                *ptr++ = 'x';
                *ptr++ = htable[buf[i] >> 4];
                *ptr++ = htable[buf[i] & 0xf];
            }
            *ptr = '\n';

            write(fdout, hdump, i*4+1);
        }

        if((size_t)n != size){
            warn("failed to dump all\n");
            break;
        } else if(!n){
            break;
        }
        total += n;
    }
}
