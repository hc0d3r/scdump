#ifndef __DATADUMP_H__
#define __DATADUMP_H__

#include <stddef.h>

void datadump(int fdout, int raw, const void *data, size_t len);

#endif
