[bits 64]

global loadTSS
loadTSS:
	mov ax, 0x28
	ltr ax
	retfq