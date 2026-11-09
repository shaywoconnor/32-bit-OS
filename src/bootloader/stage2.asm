org 0x7e00
bits 16
cpu 386

%macro SET_PE 0
  mov eax, cr0
  or al, 1
  mov cr0, eax
%endmacro
%macro CLEAR_PE 0
  mov eax, cr0
  and al, 0xfe
  mov cr0, eax
%endmacro

MMAP_BASE_SEG equ 0x0100
MMAP_MAX      equ 128
MMAP_ENTRY    equ 24

entry:
  db 'S2' ;test bytes, stage 1 jumps to 0x7e02
  jmp 0x0000:main

align 8
gdt_start:
gdt_null:
  dq 0

gdt_code:
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x9B
  db 0xCF
  db 0x00

gdt_data:
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x93
  db 0xCF
  db 0x00

gdt_code16:
  dw 0xFFFF
  dw 0x0000
  db 0x00
  db 0x9B
  db 0x00
  db 0x00
gdt_end:

gdt_desc:
  dw gdt_end - gdt_start - 1
  dd gdt_start

CODE32_SEL equ gdt_code   - gdt_start
DATA_SEL   equ gdt_data   - gdt_start
CODE16_SEL equ gdt_code16 - gdt_start

main:
  cld
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7c00

  mov [boot_drive], dl
  mov si, stage2_loaded_msg
  call print

  call enable_a20
  test al, al
  jz .a20_error

  call detect_memory
  test al, al
  jz .memory_error

  lgdt [gdt_desc]
  call enable_unreal

  call load_kernel
  test al, al
  jz .kernel_error

  cli
  SET_PE
  jmp CODE32_SEL:pm_entry

.a20_error:
  mov si, a20_error_msg
  jmp error
.memory_error:
  mov si, memory_error_msg
  jmp error
.kernel_error:
  mov si, kernel_error_msg
  jmp error


FAT32_BUF equ 0xB000
FAT32_BUF_SECTOR dd 0xffffffff
CLUSTER_BUF equ 0xC000
CLUSTER_SIZE equ 0x1000
CLUSTER_BUF_END dw 0x0000

; stored relative to start of partition
fat_lba dd 0x00000000
data_lba dd 0x00000000

kernel_name db "KERNEL  ELF"

DAP:
  db 0x10
  db 0x00
DAP_count dw 0x0000
DAP_dest_offset dw 0x0000
DAP_dest_segment dw 0x0000
DAP_lba dq 0x0000000000000000

; takes cluster no in ecx
read_cluster:
  push eax
  push si
  movzx ax, byte [0x7c0d]
  mov word [DAP_count], ax
  mov word [DAP_dest_offset], CLUSTER_BUF
  mov eax, ecx
  sub eax, 2
  push edx
  movzx edx, byte [0x7c0d]
  mul edx
  add eax, [data_lba]

  xor edx, edx
  add eax, [0x7c1c]
  adc edx, 0
  mov [DAP_lba], eax
  mov [DAP_lba + 4], edx

  mov dl, [boot_drive]
  mov ah, 0x42
  mov si, DAP
  int 0x13
  pop edx
  pop si
  pop eax
  ret

; returns next cluster no in ecx
next_cluster:
  push eax
  push ebx
  mov eax, ecx
  shl eax, 2
  mov ebx, eax
  and ebx, 0x01ff
  shr eax, 9
  add eax, [fat_lba]
  cmp eax, dword [FAT32_BUF_SECTOR]
  jz .ret
.load:
  push si
  mov word [DAP_count], 0x0001
  mov word [DAP_dest_offset], FAT32_BUF

  push eax
  push edx
  xor edx, edx
  add eax, [0x7c1c]
  adc edx, 0
  mov [DAP_lba], eax
  mov [DAP_lba + 4], edx

  mov dl, [boot_drive]
  mov ah, 0x42
  mov si, DAP
  int 0x13
  pop edx
  pop eax
  pop si
  jc .error
  mov dword [FAT32_BUF_SECTOR], eax

.ret:
  mov ecx, dword [FAT32_BUF + bx]
  and ecx, 0x0fffffff
  pop ebx
  pop eax
  ret
.error:
  pop ebx
  pop eax
  stc
  ret

; takes ecx = directory cluster
;       si = file/dir name
; returns first kernel cluster in ecx
; al = 0 if not found
find_cluster:
.search_dir:
  call read_cluster
  jc .error
  mov di, CLUSTER_BUF
.search_cluster:
  cmp byte [di], 0x00
  je .not_found
  cmp byte [di], 0xe5
  je .next
  test byte [di+0x0b], 0x08
  jnz .next
  mov al, byte [di + 0x0b]
  and al, 0x3f
  cmp al, 0x0f
  je .next
  push si
  push di
  push cx
  mov cx, 11
  repe cmpsb
  pop cx
  pop di
  pop si
  je .found
.next:
  add di, 0x20
  cmp di, word [CLUSTER_BUF_END]
  jae .next_cluster
  jmp .search_cluster

.next_cluster:
  call next_cluster
  jc .error
  cmp ecx, 0x0ffffff8
  jae .not_found
  cmp ecx, 2
  jb .error
  cmp ecx, 0x0ffffff7
  je .error
  jmp .search_dir
.not_found:
.error:
  mov al, 0x00
  ret
.found:
  mov cx, [di+0x14]
  shl ecx, 16
  mov cx, [di+0x1a]
  and ecx, 0x0fffffff
  mov al, 0x01
  ret

ELF_BUF equ 0xD000
ELF_BUF_SIZE equ 0x1000
kernel_entry dd 0x00000000

load_kernel:
  movzx eax, word [0x7c0e]
  mov dword [fat_lba], eax
  movzx eax, byte [0x7c10]
  mul dword [0x7c24]
  add eax, dword [fat_lba]
  mov dword [data_lba], eax
  cmp word [0x7c0b], 0x0200
  jne .fail
  movzx ax, byte [0x7c0d]
  imul ax, word [0x7c0b]
  cmp ax, CLUSTER_SIZE
  jne .fail
  add ax, CLUSTER_BUF
  mov word [CLUSTER_BUF_END], ax
  mov ecx, dword [0x7c2c]
  mov si, kernel_name
  call find_cluster
  test al, al
  jz .fail
.load:
  xor eax, eax
  mov ebx, 0x34
  mov edi, ELF_BUF
  call file_read
  test al, al
  jz .fail
  cmp dword [ELF_BUF], 0x464c457f
  jne .fail
  cmp word [ELF_BUF + 0x04], 0x0101
  jne .fail
  cmp word [ELF_BUF + 0x12], 0x0003
  jne .fail
  cmp word [ELF_BUF + 0x2a], 0x0020
  jne .fail
  mov eax, dword [ELF_BUF + 0x18]
  mov dword [kernel_entry], eax
  mov eax, dword [ELF_BUF + 0x1c]
  movzx ebx, word [ELF_BUF + 0x2c]
  shl ebx, 5
  cmp ebx, ELF_BUF_SIZE
  ja .fail
  mov dx, word [ELF_BUF + 0x2c]
  push dx
  call file_read
  pop dx
  test al, al
  jz .fail
  mov esi, ELF_BUF
.read_program_headers:
  test dx, dx
  jz .done
  cmp dword [esi], 0x00000001
  jne .next
  mov eax, dword [esi + 0x04]
  mov ebx, dword [esi + 0x10]
  mov edi, dword [esi + 0x0c]
  cmp edi, 0x00100000
  jb .fail
  push dx
  call file_read
  pop dx
  test al, al
  jz .fail
  mov eax, dword [esi + 0x14]
  cmp eax, ebx
  jb .fail
  je .next
  push ecx
  mov ecx, eax
  sub ecx, ebx
  xor al, al
  add edi, ebx
  cld
  a32 rep stosb
  pop ecx
.next:
  add esi, 0x20
  dec dx
  jmp .read_program_headers
.done:
  mov al, 0x01
  ret
.fail:
  mov al, 0x00
  ret

; eax = offset
; ebx = length
; ecx = file starting cluster
; edi = destination buffer
;
; returns 0x00 in al if error else 0x01
file_read:
  push ebx
  push ecx
  push esi
  push edi
.loop:
  cmp eax, CLUSTER_SIZE
  jae .push_start
  call read_cluster
  jc .fail
  push ecx
  mov ecx, CLUSTER_SIZE
  sub ecx, eax
  mov esi, eax
  add esi, CLUSTER_BUF
  cmp ecx, ebx
  jb .less
  mov ecx, ebx
.less:
  push ecx
  a32 rep movsb
  pop ecx
  jb .read_more
  pop ecx
  mov al, 0x01
  jmp .ret
.read_more:
  add eax, ecx
  sub ebx, ecx
  pop ecx
  jmp .loop
.push_start:
  sub eax, CLUSTER_SIZE
  call next_cluster
  jc .fail
  cmp ecx, 2
  jb .fail
  cmp ecx, 0x0ffffff7
  jb .loop
.fail:
  mov al, 0x00
.ret:
  pop edi
  pop esi
  pop ecx
  pop ebx
  ret

enable_unreal:
  pushfd
  push eax
  cli
  SET_PE
  jmp CODE16_SEL:.pm16
.pm16:
  mov ax, DATA_SEL
  mov fs, ax
  mov es, ax
  CLEAR_PE
  jmp 0x0000:.rm
.rm:
  xor ax, ax
  mov fs, ax
  mov es, ax
  pop eax
  popfd
  ret

enable_a20:
  call test_a20
  test al, al
  jnz .done

  mov ax, 0x2401
  int 0x15
  call wait_a20
  test al, al
  jnz .done

  call enable_a20_kbc
  call wait_a20
  test al, al
  jnz .done

  in al, 0x92
  or al, 0x02
  and al, 0xfe
  out 0x92, al
  call wait_a20
.done:
  ret

enable_a20_kbc:
  pushf
  cli
  call kbc_wait_input
  jc .done
  mov al, 0xad
  out 0x64, al

  call kbc_wait_input
  jc .reenable
  mov al, 0xd0
  out 0x64, al
  call kbc_wait_output
  jc .reenable
  in al, 0x60
  mov ah, al

  call kbc_wait_input
  jc .reenable
  mov al, 0xd1
  out 0x64, al
  call kbc_wait_input
  jc .reenable
  mov al, ah
  or al, 0x02
  out 0x60, al

.reenable:
  call kbc_wait_input
  jc .done
  mov al, 0xae
  out 0x64, al
  call kbc_wait_input
.done:
  popf
  ret

kbc_wait_input:
  push cx
  mov cx, 0xffff
.loop:
  in al, 0x64
  test al, 0x02
  jz .ok
  loop .loop
  pop cx
  stc
  ret
.ok:
  pop cx
  clc
  ret

kbc_wait_output:
  push cx
  mov cx, 0xffff
.loop:
  in al, 0x64
  test al, 0x01
  jnz .ok
  loop .loop
  pop cx
  stc
  ret
.ok:
  pop cx
  clc
  ret

wait_a20:
  push cx
  mov cx, 0x1000
.loop:
  call test_a20
  test al, al
  jnz .done
  loop .loop
.done:
  pop cx
  ret

test_a20:
  push si
  push di
  push ds
  push es
  xor ax, ax
  mov es, ax
  not ax
  mov ds, ax

  mov di, 0x0500
  mov si, 0x0510

  mov al, byte [es:di]
  push ax
  mov al, byte [ds:si]
  push ax

  mov byte [es:di], 0x00
  mov byte [ds:si], 0xff
  cmp byte [es:di], 0xff

  pop ax
  mov byte [ds:si], al

  pop ax
  mov byte [es:di], al

  mov al, 0x00
  je .done
  mov al, 0x01

.done:
  pop es
  pop ds
  pop di
  pop si
  ret

detect_memory:
  push es
  push di
  push ebx
  push edx
  push ecx

  mov ax, MMAP_BASE_SEG
  mov es, ax
  mov dword [es:0x0000], 0
  mov di, 0x0004

  xor ebx, ebx
.loop:
  mov eax, 0x0000e820
  mov edx, 0x534d4150           ; "SMAP"
  mov ecx, MMAP_ENTRY
  mov dword [es:di+20], 1
  int 0x15
  jc .end
  cmp eax, 0x534d4150
  jne .end

  cmp cl, 24
  jb .check_len
  test byte [es:di+20], 0x01
  jz .skip
.check_len:
  mov eax, [es:di+8]
  or eax, [es:di+12]
  jz .skip

  add di, MMAP_ENTRY
  inc dword [es:0x0000]
  cmp dword [es:0x0000], MMAP_MAX
  jae .end
.skip:
  test ebx, ebx
  jnz .loop

.end:
  mov al, 0x01
  cmp dword [es:0x0000], 0
  jne .ret
  mov al, 0x00
.ret:
  pop ecx
  pop edx
  pop ebx
  pop di
  pop es
  ret

print:
  lodsb
  test al, al
  jz .done
  mov ah, 0x0e
  xor bx, bx
  int 0x10
  jmp print
.done:
  ret

error:
  call print
halt:
  cli
  hlt
  jmp halt

strings:
  stage2_loaded_msg db "Loaded Stage2", 0x0d, 0x0a, 0x00
  a20_error_msg     db "failed to enable A20 line", 0x0d, 0x0a, 0x00
  memory_error_msg  db "failed to detect memory", 0x0d, 0x0a, 0x00
  kernel_error_msg  db "failed to load kernel", 0x0d, 0x0a, 0x00

boot_drive db 0x00

bits 32

pm_entry:
  mov eax, DATA_SEL
  mov ds, eax
  mov es, eax
  mov fs, eax
  mov gs, eax
  mov ss, eax
  mov esp, 0x00090000
  mov ebx, 0x1000
  cld
  movzx edx, byte [boot_drive]
  jmp [kernel_entry]
