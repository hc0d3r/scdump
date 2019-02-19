#ifndef __ELF_MULTIARCH_H__
#define __ELF_MULTIARCH_H__

#include <elf.h>
#include "elf-macros.h"
#include "io.h"

struct dynstr {
    char *ptr;
    size_t size;
};

union xuint {
    uint64_t addr_64;
    uint32_t addr_32;
};


struct extract_opts {
    char *section;
    char *symbol;
    union xuint addr;
    union xuint size;

    int raw;

    struct io_utils fh;
    int fd_out;
};

void extract_shellcode64(Elf64_Ehdr *header, struct extract_opts *opts);
void extract_shellcode32(Elf32_Ehdr *header, struct extract_opts *opts);
void dump(int fd, int len, struct dynstr *out);

#endif
