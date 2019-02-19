.PHONY: clean install

INSTALLPROG?=install
INSTALLDIR?=/usr/bin

CFLAGS+=-Wall -Wextra -pie -fPIE -fstack-protector-all \
		-D_FORTIFY_SOURCE=2 -O2

LDFLAGS+=-Wl,-z,relro,-z,now
VERSION := $(file <VERSION)

objs = 	obj/main.o obj/elf-multiarch32.o \
		obj/elf-multiarch64.o obj/elf-arch.o \
		obj/elf-common.o obj/io.o obj/datadump.o

all: scdump

VERSION:
	@[ -f ./VERSION ] && true || git describe > VERSION

scdump: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

obj/%32.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $< -DELFARCH=32

obj/%64.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $< -DELFARCH=64

obj/datadump.o: CFLAGS+=-Wno-unused-result
obj/main.o: CFLAGS+=-DVERSION=\"$(VERSION)\"

obj/%.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

install:
	$(INSTALLPROG) -s scdump $(INSTALLDIR)

clean:
	rm -f $(objs) scdump
