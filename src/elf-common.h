#ifndef __CHECK_ELF_H__
#define __CHECK_ELF_H__

#include <elf.h>

uint32_t get_section_index(const char *str, uint32_t len, const char *section);

#endif
