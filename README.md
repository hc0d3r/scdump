# scdump
just a tool to dump shellcode from elf

## Supported files

- [x] ELF 64-bit LSB
- [x] ELF 64-bit MSB
- [x] ELF 32-bit LSB
- [x] ELF 32-bit MSB

## Compile and install:

```
$ make
  CC obj/main.o
  CC obj/parser-elf-common.o
  CC obj/parser-elf-multiarch64.o
  CC obj/parser-elf-multiarch32.o
  CC obj/parser-mapfd.o
  CC obj/sc-extract64.o
  CC obj/sc-extract32.o
  CC obj/datadump.o
  CC obj/parser-elf-endian64.o
  CC obj/parser-elf-endian32.o
  CC scdump
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


## Support

If this project helped you, consider making a donation:

paypal: [![](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=RAG26EKAYHQSY&currency_code=BRL&source=url)

btc: 1PpbrY6j1HNPF7fS2LhG9SF2wtyK98GSwq
