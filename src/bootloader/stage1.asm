;
;   Stage 1 bootloader
;       Lives at 0x00 and loaded into 0x7c00
;       defines bpb header and loads stage 2 from lba 2 using extended read
;
;   error codes
;       1 - int 13h ah=41h failed, no extensions
;       2 - bad extensions signature
;       3 - extended read not supported
;       4 - read failed 3 times, followed by bios status in hex
;       5 - stage 2 signature missing
;

org 0x7c00
bits 16
cpu 386

%define STAGE2_LBA 8
%ifndef STAGE2_SECTORS
  %define STAGE2_SECTORS 22
%endif
%define STAGE2_MAGIC 'S2'

%if STAGE2_LBA + STAGE2_SECTORS > 32
  %error "stage 2 overflows reserved sectors"
%endif

  ;BPB
  jmp short start
  nop
                                        ;for addressing later
  bpb_oem db 'mkdosfs '                 ;0x7c03
  bpb_bytes_per_sector dw 0x0200        ;0x7c0b
  bpb_sectors_per_cluster db 0x08       ;0x7c0d
  bpb_reserved_sectors dw 0x0020        ;0x7c0e
  bpb_fat_count db 0x02                 ;0x7c10
  bpb_directory_entries dw 0x0000       ;0x7c11
  bpb_logical_sectors dw 0x0000         ;0x7c13
  bpb_media_type  db 0xf8               ;0x7c15
  bpb_sectors_per_fat dw 0x0000         ;0x7c16
  bpb_sectors_per_track dw 0x003f       ;0x7c18
  bpb_number_of_heads dw 0x0040         ;0x7c1a
  bpb_hidden_sector_count dd 0x00000000 ;0x7c1c
  bpb_large_sector_count dd 0x001ffff8  ;0x7c20

  ;EBR
  ebr_sectors_per_fat dd 0x00000800     ;0x7c24
  ebr_flags dw 0x0000                   ;0x7c28
  ebr_fat_version_no dw 0x0000          ;0x7c2a
  ebr_root_cluster_no dd 0x00000002     ;0x7c2c
  ebr_fsinfo_sector_no dw 0x0001        ;0x7c30
  ebr_backup_sector_no dw 0x0006        ;0x7c32

    ;reserved
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

  ebr_drive_no db 0x80                  ;0x7c40

    ;reserved (nt flags)
    db 0x00

  ebr_signature db 0x29                 ;0x7c42
  
    ;reserved (volume id)
    dd 0x00000000

  ebr_volume_label db 'OS         '
  ebr_identifier db 'FAT32   '          ;0x7c52

start:
  jmp 0x0000:main

main:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7c00
  sti
  cld

  mov [ebr_drive_no], dl

  ;check if extended read is supported
  mov dl, byte [ebr_drive_no]
  mov ax, 0x4100
  mov bx, 0x55aa
  int 0x13
  mov si, 0x01
  jc .error
  mov si, 0x02
  cmp bx, 0xaa55
  jne .error
  mov si, 0x03
  test cx, 1
  jz .error

  mov eax, [bpb_hidden_sector_count]
  add eax, STAGE2_LBA
  mov [DAP+8], eax

  mov di, 3
.read:
  mov word [DAP+2], STAGE2_SECTORS
  mov dl, byte [ebr_drive_no]
  mov si, DAP
  mov ax, 0x4200
  int 0x13
  jnc .loaded
  mov bp, ax
  xor ax, ax
  mov dl, byte [ebr_drive_no]
  int 0x13
  dec di
  jnz .read
  mov si, 0x04
  jmp .error

.loaded:
  mov si, 0x05
  cmp word [0x7e00], STAGE2_MAGIC
  jne .error

  mov dl, byte [ebr_drive_no]
  jmp 0x0000:0x7e02

.error:
  mov ax, si
  add al, '0'
  mov ah, 0x0e
  xor bx, bx
  int 0x10
  cmp si, 0x04
  jne .halt
  mov ax, bp
  mov al, ah
  call print_hex8

.halt:
  cli
  hlt
  jmp .halt

print_hex8:
  push ax
  shr al, 4
  call .nib
  pop ax
.nib:
  and al, 0x0f
  add al, '0'
  cmp al, '9'
  jbe .out
  add al, 7
.out:
  mov ah, 0x0e
  xor bx, bx
  int 0x10
  ret

DAP:
  db 0x10
  db 0x00
  dw STAGE2_SECTORS ;number of sectors to read
  dw 0x0000 ;offset to read into
  dw 0x07e0 ;segment to read into
  dd STAGE2_LBA
  dd 0x00000000

times 510 - ($-$$) db 0x00

;magic boot sector number
  dw 0xaa55
