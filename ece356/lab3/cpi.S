# cpi.S
# The purpose of this code is to create 3 functions that relate to a CycleTable
# struct.  Find_instruction looks to see if a given instruction is in a CycleTable,
# get_cycles returns how many cycles a given list of instructions take, and calc_cpi
# returns the CPI given the number of cycles and instructions.
# 10/22/2020
# David Gilson

.section .text
.global _Z16find_instructionPK10CycleTablePKc
# int find_instruction(const CycleTable *ctab, const char *inst);
_Z16find_instructionPK10CycleTablePKc:

	ld		t0, 0(a0) #Pointer to CycleTable first value (char *)
	lb		t1, 0(t0) #t1 is now the first letter of the char *r

	lb		t2, 0(a1) #t2 is the first letter of inst

	li		t5, 0 #t5 will be index of cycle table
	li		t6, 0 #t6 will be a char index incrementer
1:	
	beq		t2, zero, 1f #If we found null terminator in t2, then we found a match
	bne		t1, t2, 2f #If t1 != t2, letters aren't same, so move to next table entry

	#Letters are same, so increment both char indexes for next check
	addi	t6, t6, 1
	add		t4, t0, t6
	lb		t1, 0(t4)
	add		t4, a1, t6
	lb		t2, 0(t4)
	j		1b

2:
	#Reset instr and move to next table entry
	addi	a0, a0, 16 #Size of CycleTable entry is 16 bytes
	ld		t0, 0(a0)
	lb		t1, 0(t0)
	

	beq		t1, zero, 3f #If we found null terminator in t1, then we couldn't find match

	addi	t5, t5, 1 #Increment table index

	li		t6, 0
	lb		t2, 0(a1) #Reset t2 to first letter of inst
	j		1b

3: 
	#No match found, returning with -1
	li		a0, -1
	ret
1:	
	#We found a match, so return index
	mv		a0, t5
    ret


.global _Z10get_cyclesPK10CycleTablePK9InstTable
# int get_cycles(const CycleTable *ctab, const InstTable *itab);
_Z10get_cyclesPK10CycleTablePK9InstTable:
	
	ld		t0, 0(a1) #t0 is pointer to first InstTable element
	li		t2, 0
1:
	beq     t0, zero, 1f #If t0 is NULL, then we are done

	#Store ra, a0, a1, and the current number of cycles in stack
	addi	sp, sp, -32
	sd		ra, 0(sp)
	sd		a0, 8(sp)
	sd		a1, 16(sp)
	sd		t2, 24(sp)
	mv		a1, t0     #Put instruction in a1, a0 is the same

	call	_Z16find_instructionPK10CycleTablePKc 

	mv		t3, a0     #Move index into t3 before a0 is changed back
	li		t4, -1
	beq		t3, t4, 2f #If the function returns -1, then bad instruction, so return with error

	ld		ra, 0(sp)
	ld		a0, 8(sp)
	ld		a1, 16(sp)
	ld		t2, 24(sp)
	addi	sp, sp, 32

	slli	t3, t3, 4 #Indexing for CycleTable
	add		t4, a0, t3
	lw		t5, 8(t4) #t5 is now the number of cycles for the found instruction
	add		t2, t2, t5 #Add to existing number of cycles
	addi	a1, a1, 8 #Increment Instatable
	ld		t0, 0(a1) #t0 is start of next InstTable element
	
	j		1b
1:
	#Return number of cycles
	mv		a0, t2
    ret

2:
	#Find_Instruction returned error, so exit get_cycles with error (-1)
	li		a0, -1
	ret

.global _Z8calc_cpiii
# double calc_cpi(int cycles, int instructions)
_Z8calc_cpiii:
	
	#Convert ints into doubles
	fcvt.d.w	ft0, a0
	fcvt.d.w	ft1, a1

	#CPI = Cycles / Instructions, so return ft0/ft1
	fdiv.d		fa0, ft0, ft1
    ret

