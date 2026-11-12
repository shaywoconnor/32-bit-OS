## 32 bit protected mode OS 

![paging and page fault](misc/happy_to_see_a_page_fault.png)

# Custom Bootloader

Loads the kernel from an elf, parsing program headers to correctly load the kernel

Uses unreal mode to load the kernel (From FAT32) into higher memory while in real mode

Maps memory using e820, enables a20 line, creates temporary GDT, and loads protected mode

Successfully boots into a test kernel that prints a welcome message

# Kernel

Assembly stub creates a page directory and pages the kernel into 0xc0000000

Creates flat GDT

Initialises IDT with CPU exceptions, no IRQ yet

Successfully writes text to VGA text buffer mapped to 0xc00b8000

Intentionally triggers a page fault by dereferencing unmapped 0x00, successfully printing the error message
