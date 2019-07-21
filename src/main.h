#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include "sc-extract.h"

struct scdump_options {
    struct extract_opts options;
    char *output;
    char *filename;
    int help;
};

#endif
