#ifndef _ELF_MACROS_H_
#define _ELF_MACROS_H_

#define ElfA(prefix, suffix) _ElfA(prefix, ELFARCH, suffix)
#define _ElfA(p, a, s) _ElfA_1(p, a, s)
#define _ElfA_1(p, a, s) p##a##s

#define ElfW(type) ElfA(Elf, _##type)
#define ElfT(n) ElfA(n, _t)

#endif
