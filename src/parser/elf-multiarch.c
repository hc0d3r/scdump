#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <elf.h>

#include "elf-multiarch.h"
#include "elf-types.h"

#include "mapfd.h"
#include "../common.h"

#define sanitize(x, y) do { \
    if((x) < (y)){ \
        fprintf(stderr, "error: %zu avaliable %zu required\n", (size_t)(x), (size_t)(y)); \
        exit(1); \
    } \
} while(0)

section_t *getsectionbyname(elf_t *elf, const char *name){
    section_t *ret = NULL;
    char *strings;
    size_t len;

    uint32_t sh_name;
    uint16_t shstrndx;
    size_t i, slen;

    // get section string index
    shstrndx = elf->header->e_shstrndx;
    if(shstrndx == SHN_UNDEF || shstrndx >= elf->nsections){
        goto end;
    }

    strings = elf->sections[shstrndx].data;
    len = elf->sections[shstrndx].len;

    slen = strlen(name)+1;

    for(i=0; i<elf->nsections; i++){
        sh_name = elf->sections[i].header->sh_name;
        if(sh_name == 0)
            continue;

        if(sh_name >= len || len-sh_name < slen)
            continue;

        if(memcmp(strings+sh_name, name, slen) == 0x0){
            ret = elf->sections + i;
            break;
        }
    }

    end:
    return ret;
}

Elf_Sym *sym_name_lookup(elf_t *elf, const char *name){
    section_t *strtab, *symtab, *aux;
    Elf_Sym *ret = NULL, *symbols;
    Elf_Addr diff;

    size_t slen, i;

    strtab = getsectionbyname(elf, ".strtab");
    symtab = getsectionbyname(elf, ".symtab");

    if(!strtab || !symtab){
        goto end;
    }

    slen = strlen(name)+1;

    symbols = symtab->data;
    for(i=0; i<symtab->len/sizeof(Elf_Sym); i++){
        if(symbols[i].st_name == 0)
            continue;

        if(symbols[i].st_name >= strtab->len || \
          strtab->len-symbols[i].st_name < slen){
            continue;
        }

        if(memcmp(strtab->data+symbols[i].st_name, name, slen) == 0x0){
            ret = symbols + i;

            // if symbol has a explicity size the job is done
            if(ret->st_size){
                break;
            }

            // otherwise, we need search for the next symbol
            // based on virtual address (st_value)
            diff = (Elf_Addr)-1;

            for(i=0; i<symtab->len/sizeof(Elf_Sym); i++){
                if(symbols+i == ret || symbols[i].st_shndx != ret->st_shndx)
                    continue;

                if(ret->st_value >= symbols[i].st_value)
                    continue;

                if(diff > symbols[i].st_value){
                    diff = symbols[i].st_value;
                }
            }

            if(diff != (Elf_Addr)-1){
                ret->st_size = diff - ret->st_value;
                break;
            }

            // if there's not a next symbols,
            // we set the symbol size based on
            // the section that the symbol refer
            if(ret->st_shndx >= elf->header->e_shnum){
                die("invalid st_shndx\n");
            }

            aux = elf->sections + ret->st_shndx;
            diff = aux->header->sh_addr + aux->header->sh_size;

            if(ret->st_value > diff){
                die("invalid size\n");
            }

            ret->st_size = diff - ret->st_value;
        }
    }

    end:
    return ret;
}

void elf_parser(elf_t *elf, int fd){
    Elf_Ehdr *header;
    Elf_Phdr *phdr;
    Elf_Shdr *shdr;

    void *ptr;
    size_t i, len;

    memset(elf, 0x0, sizeof(elf_t));

    ptr = mapfd(fd, &len);
    if(ptr == MAP_FAILED){
        perror("map_file() failed");
        exit(1);
    }

    // sanity check
    sanitize(len, sizeof(Elf_Ehdr));

    elf->nbytes = len;
    elf->header = header = ptr;

    sanitize(len, header->e_phoff+sizeof(Elf_Phdr)*header->e_phnum);
    sanitize(len, header->e_shoff+sizeof(Elf_Shdr)*header->e_shnum);

    elf->nsegments = header->e_phnum;
    if(elf->nsegments){
        elf->segments = malloc(elf->nsegments*sizeof(segment_t));
        if(elf->segments == NULL){
            perror("malloc()");
            exit(1);
        }
    }

    for(i=0; i<header->e_phnum; i++){
        phdr = ptr+header->e_phoff+sizeof(Elf_Phdr)*i;

        sanitize(len, phdr->p_offset+phdr->p_filesz);

        elf->segments[i].header = phdr;
        elf->segments[i].len = phdr->p_filesz;
        elf->segments[i].data = (phdr->p_filesz) ? ptr+phdr->p_offset : NULL;
    }

    elf->nsections = header->e_shnum;
    if(elf->nsections){
        elf->sections = malloc(elf->nsections*sizeof(section_t));
        if(elf->sections == NULL){
            perror("malloc()");
            exit(1);
        }
    }

    for(i=0; i<header->e_shnum; i++){
        shdr = ptr+header->e_shoff+sizeof(Elf_Shdr)*i;

        if(shdr->sh_type == SHT_NOBITS){
            elf->sections[i].data = NULL;
            elf->sections[i].len = 0;
        } else {
            sanitize(len, shdr->sh_offset+shdr->sh_size);
            elf->sections[i].data = ptr+shdr->sh_offset;
            elf->sections[i].len = shdr->sh_size;
        }

        elf->sections[i].header = shdr;
    }
}

void free_elf_parser(elf_t *elf){
    free(elf->segments);
    free(elf->sections);
    munmap(elf->header, elf->nbytes);
}
