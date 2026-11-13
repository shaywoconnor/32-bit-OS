## 32 bit protected mode OS 

![e820 map sanitized and printed](progress_screenshots/e820_sanitized.png)

# Custom Bootloader

Loads the kernel from an elf, parsing program headers to correctly load the kernel

Uses unreal mode to load the kernel (From FAT32) into higher memory while in real mode

Maps memory using e820, enables a20 line, creates temporary GDT, and loads protected mode

Successfully boots into a test kernel that prints a welcome message

# Kernel

Assembly stub creates a page directory and pages the kernel into 0xc0000000

Creates flat GDT

Initialises IDT with CPU exceptions, no IRQ yet

Uses BIOS E820 memory map to create a new, "sanitized" one, sorted with no overlaps 

