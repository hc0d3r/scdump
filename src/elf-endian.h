#ifndef __ELF_ENDIANNES_H__
#define __ELF_ENDIANNES_H__

#include <endian.h>
#include <elf.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
 #define C_BYTE_ORDER ELFDATA2LSB
 #define bswap_16(x) x = be16toh(x)
 #define bswap_32(x) x = be32toh(x)
 #define bswap_64(x) x = be64toh(x)
#else
 #define C_BYTE_ORDER ELFDATA2MSB
 #define bswap_16(x) x = le16toh(x)
 #define bswap_32(x) x = le32toh(x)
 #define bswap_64(x) x = le64toh(x)
#endif

#ifdef ELFARCH
 #define bswap_xx(x) _bswap_xx(x, ELFARCH)
 #define _bswap_xx(x, y) _bswap_xx1(x, y)
 #define _bswap_xx1(x, y) bswap_##y(x)
#endif

#define elf_endian(x) (x).e_ident[EI_DATA]


#endif
