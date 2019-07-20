#ifndef __ELF_MULTIARCH_H__
#define __ELF_MULTIARCH_H__

#include <elf.h>
#include "elf-macros.h"
#include "io.h"

struct dynstr {
    char *ptr;
    size_t size;
};

typedef union {
    uint64_t addr_64;
    uint32_t addr_32;
} xuint;

struct info_elf64 {
    Elf64_Shdr *shdr;
    Elf64_Phdr *phdr;
    Elf64_Ehdr *header;
};

struct info_elf32 {
    Elf32_Shdr *shdr;
    Elf32_Phdr *phdr;
    Elf32_Ehdr *header;
};


struct extract_opts {
    char *section;
    char *symbol;
    xuint addr;
    xuint size;

    int raw;

    fh_t fh;
    int fd_out;
};

void extract_shellcode64(Elf64_Ehdr *header, struct extract_opts *opts);
void extract_shellcode32(Elf32_Ehdr *header, struct extract_opts *opts);
void dump(int fd, int len, struct dynstr *out);

#endif
