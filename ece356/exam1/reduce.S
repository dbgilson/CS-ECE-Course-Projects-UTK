#Reduce Question Exam 1
#David Gilson
.section .text
.global reduce
reduce:
	#float reduce(const double array[], int size, float modifier);
	#float reduce(a0, a1, fa0) returning in fa0

	li		t0, 0 #i = 0
	li		t4, 0
	fcvt.d.w ft0, t4 #sum = 0

1:
	bge		t0, a1, 1f

	slli	t1, t0, 3 #scale t1 to i * 8 (for double array position)
	add		t2, a0, t1 #t2 = array + i

	fld		ft1, 0(t2) #ft1 = array[i]
	fadd.d	ft0, ft0, ft1 #Add current array element to sum

	addi	t0, t0, 1 #i++
	j		1b

1:
	fcvt.s.d	ft0, ft0 #convert double to float
	fmul.s		fa0, ft0, fa0 #multiply converted sum by float modifier, store it in fa0
	ret
