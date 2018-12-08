#ifndef __COMMON_H__
#define __COMMON_H__

#define warn(x...) fprintf(stderr, x);
#define die(x...) do { warn(x); exit(1); } while(0)
#define pdie(x) do { perror(x); exit(1); } while(0)

#endif
