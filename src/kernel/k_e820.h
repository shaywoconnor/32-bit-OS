#ifndef K_E820_H
#define K_E820_H

#define BIOS_E820_MAX_ENTRIES 128 
#define BIOS_E820_COUNT_ADDR 0xc0001000 //from MMAP_BASE_SEG (stage2.asm in bootloader) * 16
                                        //in future pass pointer to e820 map properly
                                        //this is a temporary hack
#define BIOS_E820_MAP_ADDR (BIOS_E820_COUNT_ADDR + 8)

#include <stdint.h>

struct K_E820_Entry
{
  uint64_t base_addr;
  uint64_t region_length;
  uint32_t region_type;
  uint32_t acpi_extended_reserved; //disregarding the acpi extended attributes, declared because stride must still be 24 bytes
                                   //linux reverted support for extended attrributes, as some bioses put 0's here incorrectly, i'm doing the same
} __attribute__((packed));
typedef struct K_E820_Entry K_E820_Entry;

struct K_E820_Map
{
  K_E820_Entry entries[BIOS_E820_MAX_ENTRIES * 2]; //worse case every region overlaps and we get 2*128 entries
  uint16_t entry_count;
};
typedef struct K_E820_Map K_E820_Map;

void k_e820_sanitize();
K_E820_Map* get_k_e820_map(void);

#endif // K_E820_H
