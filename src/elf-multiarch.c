#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <err.h>

#include "elf-multiarch.h"
#include "elf-common.h"
#include "elf-shdr.h"
#include "elf-phdr.h"
#include "datadump.h"
#include "common.h"
#include "io.h"

void Arch(dumpbyaddr)(struct extract_opts *opts, struct Arch(info_elf) *info){
    ElfW(Ehdr) *header;
    ElfW(Phdr) *pheader;
    uintX_t start, end;
    uint16_t i;

    start = Arch(opts->addr.addr_);
    end = Arch(opts->size.addr_);

    header = info->header;

    if(!header->e_phoff)
        return;

    struct io_utils *fh = &opts->fh;

    for(i=0; i<header->e_phnum; i++){
        pheader = info->phdr + i;

        if(start >= pheader->p_vaddr && pheader->p_vaddr+pheader->p_filesz >= start+end){
            datadump(fh, pheader->p_offset+start-pheader->p_vaddr, end, opts->fd_out, opts->raw);
            return;
        }
    }

    warn("address %" Arch(PRIx) " not found\n", start);

}

void Arch(dumpbysymbol)(struct extract_opts *opts, struct Arch(info_elf) *info, struct dynstr *shstrtab){
    ElfW(Shdr) *symtab, *strtab, *section;
    ElfW(Ehdr) *header;
    ElfW(Sym) *sym;
    char *sname;
    int found = 0;

    uintX_t symbol_size = 0, diff;

    uint32_t i, j, strindex, symindex;
    struct dynstr strtab_dump;
    struct io_utils *fh = &opts->fh;

    strindex = get_section_index(shstrtab->ptr, shstrtab->size, ".strtab");
    if(!strindex){
        warn("section .strtab not found\n");
        return;
    }

    symindex = get_section_index(shstrtab->ptr, shstrtab->size, ".symtab");
    if(!symindex){
        warn("section .symtab not found\n");
        return;
    }

    header = info->header;

    symtab = NULL;
    strtab = NULL;
    j = 0;

    for(i=0; i<header->e_shnum && j<2; i++){
        section = info->shdr + i;

        if(section->sh_name == strindex){
            strtab = section;
        }

        else if(section->sh_name == symindex){
            symtab = section;
        }

        else {
            continue;
        }

        j++;
    }

    if(!symtab || !strtab){
        die("failed to find section\n");
    }

    /* get strtab */
    strtab_dump.size = strtab->sh_size;
    strtab_dump.ptr = malloc(strtab_dump.size);
    if(strtab_dump.ptr == NULL)
        err(1, "malloc");

    xpread(fh->fd, strtab_dump.ptr, strtab_dump.size, strtab->sh_offset);

    /* get symtab */
    sym = malloc(symtab->sh_size);
    if(sym == NULL)
        err(1, "malloc");

    xpread(fh->fd, sym, symtab->sh_size, symtab->sh_offset);

    for(i=0; i<symtab->sh_size; i++){
        if(sym[i].st_name >= strtab_dump.size || !sym[i].st_name)
            continue;

        sname = strtab_dump.ptr+sym[i].st_name;

        if(strcmp(sname, opts->symbol))
            continue;

        if(sym[i].st_size){
            symbol_size = sym[i].st_size;
        }

        else {
            diff = 0;

            for(j=0; j<symtab->sh_size; j++){
                if(sym[i].st_shndx != sym[j].st_shndx || j == i)
                    continue;

                if(sym[i].st_value > sym[j].st_value)
                    continue;

                if(!diff)
                    diff = sym[j].st_value;

                else if(diff < sym[j].st_value)
                    diff = sym[j].st_value;
            }

            if(diff)
                symbol_size = diff-sym[i].st_value;

            if(!symbol_size){

                if(sym[i].st_shndx >= header->e_shnum){
                    warn("invalid st_shndx\n");
                    return;
                }

                section = info->shdr + sym[i].st_shndx;
                diff = section->sh_addr+section->sh_size;

                if(sym[i].st_value > diff){
                    warn("invalid size\n");
                    return;
                }

                symbol_size = diff-sym[i].st_value;
            }
        }

        found = 1;
        break;
    }

    if(!found){
        warn("symbol '%s' not found\n", opts->symbol);
        return;
    }

    if(sym[i].st_shndx >= header->e_shnum){
        warn("invalid st_shndx\n");
        return;
    }

    section = info->shdr + sym[i].st_shndx;

    datadump(fh, section->sh_offset+sym[i].st_value-section->sh_addr, symbol_size, opts->fd_out, opts->raw);
}


void Arch(dumpsection)(struct extract_opts *opts, struct Arch(info_elf) *info, struct dynstr *shstrtab){
    struct io_utils *fh;
    ElfW(Shdr) *section;
    ElfW(Ehdr) *header;
    uint32_t i, index;

    index = get_section_index(shstrtab->ptr, shstrtab->size, opts->section);
    if(!index){
        warn("section %s not found\n", opts->section);
        return;
    }

    fh = &opts->fh;
    header = info->header;

    for(i=0; i<header->e_shnum; i++){
        section = info->shdr + i;

        if(section->sh_name == index){
            datadump(fh, section->sh_offset, section->sh_size, opts->fd_out, opts->raw);
            return;
        }
    }
}

void Arch(extract_shellcode)(ElfW(Ehdr) *header, struct extract_opts *opts){
    struct dynstr shstrtab;
    ElfW(Shdr) *shstrtab_header;
    struct Arch(info_elf) info;

    info.header = header;
    info.shdr = Arch(get_elf_shdr)(header, &opts->fh);
    info.phdr = Arch(get_elf_phdr)(header, &opts->fh);

    if(Arch(opts->size.addr_))
        Arch(dumpbyaddr)(opts, &info);

    if(!opts->section && !opts->symbol)
        return;


    /* get shstrtab */
    if(header->e_shstrndx >= header->e_shnum)
        die("invalid e_shstrndx\n");

    shstrtab_header = info.shdr + header->e_shstrndx;

    shstrtab.size = shstrtab_header->sh_size;
    if((shstrtab.ptr = malloc(shstrtab.size)) == NULL)
        err(1, "malloc");

    xpread(opts->fh.fd, shstrtab.ptr, shstrtab.size, shstrtab_header->sh_offset);

    if(opts->section)
        Arch(dumpsection)(opts, &info, &shstrtab);

    if(opts->symbol)
        Arch(dumpbysymbol)(opts, &info, &shstrtab);

}
