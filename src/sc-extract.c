#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <err.h>

#include "parser/elf-multiarch.h"
#include "parser/elf-macros.h"
#include "parser/elf-types.h"

#include "sc-extract.h"
#include "datadump.h"
#include "common.h"

#define sc_extract ElfA(sc_extract, )

void sc_extract(struct extract_opts *opts){
    section_t *section;
    segment_t *segment;
    Elf_Sym *symbol;
    Elf_Phdr *phdr;
    elf_t elf;

    elf_parser(&elf, opts->fd);

    if(opts->section){
        section = getsectionbyname(&elf, opts->section);

        if(section)
            datadump(opts->fd_out, opts->raw, section->data, section->len);
    }

    if(opts->symbol){
        symbol = sym_name_lookup(&elf, opts->symbol);

        if(symbol){
            if(symbol->st_shndx >= elf.header->e_shnum){
                die("invalid st_shndx\n");
            }

            section = elf.sections + symbol->st_shndx;

            datadump(opts->fd_out, opts->raw,
                (char *)section->data+symbol->st_value-section->header->sh_addr,
                symbol->st_size);
        }
    }

    if(ElfA(opts->size.addr_, )){
        uintx_t start, end;
        uint16_t i;

        start = ElfA(opts->addr.addr_, );
        end = ElfA(opts->size.addr_, );

        for(i=0; i<elf.header->e_phnum; i++){
            phdr = elf.segments[i].header;
            if(start >= phdr->p_vaddr){

                if(phdr->p_vaddr+phdr->p_filesz >= start+end){
                    segment = elf.segments + i;

                    datadump(opts->fd_out, opts->raw,
                        segment->data + start - phdr->p_vaddr, end);
                }
            }
        }
    }
}
