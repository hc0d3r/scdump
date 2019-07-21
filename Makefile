.PHONY: clean install

INSTALLPROG?=install
INSTALLDIR?=/usr/bin

CFLAGS+=-Wall -Wextra -pie -fPIE -fstack-protector-all \
		-D_FORTIFY_SOURCE=2 -O2

LDFLAGS+=-Wl,-z,relro,-z,now
VERSION := $(file <VERSION)

objs = 	obj/main.o obj/parser-elf-common.o \
		obj/parser-elf-multiarch64.o obj/parser-elf-multiarch32.o \
		obj/parser-mapfd.o obj/sc-extract64.o obj/sc-extract32.o \
		obj/datadump.o

all: scdump

VERSION:
	@[ -f ./VERSION ] && true || git describe > VERSION

scdump: $(objs)
	@echo "  CC scdump"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

obj/datadump.o: CFLAGS+=-Wno-unused-result
obj/main.o: CFLAGS+=-DVERSION=\"$(VERSION)\"
obj/%64.o: CFLAGS+=-DELFARCH=64
obj/%32.o: CFLAGS+=-DELFARCH=32

obj/%32.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

obj/%64.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

obj/parser-%64.o: src/parser/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

obj/parser-%32.o: src/parser/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<


obj/parser-%.o: src/parser/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

obj/%.o: src/%.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

install:
	$(INSTALLPROG) -s scdump $(INSTALLDIR)

clean:
	rm -f $(objs) scdump
