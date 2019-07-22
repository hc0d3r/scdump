#ifndef _ELF_ENDIANNES_H_
#define _ELF_ENDIANNES_H_

#include <endian.h>
#include <elf.h>

#include "elf-macros.h"

#if __BYTE_ORDER == __LITTLE_ENDIAN
 #define M_BYTE_ORDER ELFDATA2LSB
 #define bswap_16(x) x = be16toh(x)
 #define bswap_32(x) x = be32toh(x)
 #define bswap_64(x) x = be64toh(x)
#else
 #define M_BYTE_ORDER ELFDATA2MSB
 #define bswap_16(x) x = le16toh(x)
 #define bswap_32(x) x = le32toh(x)
 #define bswap_64(x) x = le64toh(x)
#endif

#define elf_endian(x) (x).e_ident[EI_DATA]

#define bswap_xx ElfA(bswap_, )
#define normalize ElfA(normalize, )
#define symnormalize ElfA(symnormalize, )

void normalize32(void *elfptr, size_t len);
void normalize64(void *elfptr, size_t len);

void symnormalize32(Elf32_Sym *symbols, size_t len);
void symnormalize64(Elf64_Sym *symbols, size_t len);


#endif
