#include <stdint.h>

#include "k_print.h"
#include "k_idt.h"
#include "k_gdt.h"
#include "k_e820.h"

static void u64_to_hex(uint64_t value, char* out)
{
  for (int i = 15; i >= 0; i--)
  {
    out[i] = "0123456789ABCDEF"[value & 0xF];
    value >>= 4;
  }
  out[16] = '\0';
}

static void k_e820_dump(void)
{
  K_E820_Map* map = get_k_e820_map();
  char line[40];
  for (uint32_t i = 0; i < map->entry_count; i++)
  {
    u64_to_hex(map->entries[i].base_addr, line);
    line[16] = ' ';
    u64_to_hex(map->entries[i].region_length, line + 17);
    line[33] = ' ';
    line[34] = "0123456789ABCDEF"[map->entries[i].region_type & 0xF];
    line[35] = '\n';
    line[36] = '\0';
    k_print_vga_str(line, VGA_COLOUR_BLACK, VGA_COLOUR_WHITE);
  }
}

void k_init(void)
{
  k_gdt_init();
  k_idt_init();
  k_e820_sanitize();
}

void k_entry_main()
{
  k_init();
  k_print_vga_cls(VGA_COLOUR_BLACK);
  k_print_vga_str("Sanitized E820 map contents: \n", VGA_COLOUR_BLACK, VGA_COLOUR_RED);
  k_print_vga_str("BASE             LENGTH           T\n", VGA_COLOUR_BLACK, VGA_COLOUR_LIGHT_CYAN);
  k_e820_dump();
  //k_user_handoff();
  for (;;) {}
}
