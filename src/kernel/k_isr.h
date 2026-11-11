#ifndef K_ISR_STUBS_H
#define K_ISR_STUBS_H

#include "k_print.h"
#include "k_gdt.h"

#define K_ISR_DEF(_name) extern void k_isr_##_name(void);
#define K_ISR_DESCRIPTOR(_name, _segment, _flags) \
  ((uint64_t)((uintptr_t)k_isr_##_name & 0xffff0000) << 32) | \
  ((uint64_t)(_flags & 0xff) << 40) | \
  ((uint64_t)(_segment & 0xffff) << 16) | \
  (((uintptr_t)k_isr_##_name & 0x0000ffff))

K_ISR_DEF(div_err);
#define K_ISR_DIV_ERR_DESCRIPTOR K_ISR_DESCRIPTOR(div_err, K_CODE_SEG, 0b10001110)

K_ISR_DEF(debug);
#define K_ISR_DEBUG_DESCRIPTOR K_ISR_DESCRIPTOR(debug, K_CODE_SEG, 0b10001110)

K_ISR_DEF(nmi);
#define K_ISR_NMI_DESCRIPTOR K_ISR_DESCRIPTOR(nmi, K_CODE_SEG, 0b10001110)

K_ISR_DEF(breakpoint);
#define K_ISR_BREAKPOINT_DESCRIPTOR K_ISR_DESCRIPTOR(breakpoint, K_CODE_SEG, 0b10001110)

K_ISR_DEF(overflow);
#define K_ISR_OVERFLOW_DESCRIPTOR K_ISR_DESCRIPTOR(overflow, K_CODE_SEG, 0b10001110)

K_ISR_DEF(bound_range);
#define K_ISR_BOUND_RANGE_DESCRIPTOR K_ISR_DESCRIPTOR(bound_range, K_CODE_SEG, 0b10001110)

K_ISR_DEF(invalid_opcode);
#define K_ISR_INVALID_OPCODE_DESCRIPTOR K_ISR_DESCRIPTOR(invalid_opcode, K_CODE_SEG, 0b10001110)

K_ISR_DEF(device_not_available);
#define K_ISR_DEVICE_NOT_AVAILABLE_DESCRIPTOR K_ISR_DESCRIPTOR(device_not_available, K_CODE_SEG, 0b10001110)

K_ISR_DEF(double_fault);
#define K_ISR_DOUBLE_FAULT_DESCRIPTOR K_ISR_DESCRIPTOR(double_fault, K_CODE_SEG, 0b10001110)

K_ISR_DEF(coprocessor_segment_overrun);
#define K_ISR_COPROCESSOR_SEGMENT_OVERRUN_DESCRIPTOR K_ISR_DESCRIPTOR(coprocessor_segment_overrun, K_CODE_SEG, 0b10001110)

K_ISR_DEF(invalid_tss);
#define K_ISR_INVALID_TSS_DESCRIPTOR K_ISR_DESCRIPTOR(invalid_tss, K_CODE_SEG, 0b10001110)

K_ISR_DEF(segment_not_present);
#define K_ISR_SEGMENT_NOT_PRESENT_DESCRIPTOR K_ISR_DESCRIPTOR(segment_not_present, K_CODE_SEG, 0b10001110)

K_ISR_DEF(stack_segment_fault);
#define K_ISR_STACK_SEGMENT_FAULT_DESCRIPTOR K_ISR_DESCRIPTOR(stack_segment_fault, K_CODE_SEG, 0b10001110)

K_ISR_DEF(general_protection);
#define K_ISR_GENERAL_PROTECTION_DESCRIPTOR K_ISR_DESCRIPTOR(general_protection, K_CODE_SEG, 0b10001110)

K_ISR_DEF(page_fault);
#define K_ISR_PAGE_FAULT_DESCRIPTOR K_ISR_DESCRIPTOR(page_fault, K_CODE_SEG, 0b10001110)

K_ISR_DEF(reserved);
#define K_ISR_RESERVED_DESCRIPTOR K_ISR_DESCRIPTOR(reserved, K_CODE_SEG, 0b10001110)

K_ISR_DEF(x87_floating_point);
#define K_ISR_X87_FLOATING_POINT_DESCRIPTOR K_ISR_DESCRIPTOR(x87_floating_point, K_CODE_SEG, 0b10001110)

K_ISR_DEF(alignment_check);
#define K_ISR_ALIGNMENT_CHECK_DESCRIPTOR K_ISR_DESCRIPTOR(alignment_check, K_CODE_SEG, 0b10001110)

K_ISR_DEF(machine_check);
#define K_ISR_MACHINE_CHECK_DESCRIPTOR K_ISR_DESCRIPTOR(machine_check, K_CODE_SEG, 0b10001110)

K_ISR_DEF(simd_floating_point);
#define K_ISR_SIMD_FLOATING_POINT_DESCRIPTOR K_ISR_DESCRIPTOR(simd_floating_point, K_CODE_SEG, 0b10001110)

K_ISR_DEF(virtualization);
#define K_ISR_VIRTUALIZATION_DESCRIPTOR K_ISR_DESCRIPTOR(virtualization, K_CODE_SEG, 0b10001110)

#endif // K_ISR_STUBS_H
