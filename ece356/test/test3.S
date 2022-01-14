.section .text
.global doasm
doasm:
	
	lw		t0, zero

1:
	bge		t0, a1, 2f
	lw		


	addi	t0, t0, 1
	j		1b
2:
	ret

///////////////Dr. Mar's Implementation with c just as int
.sectoin .text
.global doasm
doasm:

	li		t0, 0 #i
1:
	bge		t0, a1, 1f
	
	slli	t1, t0, 3
	add		t1, t1, a0

	lb		t2, 0(t1) # s[i].a
	lb		t3, 1(t1) # s[i].b

	add		t3, t2, t3
	sw		t3, 4(t1) # s[i].c = s[i].a + s[i].b


	addi	t0, t0, 1
	j		1b
1:	
	ret

///////////////Dr. Mar's Implementation with c as pointer to int (big help on lab)
//Note *(s.c) = s->c
.sectoin .text
.global doasm
doasm:

    li      t0, 0 #i
1:
    bge     t0, a1, 1f

    slli    t1, t0, 4 # Size is now 16 instead of 8
    add     t1, t1, a0

    lb      t2, 0(t1) # s[i].a
    lb      t3, 1(t1) # s[i].b

    add     t3, t2, t3
	ld		t4, 8(t1) # s[i].c
    sw      t3, 0(t4) # s[i].c = s[i].a + s[i].b


    addi    t0, t0, 1
    j       1b
1:
    ret

