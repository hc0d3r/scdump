#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <err.h>

#include "elf-multiarch.h"
#include "elf-common.h"
#include "datadump.h"
#include "common.h"
#include "io.h"

void archS(dumpbyaddr)(ElfW(Ehdr) *header, struct extract_opts *opts){
    ElfW(Phdr) pheader;
    uintX_t start, end;
    uint16_t i;

    start = archS(opts->addr.addr_);
    end = archS(opts->size.addr_);

    if(!header->e_phoff)
        return;

    struct io_utils *fh = &opts->fh;

    xset(fh, header->e_phoff);
    for(i=0; i<header->e_phnum; i++){
        xread(fh->fd, &pheader, sizeof(pheader));

        if(start >= pheader.p_vaddr && pheader.p_vaddr+pheader.p_filesz >= start+end){
            datadump(fh, pheader.p_offset+start-pheader.p_vaddr, end, opts->fd_out, opts->raw);
            return;
        }
    }

    warn("address %" archS(PRIx) " not found\n", start);

}

void archS(dumpbysymbol)(ElfW(Ehdr) *header, struct extract_opts *opts, struct dynstr *shstrtab){
    ElfW(Shdr) buf[2], *symtab, *strtab;
    ElfW(Off) offset;
    ElfW(Shdr) section;
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

    symtab = NULL;
    strtab = NULL;
    j = 0;

    xset(fh, header->e_shoff);
    for(i=0; i<header->e_shnum && j<2; i++){
        xread(fh->fd, &(buf[j]), sizeof(ElfW(Shdr)));
        if(buf[j].sh_name == strindex){
            strtab = &(buf[j++]);
        } else if(buf[j].sh_name == symindex){
            symtab = &(buf[j++]);
        }
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

        if(!strcmp(sname, opts->symbol)){
            if(!sym[i].st_size){
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
                    xset(fh, header->e_shoff+(sizeof(section)*sym[i].st_shndx));
                    xread(fh->fd, &section, sizeof(section));

                    diff = section.sh_addr+section.sh_size;

                    if(sym[i].st_value > diff){
                        warn("invalid size\n");
                        return;
                    }

                    symbol_size = diff-sym[i].st_value;
                }
            } else {
                symbol_size = sym[i].st_size;
            }

            found = 1;
            break;
        }
    }

    if(!found){
        warn("symbol '%s' not found\n", opts->symbol);
        return;
    }

    if(sym[i].st_shndx >= header->e_shnum){
        warn("invalid st_shndx\n");
        return;
    }

    offset = header->e_shoff+sym[i].st_shndx*sizeof(ElfW(Shdr));
    xset(fh, offset);
    xread(fh->fd, &section, sizeof(ElfW(Shdr)));

    datadump(fh, section.sh_offset+sym[i].st_value-section.sh_addr, symbol_size, opts->fd_out, opts->raw);
}

void archS(dumpsection)(ElfW(Ehdr) *header, struct extract_opts *opts, struct dynstr *shstrtab){
    ElfW(Shdr) section;
    uint32_t i, index;

    index = get_section_index(shstrtab->ptr, shstrtab->size, opts->section);
    if(!index){
        warn("section %s not found\n", opts->section);
        return;
    }

    struct io_utils *fh = &opts->fh;

    xset(fh, header->e_shoff);
    for(i=0; i<header->e_shnum; i++){
        xread(fh->fd, &section, sizeof(ElfW(Shdr)));
        if(section.sh_name == index){
            datadump(fh, section.sh_offset, section.sh_size, opts->fd_out, opts->raw);
            return;
        }
    }
}

void archS(extract_shellcode)(ElfW(Ehdr) *header, struct extract_opts *opts){
    struct dynstr shstrtab;
    ElfW(Shdr) shstrtab_section;
    ElfW(Off) shstr_offset;

    struct io_utils *fh = &opts->fh;

    if(archS(opts->size.addr_))
        archS(dumpbyaddr)(header, opts);

    if(!opts->section && !opts->symbol)
        return;

    /* get shstrtab */

    shstr_offset = header->e_shoff+header->e_shstrndx*sizeof(ElfW(Shdr));
    if(shstr_offset == 0){
        die("shstrtab null offset\n");
    }

    xset(fh, shstr_offset);
    xread(fh->fd, &shstrtab_section, sizeof(ElfW(Shdr)));

    shstrtab.size = shstrtab_section.sh_size;
    shstrtab.ptr = malloc(shstrtab.size);
    if(shstrtab.ptr == NULL)
        err(1, "malloc");

    xpread(fh->fd, shstrtab.ptr, shstrtab.size, shstrtab_section.sh_offset);

    if(opts->section)
        archS(dumpsection)(header, opts, &shstrtab);

    if(opts->symbol)
        archS(dumpbysymbol)(header, opts, &shstrtab);

}
