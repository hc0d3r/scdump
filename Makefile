.PHONY: clean install

INSTALLPROG?=install
INSTALLDIR?=/usr/bin

CFLAGS+=-Wall -Wextra -pie -fPIE -fstack-protector-all \
		-D_FORTIFY_SOURCE=2 -O2

LDFLAGS+=-Wl,-z,relro,-z,now

objs = 	obj/main.o obj/elf-multiarch32.o \
		obj/elf-multiarch64.o obj/elf-common.o \
		obj/io.o obj/datadump.o

scdump: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

obj/elf-multiarch32.o: src/elf-multiarch.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $< -DELFARCH=32

obj/elf-multiarch64.o: src/elf-multiarch.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $< -DELFARCH=64

obj/datadump.o: CFLAGS+=-Wno-unused-result

obj/%.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

install:
	$(INSTALLPROG) -s scdump $(INSTALLDIR)

clean:
	rm -f $(objs) scdump
