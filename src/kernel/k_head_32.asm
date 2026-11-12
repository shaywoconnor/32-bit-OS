bits 32
extern __kernel_end ;kernel end defined in link.ld
extern k_entry_main
section .bss
align 4096
boot_page_directory:
  resb 4096
align 4096
stack_bottom:
  resb 16384
stack_top:
page_tables_end:
  resd 1

section .boot
global _start
_start:
  mov edi, boot_page_directory - 0xc0000000
  mov ecx, 1024
  xor eax, eax
  cld
  rep stosd
  mov edi, __kernel_end - 0xc0000000
  mov edx, boot_page_directory - 0xc0000000
  xor eax, eax
.next_table:
  lea ecx, [edi + 0x03]
  mov [edx], ecx ;first identity entry
  mov [edx + 768*4], ecx
  add edx, 4
  mov ecx, 1024
.fill:
  mov ebx, eax
  or ebx, 0x03
  mov [edi], ebx
  add edi, 4
  add eax, 4096
  loop .fill
  cmp eax, edi
  jb .next_table
  mov [page_tables_end - 0xc0000000], edi
  mov eax, boot_page_directory - 0xc0000000
  mov cr3, eax
  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax
  lea eax, [.higher]
  jmp eax

section .text
.higher:
  mov dword [boot_page_directory], 0
  mov eax, cr3
  mov cr3, eax ;clear tlb
  mov esp, stack_top
  call k_entry_main
  cli
.hang:
  hlt
  jmp .hang

