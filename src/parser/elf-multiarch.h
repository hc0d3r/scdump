#ifndef _ELF_MULTIARCH_H_
#define _ELF_MULTIARCH_H_

#include <stddef.h>
#include <elf.h>

#include "elf-types.h"

/* 64-bits structures */
typedef struct {
    Elf64_Shdr *header;
    void *data;
    size_t len;
} section64_t;

typedef struct {
    Elf64_Phdr *header;
    void *data;
    size_t len;
} segment64_t;

typedef struct {
    Elf64_Ehdr *header;
    segment64_t *segments;
    size_t nsegments;
    section64_t *sections;
    size_t nsections;
    size_t nbytes;
} elf64_t;

/* 32-bits structures */

typedef struct {
    Elf32_Shdr *header;
    void *data;
    size_t len;
} section32_t;

typedef struct {
    Elf32_Phdr *header;
    void *data;
    size_t len;
} segment32_t;

typedef struct {
    Elf32_Ehdr *header;
    segment32_t *segments;
    size_t nsegments;
    section32_t *sections;
    size_t nsections;
    size_t nbytes;
} elf32_t;


/* 64-bits functions */
section64_t *getsectionbyname64(elf64_t *elf, const char *name);
Elf64_Sym *sym_name_lookup64(elf64_t *elf, const char *name);
void elf_parser64(elf64_t *elf, int fd);
void free_elf64(elf64_t *elf);

/* 32-bits functions */
section32_t *getsectionbyname32(elf32_t *elf, const char *name);
Elf32_Sym *sym_name_lookup32(elf32_t *elf, const char *name);
void elf_parser32(elf32_t *elf, int fd);
void free_elf32(elf32_t *elf);


#endif
