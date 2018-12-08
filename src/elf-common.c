#include <elf.h>
#include <unistd.h>

#include "elf-common.h"

int get_elf_arch(int fd, void *header, int size){
    char *e_ident;
    ssize_t n;
    int ret;

    if((n = read(fd, header, size)) == -1)
        return -1;

    /* I dont check magic header, etc, etc, just check if elf is
     * 32 or 64 bits */
    if(n < 4)
        return 0;

    e_ident = header;
    ret = e_ident[EI_CLASS];
    if(ret == ELFCLASS32 && n < sizeof(Elf32_Ehdr))
        ret = 0;
    else if(ret == ELFCLASS64 && n < sizeof(Elf64_Ehdr))
        ret = 0;

    /* only support little-endian for now, if you run it on a big-endian machine
     * you pc will explode */
    if(e_ident[EI_DATA] == ELFDATA2MSB)
        ret = 0;

    return ret;
}

uint32_t get_section_index(const char *str, uint32_t len, const char *section){
    uint32_t i, j, k;

    for(i=1; i<len; i++){
        for(j=0, k=i; section[j] == str[k] && k < len; j++, k++){
            if(section[j+1] == 0x0 && section[j+1] == str[k+1] && str[i-1] == 0x0)
                return i;
        }
    }

    return 0;
}
