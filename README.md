# Building

Clone limine binaries into `limine/` and run `make -C limine` (currently using v. 5.x):
```
git clone https://github.com/limine-bootloader/limine.git --branch=v5.x-branch-binary --depth=1
```
Dependencies:
```
coreutils + clang
xorriso
qemu
nasm
```


To build OS:
```
make
```

To run OS:
```
./run.sh
```