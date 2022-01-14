.section .rodata
prompt: .asciz "%d\n"
.section .text
.global getasm
getasm:
//void getasm(int s[], int size, int a){
//   int i; 
//   for( i = 0; i < size; i++){
//        s[i] *= a;
//   }
//}
	//int i =0
	li		t0, 0
1:
	//i < size
	bge		t0, a1
	lw		t1, 
	mul			
	addi    t0, t0, 1
	j		1b
2:
	ret
	
//void getasm2(int s[], int size){
//    int result = 0;
//	  int i;
//    for(int i = 0; i < size; i++){
//       result += s[i];
//    }

//    printf("%d\n", result);

//}
////////////////////////////////////Dr. Marz implementation
.global getasm
getasm:
	# void [a0 s[], a1 size, a2 value]

	li		t0, 0
1:
	bge		t0, a1, 1f
	
	slli	t1, t0, 2
	add		t2, t1, a0 //s + i

	lw		t3, 0(t2) // s[i]
	mul		t3. t3, a2
	sw		t3, 0(t2)

	addi	t0, t0, 1
	j		1b
1:
	ret

.global getasm2
getasm2:
	# void (a0 s[], a1 size)

	addi	sp, sp, -8
	sd		ra, 0(sp)

	li		t0, 0 //result
	li		t1, 0 //i

1:
	bge		t1, a1, 1f

	slli	t2, t1, 2
	add		t2, a0, t2

	lw		t3, 0(t2)
	add		t0, t0, t3 // result = result + s[i]
	
	addi	t1, t1, 1
	j		1b	
1:
	la		a0, output
	mv		a1, t0
	call	printf 

	ld		ra, 0(sp)
	adi		sp, sp, 8
	ret
