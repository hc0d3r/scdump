#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include "elf-multiarch.h"

union elfHeader {
    Elf64_Ehdr header_64;
    Elf32_Ehdr header_32;
};

struct scdump_options {
    struct extract_opts options;
    char *output;
    char *filename;
    int help;
};



#endif
