#include "k_e820.h"

static K_E820_Map k_e820_map = (K_E820_Map) { {0}, 0 };

void sort_limits(uint64_t* limits, uint16_t limit_count)
{
  //insertion sort
  //benched insertion sort >2us faster for <256 limits than heap sort, assuming a near-sorted e820_map
  for (uint16_t i = 1; i < limit_count; i++)
  {
    uint64_t key = limits[i];
    int16_t j = (int16_t)i - 1;
    while (j >= 0 && limits[j] > key)
    {
      limits[j+1] = limits[j];
      j--;
    }
    limits[j+1] = key;
  }
}

//e820 is horrible and reports info for overlapping memory regions so this function is needed
void k_e820_sanitize()
{
  K_E820_Entry* e820_map = (K_E820_Entry*)(BIOS_E820_MAP_ADDR);
  uint32_t entry_count = *((volatile uint32_t*)BIOS_E820_COUNT_ADDR);
  k_e820_map.entry_count = 0;
  if (entry_count == 0 || entry_count > BIOS_E820_MAX_ENTRIES) return;

  uint16_t limit_count = (uint16_t)entry_count * 2;
  uint64_t limits[BIOS_E820_MAX_ENTRIES * 2] = {0};
  for (int i = 0, c = 0; i < entry_count; i++, c+=2)
  {
    limits[c] = e820_map[i].base_addr;
    limits[c+1] = e820_map[i].base_addr + e820_map[i].region_length;
  }
  sort_limits(limits, limit_count);
  uint16_t out = 0;
  for (uint32_t i = 0; i < limit_count - 1; i++)
  {
    uint64_t interval_low = limits[i];
    uint64_t interval_high = limits[i+1];
    if (interval_low == interval_high) continue;
    uint32_t most_restrictive_type = 0;
    for (uint32_t j = 0; j < entry_count; j++)
    {
      K_E820_Entry entry = e820_map[j];
      if (entry.base_addr <= interval_low && entry.base_addr + entry.region_length >= interval_high)
      {
        if (entry.region_type > most_restrictive_type) most_restrictive_type = entry.region_type;
      }
    }
    if (most_restrictive_type == 0) continue;
    if (out >= BIOS_E820_MAX_ENTRIES * 2) break;
    k_e820_map.entries[out++] = (K_E820_Entry){interval_low, interval_high - interval_low, most_restrictive_type, 0};
  }
  k_e820_map.entry_count = out;
}

K_E820_Map* get_k_e820_map(void)
{
  return &k_e820_map;
}
