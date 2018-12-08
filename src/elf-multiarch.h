#ifndef __ELF_MULTIARCH_H__
#define __ELF_MULTIARCH_H__

#include <elf.h>
#include "io.h"

struct dynstr {
    char *ptr;
    int size;
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

#define ElfW(type)	_ElfW (Elf, ELFARCH, type)
#define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)	e##w##t

#define uintX_t uintX_te(uint, ELFARCH)
#define uintX_te(x, y) uintX_t1(x, y, _t)
#define uintX_t1(x, y, z) x##y##z

#define arch_sufix(n) _arch_sufix(n, ELFARCH)
#define _arch_sufix(n, a) _arch_sufix1(n, a)
#define _arch_sufix1(n, a) n##a


void extract_shellcode64(Elf64_Ehdr *header, struct extract_opts *opts);
void extract_shellcode32(Elf32_Ehdr *header, struct extract_opts *opts);
void dump(int fd, int len, struct dynstr *out);

#endif
