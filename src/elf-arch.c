#include <elf.h>
#include <unistd.h>

#include "elf-arch.h"

int elf_arch(int fd, void *header, int size){
    char *e_ident;
    ssize_t n;
    int ret;

    if((n = read(fd, header, size)) == -1)
        return -1;

    /* I dont check magic header, etc, etc, just check if elf is
     * 32 or 64 bits */
    if(n < EI_CLASS)
        return 0;

    e_ident = header;
    ret = e_ident[EI_CLASS];
    if(ret == ELFCLASS32 && (size_t)n < sizeof(Elf32_Ehdr))
        ret = 0;
    else if(ret == ELFCLASS64 && (size_t)n < sizeof(Elf64_Ehdr))
        ret = 0;

    /* check if elf is little or big endian */
    if(e_ident[EI_DATA] != ELFDATA2MSB && e_ident[EI_DATA] != ELFDATA2LSB)
        ret = 0;

    return ret;
}
