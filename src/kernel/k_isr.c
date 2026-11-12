#include "k_isr.h"

void k_isr_general_halt(void)
{
  __asm__ volatile ("cli");
  for (;;) __asm__ volatile ("hlt");
}

//temporary isrs that just print and halt, ugly repeated code, but the idea is later i can modify these independently
void k_isr_div_err_handler(void)
{
  k_print_vga_str("ERR: CPU DIV ERR\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_debug_handler(void)
{
  k_print_vga_str("ERR: CPU DEBUG\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_nmi_handler(void)
{
  k_print_vga_str("ERR: CPU NMI\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_breakpoint_handler(void)
{
  k_print_vga_str("ERR: CPU BREAKPOINT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_overflow_handler(void)
{
  k_print_vga_str("ERR: CPU OVERFLOW\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_bound_range_handler(void)
{
  k_print_vga_str("ERR: CPU BOUND RANGE\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_invalid_opcode_handler(void)
{
  k_print_vga_str("ERR: CPU INVALID OPCODE\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_device_not_available_handler(void)
{
  k_print_vga_str("ERR: CPU DEVICE NOT AVAILABLE\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_double_fault_handler(void)
{
  k_print_vga_str("ERR: CPU DOUBLE FAULT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_coprocessor_segment_overrun_handler(void)
{
  k_print_vga_str("ERR: CPU COPROCESSOR SEGMENT OVERRUN\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_invalid_tss_handler(void)
{
  k_print_vga_str("ERR: CPU INVALID TSS\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_segment_not_present_handler(void)
{
  k_print_vga_str("ERR: CPU SEGMENT NOT PRESENT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_stack_segment_fault_handler(void)
{
  k_print_vga_str("ERR: CPU STACK SEGMENT FAULT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_general_protection_handler(void)
{
  k_print_vga_str("ERR: CPU GENERAL PROTECTION\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_page_fault_handler(void)
{
  k_print_vga_str("ERR: CPU PAGE FAULT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_reserved_handler(void)
{
  k_print_vga_str("ERR: CPU RESERVED EXCEPTION\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_x87_floating_point_handler(void)
{
  k_print_vga_str("ERR: CPU X87 FLOATING POINT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_alignment_check_handler(void)
{
  k_print_vga_str("ERR: CPU ALIGNMENT CHECK\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_machine_check_handler(void)
{
  k_print_vga_str("ERR: CPU MACHINE CHECK\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_simd_floating_point_handler(void)
{
  k_print_vga_str("ERR: CPU SIMD FLOATING POINT\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_virtualization_handler(void)
{
  k_print_vga_str("ERR: CPU VIRTUALIZATION\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

void k_isr_no_isr(void)
{
  k_print_vga_str("INT RAISED BUT NO ISR\n", VGA_COLOUR_RED, VGA_COLOUR_GREEN);
  k_isr_general_halt();
}

static void (*k_isr_fptrs[256])(void) =
{
  [0] = k_isr_div_err_handler,
  [1] = k_isr_debug_handler,
  [2] = k_isr_nmi_handler,
  [3] = k_isr_breakpoint_handler,
  [4] = k_isr_overflow_handler,
  [5] = k_isr_bound_range_handler,
  [6] = k_isr_invalid_opcode_handler,
  [7] = k_isr_device_not_available_handler,
  [8] = k_isr_double_fault_handler,
  [9] = k_isr_coprocessor_segment_overrun_handler,
  [10] = k_isr_invalid_tss_handler,
  [11] = k_isr_segment_not_present_handler,
  [12] = k_isr_stack_segment_fault_handler,
  [13] = k_isr_general_protection_handler,
  [14] = k_isr_page_fault_handler,
  [15] = k_isr_reserved_handler,
  [16] = k_isr_x87_floating_point_handler,
  [17] = k_isr_alignment_check_handler,
  [18] = k_isr_machine_check_handler,
  [19] = k_isr_simd_floating_point_handler,
  [20] = k_isr_virtualization_handler,
  [21 ... 255] = k_isr_no_isr
};

void k_isr_handler(uint32_t vector)
{
  k_isr_fptrs[vector]();
}
