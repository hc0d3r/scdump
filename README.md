# scdump
just a tool to dump shellcode from elf

## Compile and install:

```
$ make
  CC src/main.c
  CC src/elf-multiarch.c
  CC src/elf-multiarch.c
  CC src/elf-common.c
  CC src/io.c
cc -Wall -Wextra -pie -fPIE -fstack-protector-all -D_FORTIFY_SOURCE=2 -O2 -Wl,-z,relro,-z,now -o scdump obj/main.o obj/elf-multiarch32.o obj/elf-multiarch64.o obj/elf-common.o obj/io.o
$ sudo make install
install -s scdump /usr/bin
```

### options:
```
CC - compiler to use
INSTALLPROG - program to be used for installation (Default: install)
INSTALLDIR - dir to install the program (Default: /usr/bin)
```

## Usage examples:

For these examples, I will use the following source code:

```asm
; test.asm

section .data
    db 'this come from .data', 0xa
section .rodata
    db 'this come from .rodata', 0xa
section .text
    global _start
_start:
    db 'this come from .text', 0xa
xyz:
    db 'dumping some shellcode from a symbol name !', 0xa
```

building:
```
$ nasm -f elf32 test.asm -o test.o
$ ld -o test test.o -m elf_i386
```
__Dumping a section:__

```
$ scdump -S .data test
\x74\x68\x69\x73\x20\x63\x6f\x6d\x65\x20\x66\x72\x6f\x6d\x20\x2e\x64\x61\x74\x61\x0a
```

if you want non formated output just use -r option:

```
$ scdump -S .data test -r
this come from .data
$ scdump -S .rodata test -r
this come from .rodata
$ scdump -S .text test -r
this come from .text
dumping some shellcode from a symbol name !
```

__Dumping shellcode by symbol name:__

```
$ scdump -s _start test -r
this come from .text
$ scdump -s xyz test -r
dumping some shellcode from a symbol name !
```

__Dumping shellcode based on virtual address:__

```
$ readelf -S test | grep -i .rodata
  [ 2] .rodata           PROGBITS        080480c4 0000c4 000017 00   A  0   0  4
$ scdump -a 080480c4,23 test -r
this come from .rodata
```

note that 0x17 becomes 23 in decimal


## TODO

- [ ] add support for little-endian and big-endian, regardless of machine byte order
