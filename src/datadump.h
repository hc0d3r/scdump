#ifndef __DATADUMP_H__
#define __DATADUMP_H__

#include "io.h"

void datadump(struct io_utils *fh, off_t offset, size_t len, int fdout, int raw);


#endif
