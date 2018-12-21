#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "datadump.h"
#include "common.h"

void datadump(struct io_utils *fh, off_t offset, size_t len, int fdout, int raw){
    static const char htable[]="0123456789abcdef";
    unsigned char buf[1024];
    char hdump[4096];
    char *ptr;

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
            ptr = hdump;

            for(i=0; i<(size_t)n; i++){
                *ptr++ = '\\';
                *ptr++ = 'x';
                *ptr++ = htable[buf[i] >> 4];
                *ptr++ = htable[buf[i] & 0xf];
            }

            write(fdout, hdump, i*4);
        }

        if((size_t)n != size){
            warn("failed to dump all\n");
            break;
        } else if(!n){
            break;
        }
        total += n;
    }

    if(!raw)
        write(fdout, "\n", 1);
}
