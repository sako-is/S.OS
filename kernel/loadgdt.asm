global gdt_flush	; Allows the C code to link to this
extern gdt_ptr		; Says that '_gp' is in another file

loadGDT:
    mov [gdt_ptr], rdi
	lgdt rdi	     ; Load the GDT with our '_gp' which is a special pointer
    mov 0x10, ax
    mov ax, ds
    mov ax, es
    mov ax, ss
    mov 0x2b, ax
    ltr ax
	retfq
