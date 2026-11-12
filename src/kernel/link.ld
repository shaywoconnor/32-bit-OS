ENTRY(_start)
SECTIONS 
{
  . = 0x100000;
  .boot :
  {
    build/k_head_32.o(.boot) /* preventing myself from later making some .boot section that gets linked to low memory*/
  }
  . += 0xc0000000;
  .text ALIGN(4096) : AT(ADDR(.text) - 0xc0000000)
  {
    *(.text)
    *(.text.*)
  }
  .rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xc0000000)
  {
    *(.rodata)
    *(.rodata.*)
  }
  .data ALIGN(4096) : AT(ADDR(.data) - 0xc0000000)
  {
    *(.data)
    *(.data.*)
  }
  .bss ALIGN(4096) : AT(ADDR(.bss) - 0xc0000000)
  {
    __bss_start = .;
    *(COMMON)
    *(.bss)
    *(.bss.*)
    __bss_end = .;
  }
  . = ALIGN(4096);
  __kernel_end = .; /* lets me page an arbitrarily long kernel in the head*/
}
