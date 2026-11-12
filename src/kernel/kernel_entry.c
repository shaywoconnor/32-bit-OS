#include <stdint.h>

#include "k_print.h"
#include "k_idt.h"
#include "k_gdt.h"

void k_hello(void)
{
  k_print_vga_cls(VGA_COLOUR_CYAN);
  k_print_vga_str("Hello, World!\n", VGA_COLOUR_CYAN, VGA_COLOUR_RED);
}

void k_init(void)
{
  k_gdt_init();
  k_idt_init();
}

void k_entry_main(void)
{
  k_init();
  k_hello();
  volatile uint32_t should_page_fault = *(volatile uint32_t*)(0x00);
  (void)should_page_fault;
  uint32_t should_interrupt_div_err_test = 32 / 0;

  //k_user_handoff();
  for (;;) {}
}
