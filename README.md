# Building

Clone limine binaries into `limine/` and run `make -C limine`

Compile one of the fonts (this makefile uses Tamsyn) into `kernel/` using `objcpy`
```
objcopy -O elf64-x86-64 -B i386 -I binary assets/Tamsyn8x16r.psf kernel/tamsyn.o
```
