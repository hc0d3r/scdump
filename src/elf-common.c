#include <elf.h>
#include <unistd.h>

#include "elf-common.h"

uint32_t get_section_index(const char *str, uint32_t len, const char *section){
    uint32_t i, j, k;

    for(i=1; i<len; i++){
        for(j=0, k=i; section[j] == str[k] && k < len; j++, k++){
            if(section[j+1] == 0x0 && section[j+1] == str[k+1] && str[i-1] == 0x0)
                return i;
        }
    }

    return 0;
}
