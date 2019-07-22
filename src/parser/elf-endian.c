#include "elf-endian.h"
#include "elf-types.h"

void normalize(void *elfptr, size_t len){
    Elf_Ehdr *header;
    Elf_Phdr *phdr;
    Elf_Shdr *shdr;

    uint16_t i;

    header = elfptr;
    bswap_16(header->e_type);
    bswap_16(header->e_machine);
    bswap_32(header->e_version);
    bswap_xx(header->e_entry);
    bswap_xx(header->e_phoff);
    bswap_xx(header->e_shoff);
    bswap_32(header->e_flags);
    bswap_16(header->e_ehsize);
    bswap_16(header->e_phentsize);
    bswap_16(header->e_phnum);
    bswap_16(header->e_shentsize);
    bswap_16(header->e_shnum);
    bswap_16(header->e_shstrndx);

    sanity_check(len, header->e_phoff+sizeof(Elf_Phdr)*header->e_phnum);
    sanity_check(len, header->e_shoff+sizeof(Elf_Shdr)*header->e_shnum);

    phdr = elfptr+header->e_phoff;
    for(i=0; i<header->e_phnum; i++){
        bswap_32(phdr->p_type);
        bswap_32(phdr->p_flags);
        bswap_xx(phdr->p_offset);
        bswap_xx(phdr->p_vaddr);
        bswap_xx(phdr->p_paddr);
        bswap_xx(phdr->p_filesz);
        bswap_xx(phdr->p_memsz);
        bswap_xx(phdr->p_align);

        phdr++;
    }

    shdr = elfptr+header->e_shoff;
    for(i=0; i<header->e_shnum; i++){
        bswap_32(shdr->sh_name);
        bswap_32(shdr->sh_type);
        bswap_xx(shdr->sh_flags);
        bswap_xx(shdr->sh_addr);
        bswap_xx(shdr->sh_offset);
        bswap_xx(shdr->sh_size);
        bswap_32(shdr->sh_link);
        bswap_32(shdr->sh_info);
        bswap_xx(shdr->sh_addralign);
        bswap_xx(shdr->sh_entsize);

        shdr++;
    }
}


void symnormalize(Elf_Sym *symbols, size_t len){
    size_t i;

    for(i=0; i<len/sizeof(Elf_Sym); i++){
        bswap_32(symbols->st_name);
        bswap_16(symbols->st_shndx);
        bswap_xx(symbols->st_value);
        bswap_xx(symbols->st_size);
        symbols++;
    }
}
