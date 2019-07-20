#include "elf-phdr.h"
#include "elf-macros.h"

#include <stdlib.h>
#include <err.h>

ElfW(Phdr) *Arch(get_elf_phdr)(ElfW(Ehdr) *header, fh_t *fh){
    ElfW(Phdr) *ret;

    xset(fh, header->e_phoff);

    ret = malloc(sizeof(ElfW(Shdr)) * header->e_phnum);
    if(ret == NULL)
        err(1, "malloc");

    xread(fh->fd, ret, sizeof(ElfW(Shdr))*header->e_phnum);


    return ret;
}
