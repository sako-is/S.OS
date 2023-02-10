# Building

Clone limine binaries into `limine/` and run `make -C limine`

```
make
cp -v kernel.elf limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso/

xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot \
    limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso -o image.iso

./limine/limine-deploy image.iso
```