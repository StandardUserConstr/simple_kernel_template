bits 16
org 0x7C00                                      ;-bios pointing this address to start executing for processor
                                                ;so i inform compilator to add this offset to data in this program
KERNEL_OFFSET equ 0x1000                        ;-i want to load kernel in this address (avoiding table of interrupts and
                                                ;memory used by bios loaded at address 0x0 to 0x900 in ram
                                                ;leaving 0x100bytes for reserve)

mov [BOOT_DRIVE],Dl                             ;-bios storing boot drive for his drive interruption INT13 in DL register

mov Bp,0x7F00                                   ;
mov Sp,Bp                                       ;-set-up the stack (base-stack pointer & actual-stack pointer)
                                                ;max size of actual stack can be 256

mov Ah,0x0                                      ;
mov Al,0x3                                      ;
int 0x10                                        ;-set display mode

mov Si,MSG_REAL_MODE             ;
call bios_print16_string                        ;-print message
mov Si,MSG_LOAD_KERNEL             
call bios_print16_string

mov Ah,0x02                                     ;-read disk sectors
mov Al,6                                        ;max to read: 54 sectors (27648bytes || 0x6C00bytes)+0x1000 = 0x7C00
mov Ch,0                                        ;select cylinder 0
mov Dh,0                                        ;select head 0
mov Cl,2                                        ;start reading from second sector (first sector 512bytes is actual bootsector)
mov Bx,KERNEL_OFFSET                            ;load sectors to this address
mov Dl,[BOOT_DRIVE]                             ;disk identification number to read from
int 0x13

jc failed_disk                                  ;carry flag will be set if there's some errors
cmp Al,6                                       ;int 0x13 returns to Al number of sectors that was readed
jne failed_disk

cli                                             ;-off bios interrupts
lgdt [gdt_descriptor]                           ;-load global descriptor table for protected mode
mov eax,cr0                                     ;-make a switch to protected mode
or eax,1
mov cr0,eax

jmp CODE_SEG:init_pm                            ;-far jump forces to flush pre-fetched 16bit instruction from CPU cashe.
                                                ;segment:offset instruction works diffrent in protected mode and in this
                                                ;scenario register CS (code segment) is updating of CODE_SEG value
                                                ;and that's it. what i know there isn't anything more of it.
bits 32
init_pm:
	mov ax,DATA_SEG                             ;update segment registers to data selectors in GDT
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	;mov ebp,0x90000                            ;more space of stack memory can be "reserved"
	;mov esp,ebp

call KERNEL_OFFSET
hlt
jmp $


bits 16
                                                ;-global variables
BOOT_DRIVE db 0
MSG_REAL_MODE db " started in 16-bit Real Mode",0
MSG_LOAD_KERNEL db " loading kernel into memory",0
;MSG_PROT_MODE db " successfully initialized 32-bit Protected Mode",0
;MSG_LOAD_KERNEL_SUCCESS db " successfully loaded kernel",0

MSG_FAILED_DISK db " READING DISK FAILED",0


bios_print16_string:                            ;-Si = address to string (source pointer)
	pusha
	xor Bx,Bx
	mov Ah,0xE
	bios_print16_string_loop0:
		mov Al,BYTE [Si]
		int 0x10
		add Si,1
		cmp BYTE [Si],0
		jnz bios_print16_string_loop0
		
	mov Ah,0x3
	int 0x10
	sub Ah,1
	add Dh,1
	xor Dl,Dl
	int 0x10
	popa
	ret

bios_screen16_clear:
	pusha
	
	mov Ah,0x2
	xor Bx,Bx
	xor Dx,Dx
	int 0x10

	mov Ah,0xE
	mov Al,' '
	xor Cx,Cx
	bios_screen16_clear_loop0:
		int 0x10
		add Cx,1
		cmp Cx,2000
		jnz bios_screen16_clear_loop0	

	mov Ah,0x2
	int 0x10
	popa
	ret

failed_disk:
	mov Si,MSG_FAILED_DISK
	call bios_print16_string
	cli
	hlt
	jmp $

;                                                         not my table. table maded by cs.bham.ac.uk                                                   ;                     
gdt_start:
	gdt_null:                        
		dd 0x0        
		dd 0x0
	gdt_code:
                                                         ;-base =0x0 | limit =0 xfffff ,
                                                         ;1st flags: (present)1 (privilege)00 (descriptor type)1 -> 1001B
                                                         ;type flags: (code)1 (conforming)0 (readable)1 (accessed)0 -> 1010B
                                                         ;2nd flags: (granularity)1 (32bit default)1 (64bit seg)0 (AVL)0 -> 1100B
		dw 0xffff            ;limit (bits 0-15)
		dw 0x0              ;base (bits 0-15)
		db 0x0              ;base (bits 16-23)
		db 10011010B  ;1st flags -> type flags
		db 11001111B   ;2nd flags -> limit (bits 16-19)
		db 0x0              ;base (bits 24-31)
	gdt_data:
                                                        ;same as code segment except for the type flags :
                                                        ;type flags: (code)0 (expand down)0 (writable)1 (accessed)0 -> 0010B
		dw 0xffff           ;limit (bits 0-15)
		dw 0x0             ;base (bits 0-15)
		db 0x0             ;base (bits 16-23)
		db 10010010B ;1st flags -> type flags
		db 11001111B  ;2nd flags -> limit (bits 16-19)
		db 0x0             ;base (bits 24-31)
gdt_end: 



gdt_descriptor:                                ;GDT descriptior
	dw gdt_end-gdt_start-1   ;size of GDT always less one of the true size
	dd gdt_start 

CODE_SEG equ gdt_code-gdt_start
DATA_SEG equ gdt_data-gdt_start

;                                                         not my table. table maded by cs.bham.ac.uk                                                   ;


times 510-($-$$) db 0
db 0x55
db 0xAA
