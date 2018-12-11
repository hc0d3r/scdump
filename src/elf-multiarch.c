#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <err.h>

#include "elf-multiarch.h"
#include "elf-common.h"
#include "io.h"
#include "common.h"

void arch_sufix(dumpbyaddr)(ElfW(Ehdr) *header, struct extract_opts *opts){
    ElfW(Phdr) pheader;
    uintX_t start, end;
    uint16_t i;

    start = arch_sufix(opts->addr.addr_);
    end = arch_sufix(opts->size.addr_);

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

    warn("address %" arch_sufix(PRIx) " not found\n", start);

}

void arch_sufix(dumpbysymbol)(ElfW(Ehdr) *header, struct extract_opts *opts, struct dynstr *shstrtab){
    ElfW(Shdr) buf[2], *symtab, *strtab;
    ElfW(Off) offset;
    ElfW(Shdr) section;
    ElfW(Sym) sym;
    char *sname;
    int found = 0;

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

    strtab_dump.size = strtab->sh_size;
    strtab_dump.ptr = malloc(strtab_dump.size);
    if(strtab_dump.ptr == NULL)
        err(1, "malloc");

    xpread(fh->fd, strtab_dump.ptr, strtab_dump.size, strtab->sh_offset);

    xset(fh, symtab->sh_offset);
    for(i=0; i<symtab->sh_size; i+=sizeof(sym)){
        xread(fh->fd, &sym, sizeof(sym));
        if(sym.st_name >= strtab_dump.size || !sym.st_name)
            continue;

        sname = strtab_dump.ptr+sym.st_name;
        if(!strcmp(sname, opts->symbol)){
            if(!sym.st_size){
                warn("%s has unknow size\n", opts->symbol);
                return;
            }

            found = 1;
            break;
        }
    }

    if(!found){
        warn("symbol '%s' not found\n", opts->symbol);
        return;
    }

    if(sym.st_shndx >= header->e_shnum){
        warn("invalid st_shndx\n");
        return;
    }

    offset = header->e_shoff+sym.st_shndx*sizeof(ElfW(Shdr));
    xset(fh, offset);
    xread(fh->fd, &section, sizeof(ElfW(Shdr)));

    datadump(fh, section.sh_offset+sym.st_value-section.sh_addr, sym.st_size, opts->fd_out, opts->raw);
}

void arch_sufix(dumpsection)(ElfW(Ehdr) *header, struct extract_opts *opts, struct dynstr *shstrtab){
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

void arch_sufix(extract_shellcode)(ElfW(Ehdr) *header, struct extract_opts *opts){
    struct dynstr shstrtab;
    ElfW(Shdr) shstrtab_section;
    ElfW(Off) shstr_offset;

    struct io_utils *fh = &opts->fh;

    if(arch_sufix(opts->size.addr_))
        arch_sufix(dumpbyaddr)(header, opts);

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
        arch_sufix(dumpsection)(header, opts, &shstrtab);

    if(opts->symbol)
        arch_sufix(dumpbysymbol)(header, opts, &shstrtab);

}
