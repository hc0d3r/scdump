#ifndef __ELF_SHDR_H__
#define __ELF_SHDR_H__

#include <elf.h>
#include "io.h"

Elf64_Shdr *get_elf_shdr64(Elf64_Ehdr *header, fh_t *fh);
Elf32_Shdr *get_elf_shdr32(Elf32_Ehdr *header, fh_t *fh);

#endif
