#include "k_gdt.h"

static uint64_t k_gdt[6] = 
{
  0x0000000000000000,
  0x00cf9a000000ffff,
  0x00cf92000000ffff,
  0x00cffa000000ffff,
  0x00cff2000000ffff,
  0x0000000000000000
};
static uint64_t k_gdtr;

void k_gdt_init(void)
{
  k_gdtr = ((uint64_t)(uint32_t)k_gdt << 16) | (sizeof(k_gdt) - 1);

  __asm__ volatile
  (
    "lgdt %0"
    :
    : "m"(k_gdtr)
    : "memory"
  );
}
