#ifndef _ELF_MACROS_H_
#define _ELF_MACROS_H_

#include <stdio.h>
#include <stdlib.h>

#define ElfA(prefix, suffix) _ElfA(prefix, ELFARCH, suffix)
#define _ElfA(p, a, s) _ElfA_1(p, a, s)
#define _ElfA_1(p, a, s) p##a##s

#define ElfW(type) ElfA(Elf, _##type)
#define ElfT(n) ElfA(n, _t)

#define sanity_check(x, y) do { \
    if((x) < (y)){ \
        fprintf(stderr, "error: %zu avaliable %zu required\n", (size_t)(x), (size_t)(y)); \
        exit(1); \
    } \
} while(0)

#endif
