#ifndef K_GDT_H
#define K_GDT_H
#define K_CODE_SEG 0x08
#define K_DATA_SEG 0x10

#include <stdint.h>

void k_gdt_init(void);
#endif // K_GDT_H
