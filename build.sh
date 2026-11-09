#!/bin/sh
set -e

SRC=src
OUT=build
mkdir -p $OUT

nasm -f bin $SRC/bootloader/stage1.asm -o $OUT/stage1.bin
nasm -f bin $SRC/bootloader/stage2.asm -o $OUT/stage2.bin
CFLAGS="-m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
        -fno-asynchronous-unwind-tables -nostdlib -O2 -Wall -Wextra"

for f in $SRC/kernel/*.c; do
  gcc $CFLAGS -c "$f" -o "$OUT/$(basename "${f%.c}").o"
done
ld -m elf_i386 -T $SRC/kernel/link.ld -o $OUT/kernel.elf $OUT/*.o

rm -f $OUT/os.img
truncate -s 1G $OUT/os.img
mkfs.fat -F 32 -s 8 -R 32 -n OS $OUT/os.img
MTOOLS_SKIP_CHECK=1 mcopy -i $OUT/os.img $OUT/kernel.elf ::/KERNEL.ELF

dd if=$OUT/stage1.bin of=$OUT/os.img bs=1 count=3 conv=notrunc
dd if=$OUT/stage1.bin of=$OUT/os.img bs=1 skip=90 seek=90 count=420 conv=notrunc
dd if=$OUT/stage2.bin of=$OUT/os.img bs=512 seek=8 conv=notrunc

qemu-system-i386 -drive file=$OUT/os.img,format=raw
