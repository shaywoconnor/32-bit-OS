bits 32
section .text
global _start
_start:
  mov edi, 0xb8000
  mov ecx, 80 * 25
  mov ax, 0x0a20
  rep stosw

  mov esi, msg
  mov edi, 0xb8000
  mov ah, 0x0a
.loop:
  lodsb
  test al, al
  jz .hang
  stosw
  jmp .loop

.hang:
  cli
  hlt
  jmp .hang

section .data
msg db "Hello from the kernel!", 0
