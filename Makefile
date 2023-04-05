override KERNEL := kernel.elf

CC := clang
LD := ld.lld

CFLAGS ?= -g -O3 -pipe -Wall -Wextra
NASMFLAGS ?= -F dwarf -g
LDFLAGS ?= 

override CFLAGS +=			\
	-std=c18				\
	-ffreestanding			\
	-fno-stack-protector	\
	-fno-stack-check		\
	-fno-lto				\
	-fno-pie				\
	-fno-pic				\
	-m64					\
	-target x86_64-none-elf	\
	-march=x86-64			\
	-mabi=sysv				\
	-mcmodel=kernel			\
	-MMD					\
	-I.						\
	-Iinclude/

override LDFLAGS +=			\
	-nostdlib				\
	-static					\
	-m elf_x86_64			\
	-z max-page-size=0x1000	\
	-T linker.ld

override NASMFLAGS +=		\
	-f elf64

override CFILES := $(shell find kernel -type f -name '*.c')
override ASFILES := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')
override OBJ := $(CFILES:.c=.o) $(ASFILES:.S=.o) $(NASMFILES:.asm=.o)
override HEADER_DEPS := $(CFILES:.c=.d) $(ASFILES:.S=.d)

.PHONY: all
all: $(KERNEL)
	cp -v kernel.elf limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso/
	xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso -o image.iso
	./limine/limine-deploy image.iso
	make clean

$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) bin/tamsyn.o -o $@

-include $(HEADER_DEPS)

%.o: %.c limine.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S limine.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)
