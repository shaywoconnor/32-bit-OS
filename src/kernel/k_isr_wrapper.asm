global k_isr_wrapper
align 4

extern k_isr_handler

%macro CREATE_STUB 2
global k_isr_%1
k_isr_%1:
  pushad
  push ds
  push es
  push fs
  push gs
  push %2
  cld
  call k_isr_handler
  add esp, 4
  pop gs
  pop fs
  pop es
  pop ds
  popad
  iret
%endmacro

%macro CREATE_STUB_ERR 2
global k_isr_%1
k_isr_%1:
  pushad
  push ds
  push es
  push fs
  push gs
  push %2
  cld
  call k_isr_handler
  add esp, 4
  pop gs
  pop fs
  pop es
  pop ds
  popad
  add esp, 4
  iret
%endmacro

CREATE_STUB div_err, 0
CREATE_STUB debug, 1
CREATE_STUB nmi, 2
CREATE_STUB breakpoint, 3
CREATE_STUB overflow, 4
CREATE_STUB bound_range, 5
CREATE_STUB invalid_opcode, 6
CREATE_STUB device_not_available, 7
CREATE_STUB_ERR double_fault, 8
CREATE_STUB coprocessor_segment_overrun, 9
CREATE_STUB_ERR invalid_tss, 10
CREATE_STUB_ERR segment_not_present, 11
CREATE_STUB_ERR stack_segment_fault, 12
CREATE_STUB_ERR general_protection, 13
CREATE_STUB_ERR page_fault, 14
CREATE_STUB reserved, 15
CREATE_STUB x87_floating_point, 16
CREATE_STUB_ERR alignment_check, 17
CREATE_STUB machine_check, 18
CREATE_STUB simd_floating_point, 19
CREATE_STUB virtualization, 20
