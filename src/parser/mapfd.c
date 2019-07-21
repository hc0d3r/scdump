#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mapfd.h"

void *mapfd(int fd, size_t *len){
    void *ret = MAP_FAILED;
    struct stat st;

    if(fstat(fd, &st) == -1)
        goto end;

    ret = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    *len = (size_t)st.st_size;

    end:
    close(fd);
    return ret;
}
