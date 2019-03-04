#ifndef __ELF_MACROS_H__
#define __ELF_MACROS_H__

#define ElfW(type)	_ElfW (Elf, ELFARCH, type)
#define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)	e##w##t

#define uintX_t uintX_te(uint, ELFARCH)
#define uintX_te(x, y) uintX_t1(x, y, _t)
#define uintX_t1(x, y, z) x##y##z

#define Arch(n) _Arch(n, ELFARCH)
#define _Arch(n, a) _Arch1(n, a)
#define _Arch1(n, a) n##a

#endif
