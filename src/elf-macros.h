#ifndef __ELF_MACROS_H__
#define __ELF_MACROS_H__

#define ElfW(type)	_ElfW (Elf, ELFARCH, type)
#define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)	e##w##t

#define uintX_t uintX_te(uint, ELFARCH)
#define uintX_te(x, y) uintX_t1(x, y, _t)
#define uintX_t1(x, y, z) x##y##z

#define archS(n) _archS(n, ELFARCH)
#define _archS(n, a) _archS1(n, a)
#define _archS1(n, a) n##a

#endif
