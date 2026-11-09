ENTRY(k_entry_main)
SECTIONS {
  . = 1M;
  .text : { *(.text*) }
  .data : { *(.data*) }
  .bss  : { *(.bss*) }
}
