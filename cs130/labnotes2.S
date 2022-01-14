.intel_syntax noprefix

.section .text
.global main
main:
	mov eax, 13
	shr eax, 1
	add eax, 5

	call printf@plt	
