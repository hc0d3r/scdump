#include "elf-shdr.h"
#include "elf-macros.h"

#include <stdlib.h>
#include <err.h>

ElfW(Shdr) *Arch(get_elf_shdr)(ElfW(Ehdr) *header, fh_t *fh){
    ElfW(Shdr) *ret;

    xset(fh, header->e_shoff);

    ret = malloc(sizeof(ElfW(Shdr)) * header->e_shnum);
    if(ret == NULL)
        err(1, "malloc");

    xread(fh->fd, ret, sizeof(ElfW(Shdr))*header->e_shnum);


    return ret;
}
