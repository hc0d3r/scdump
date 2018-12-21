#ifndef __IO_H__
#define __IO_H__

#include <sys/types.h>

struct io_utils {
    int fd;
    off_t size;
};

void rdonly(const char *filename, struct io_utils *out);
void xset(struct io_utils *fh, off_t offset);
void xread(int fd, void *buf, size_t count);
void xpread(int fd, void *buf, size_t count, off_t offset);

#endif
