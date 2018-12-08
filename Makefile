.PHONY: clean install

INSTALLPROG?=install
INSTALLDIR?=/usr/bin


CFLAGS+=-Wall -Wextra -pie -fPIE -fstack-protector-all \
		-D_FORTIFY_SOURCE=2 -O2

LDFLAGS+=-Wl,-z,relro,-z,now

objs = 	obj/main.o obj/elf-multiarch32.o \
		obj/elf-multiarch64.o obj/elf-common.o \
		obj/io.o

scdump: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

obj/elf-multiarch32.o: src/elf-multiarch.c
	$(CC) $(CFLAGS) -DELFARCH=32 -o $@ -c $<

obj/elf-multiarch64.o: src/elf-multiarch.c
	$(CC) $(CFLAGS) -DELFARCH=64 -o $@ -c $<

obj/io.o: src/io.c
	$(CC) $(CFLAGS) -Wno-unused-result -o $@ -c $<


obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

install:
	$(INSTALLPROG) -s scdump $(INSTALLDIR)

clean:
	rm -f $(objs) scdump
