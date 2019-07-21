#ifndef __ELF_MULTIARCH_H__
#define __ELF_MULTIARCH_H__

#include "parser/elf-macros.h"
#include <elf.h>

typedef union {
    uint64_t addr_64;
    uint32_t addr_32;
} xuint;

struct extract_opts {
    char *section;
    char *symbol;
    xuint addr;
    xuint size;

    int raw;
    int fd;
    int fd_out;
};

void sc_extract32(struct extract_opts *);
void sc_extract64(struct extract_opts *);

#endif
