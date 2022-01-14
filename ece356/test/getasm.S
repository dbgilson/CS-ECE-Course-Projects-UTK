.section .text
.global getasm
getasm:
	//int getasm(char s[])
	lw		s0, a0

	//printf("Enter: ");
	addi    sp, sp, -8
	sd      ra, 0(sp)
	call    printf
	ld      ra, 0(sp)
	addi    sp, sp, 8
	
	//scanf("%s", s
	//Put s back into a0
	lw		a0, s0

	//int i


	//i = strlen(s)
	addi    sp, sp, -8
	sd      ra, 0(sp)
	call    strlen
	ld      ra, 0(sp)
	addi    sp, sp, 8


	//return i;
	ret


/////////Dr. Mars' Implementation
//Calling Functions:
//1)Set registers -> ABI
//2)
/*
.section .rodata
prompt: .asciz "Enter: "
scan: /asciz "%s"

.section .text
.global getasm
getasm:
	addi	sp, sp, -16
	sd		ra, 0(sp)
	sd		s0, s0, 8(sp)

	mv		s0,	a0

	la		a0, prompt
	call	printf

	la		a0, scan
	mv		a1, s0
	call	scanf

	mv		a0, s0
	cal		strlen

	ld		ra, 0(sp)
	ld		s0, 8(sp)
	addi	sp, sp, 16
	ret
*/
