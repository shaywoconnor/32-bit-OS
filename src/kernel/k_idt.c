#include "k_idt.h"

//gcc required memset.. this is temporary
#include <stddef.h>

void* memset(void* dest, int value, size_t count)
{
  unsigned char* ptr = dest;

  for (size_t i = 0; i < count; i++)
    ptr[i] = (unsigned char)value;

  return dest;
}

static K_IDT k_idt;

void k_idt_init(void)
{
  //from osdev wiki, first 21 interrupts
  k_idt = (K_IDT)
  {
  {
    K_ISR_DIV_ERR_DESCRIPTOR,
    K_ISR_DEBUG_DESCRIPTOR,
    K_ISR_NMI_DESCRIPTOR,
    K_ISR_BREAKPOINT_DESCRIPTOR,
    K_ISR_OVERFLOW_DESCRIPTOR,
    K_ISR_BOUND_RANGE_DESCRIPTOR,
    K_ISR_INVALID_OPCODE_DESCRIPTOR,
    K_ISR_DEVICE_NOT_AVAILABLE_DESCRIPTOR,
    K_ISR_DOUBLE_FAULT_DESCRIPTOR,
    K_ISR_COPROCESSOR_SEGMENT_OVERRUN_DESCRIPTOR,
    K_ISR_INVALID_TSS_DESCRIPTOR,
    K_ISR_SEGMENT_NOT_PRESENT_DESCRIPTOR,
    K_ISR_STACK_SEGMENT_FAULT_DESCRIPTOR,
    K_ISR_GENERAL_PROTECTION_DESCRIPTOR,
    K_ISR_PAGE_FAULT_DESCRIPTOR,
    K_ISR_RESERVED_DESCRIPTOR,
    K_ISR_X87_FLOATING_POINT_DESCRIPTOR,
    K_ISR_ALIGNMENT_CHECK_DESCRIPTOR,
    K_ISR_MACHINE_CHECK_DESCRIPTOR,
    K_ISR_SIMD_FLOATING_POINT_DESCRIPTOR,
    K_ISR_VIRTUALIZATION_DESCRIPTOR
  },
  ((uint64_t)(uint32_t)&k_idt << 16) | ((sizeof(k_idt.table) - 1) & 0xFFFF)
  };
  __asm__ volatile
  (
    "lidt %0"
    :
    : "m"(k_idt.idtr)
    : "memory"
  );
}
