.PHONY: clean install

INSTALLPROG?=install
INSTALLDIR?=/usr/bin

CFLAGS+=-Wall -Wextra

LDFLAGS+=-Wl,-z,relro,-z,now
VERSION := $(file <VERSION)
Q = @

objs = 	obj/main.o obj/parser-elf-common.o \
		obj/parser-elf-multiarch64.o obj/parser-elf-multiarch32.o \
		obj/parser-mapfd.o obj/sc-extract64.o obj/sc-extract32.o \
		obj/datadump.o obj/parser-elf-endian64.o obj/parser-elf-endian32.o

ifeq ($(V),1)
	Q =
endif

ifeq ($(NH),)
	CFLAGS += -pie -fPIE -fstack-protector-all -D_FORTIFY_SOURCE=2 -O2
	LDFLAGS += -Wl,-z,relro,-z,now
endif

all: scdump

scdump: $(objs)
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

obj/datadump.o: CFLAGS+=-Wno-unused-result
obj/main.o: CFLAGS+=-DVERSION=\"$(VERSION)\"
obj/%64.o: CFLAGS+=-DELFARCH=64
obj/%32.o: CFLAGS+=-DELFARCH=32

obj/%32.o: src/%.c
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

obj/%64.o: src/%.c
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

obj/parser-%64.o: src/parser/%.c
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

obj/parser-%32.o: src/parser/%.c
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

obj/parser-%.o: src/parser/%.c
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

obj/%.o: src/%.c
	@echo "  CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

install:
	$(INSTALLPROG) -s scdump $(INSTALLDIR)

clean:
	rm -f $(objs) scdump
