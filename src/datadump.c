#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "datadump.h"
#include "common.h"

void datadump(int fdout, int raw, const void *data, size_t len){
    static const char htable[]="0123456789abcdef";
    char hdump[4096], *ptr;
    unsigned char *aux;
    size_t i, limit, total;

    if(raw){
        write(fdout, data, len);
        return;
    }

    aux = (unsigned char *)data;
    total = 0;

    while(len){
        ptr = hdump;

        limit = (len > 1024) ? 1024 : len;
        len -= limit;

        for(i=0; i<limit; i++){
            *ptr++ = '\\';
            *ptr++ = 'x';
            *ptr++ = htable[aux[total]/16];
            *ptr++ = htable[aux[total]%16];

            total++;
        }
        write(fdout, hdump, limit*4);
    }

    write(fdout, "\n", 1);
}
