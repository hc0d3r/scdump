#ifndef _ELF_TYPES_H_
#define _ELF_TYPES_H_

#include "elf-macros.h"

/* types aliases */

#define Elf_Ehdr ElfW(Ehdr)
#define Elf_Phdr ElfW(Phdr)
#define Elf_Shdr ElfW(Shdr)
#define Elf_Sym ElfW(Sym)
#define Elf_Addr ElfW(Addr)

#define section_t ElfT(section)
#define elf_t ElfT(elf)
#define segment_t ElfT(segment)
#define uintx_t ElfT(uint)

/* functions aliases */

#define getsectionbyname ElfA(getsectionbyname, )
#define sym_name_lookup ElfA(sym_name_lookup, )
#define elf_parser ElfA(elf_parser, )
#define free_elf_parser ElfA(free_elf_parser, )

#endif
