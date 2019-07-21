#include <elf.h>
#include <unistd.h>

#include "elf-common.h"

int get_elf_arch(int fd){
    char buf[EI_CLASS+1];
    int ret;

    if(read(fd, buf, EI_CLASS+1) != sizeof(buf)){
        ret = -1;
    } else {
        ret = buf[EI_CLASS];
        lseek(fd, 0, SEEK_SET);
    }


    return ret;
}
