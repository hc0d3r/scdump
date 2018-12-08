# scdump
just a tool to dump elf shellcode

## Usage:

```
$ make
cc -o obj/main.o -c src/main.c
cc -DELFARCH=32 -o obj/elf-multiarch32.o -c src/elf-multiarch.c
cc -DELFARCH=64 -o obj/elf-multiarch64.o -c src/elf-multiarch.c
cc -o obj/elf-common.o -c src/elf-common.c
cc -o obj/io.o -c src/io.c
cc -o scdump obj/main.o obj/elf-multiarch32.o obj/elf-multiarch64.o obj/elf-common.o obj/io.o
$ ./scdump -h
-- scdump v0.1 --
Usage: scdump [file] [options]

Options:
  -S, --section <name>   Dump an entire section
  -s, --symbol <name>    Dump shellcode by symbol name
  -a, --addr <hexnumber,size>
                         Dump shellcode based on virtual address
  -o, --output <file>    Write to file instead of stdout
  -r, --raw              Raw output
  -h, --help             Display this help menu
```


## TODO

- [ ] add support for little-endian and big-endian, regardless of machine byte order
- [ ] fix an issue when the symbol value is equal to zero
