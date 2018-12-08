.PHONY: clean

objs = 	obj/main.o obj/elf-multiarch32.o \
		obj/elf-multiarch64.o obj/elf-common.o \
		obj/io.o


scdump: $(objs)
	$(CC) -o $@ $^

obj/elf-multiarch32.o: src/elf-multiarch.c
	$(CC) -DELFARCH=32 -o $@ -c $<

obj/elf-multiarch64.o: src/elf-multiarch.c
	$(CC) -DELFARCH=64 -o $@ -c $<

obj/%.o: src/%.c
	$(CC) -o $@ -c $<



clean:
	rm -f $(objs) scdump
