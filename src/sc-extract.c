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
    segment_t *segment = NULL;
    Elf_Sym *symbol;
    Elf_Phdr *phdr = NULL, *aux = NULL;
    elf_t elf;

    uintx_t start, end, len = 0, max = 0;
    uint16_t i;

    elf_parser(&elf, opts->fd);

    if(opts->section){
        section = getsectionbyname(&elf, opts->section);

        if(section)
            datadump(opts->fd_out, opts->raw, section->data, section->len);
        else
            die("section %s not found\n", opts->section);
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
        } else {
            die("symbol '%s' not found\n", opts->symbol);
        }
    }

    if(ElfA(opts->size.addr_, )){
        start = ElfA(opts->addr.addr_, );
        end = ElfA(opts->size.addr_, );

        for(i = 0; i < elf.header->e_phnum; i++){
            phdr = elf.segments[i].header;
            if(start >= phdr->p_vaddr){
                if(phdr->p_vaddr+phdr->p_filesz >= start+end){
                    segment = elf.segments + i;
                    len = end;
                    break;
                }

                if(max < phdr->p_filesz){
                    max = phdr->p_filesz;
                    aux = phdr;
                }
            }
        }

        if(segment){
            datadump(opts->fd_out, opts->raw,
                segment->data + start - phdr->p_vaddr, len);
        } else {
            if(!max){
                die("address 0x%" ElfA(PRIx, ) " not found\n", start);
            } else {
                die("out of range: %" ElfA(PRIu, ) " byte(s) required,"
                    " %" ElfA(PRIu, ) " avaliable\n", end, (aux->p_vaddr - start)+aux->p_filesz);
            }
        }
    }
}
