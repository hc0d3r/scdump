#ifndef __ELF_PHDR_H__
#define __ELF_PHDR_H__

#include <elf.h>
#include "io.h"

Elf64_Phdr *get_elf_phdr64(Elf64_Ehdr *header, struct io_utils *fh);
Elf32_Phdr *get_elf_phdr32(Elf32_Ehdr *header, struct io_utils *fh);

#endif
