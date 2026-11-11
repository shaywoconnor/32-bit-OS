#ifndef K_IDT_H
#define K_IDT_H

#define K_IDT_ENTRY_COUNT 256

#include "k_isr.h"

typedef uint64_t k_idt_entry;
struct K_IDT
{
  k_idt_entry table[K_IDT_ENTRY_COUNT];
  uint64_t idtr;
};
typedef struct K_IDT K_IDT;

void k_idt_init(void);

#endif // K_IDT_H
