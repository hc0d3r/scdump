#ifndef __IO_H__
#define __IO_H__

#include <sys/types.h>

typedef struct {
    int fd;
    off_t size;
} fh_t;

void rdonly(const char *filename, fh_t *out);
void xset(fh_t *fh, off_t offset);
void xread(int fd, void *buf, size_t count);
void xpread(int fd, void *buf, size_t count, off_t offset);

#endif
