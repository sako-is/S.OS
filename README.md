# Building

Clone limine binaries into `limine/` and run `make -C limine`

Compile one of the fonts (this makefile uses Tamsyn) into `kernel/` using `objcpy`
```
objcopy -O elf64-x86-64 -B i386 -I binary assets/Tamsyn8x16r.psf kernel/tamsyn.o
```

```
make
cp -v kernel.elf limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso/

xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot \
    limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso -o image.iso

./limine/limine-deploy image.iso
```