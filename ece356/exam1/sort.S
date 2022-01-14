#Sort Question exam 1, doing bubble sort on double array
#David Gilson
.section .text
.global sort
sort:

1: 
	li		t0, 0 #swapped = false
	li		t1, 1 #int i = 1

2:
	bge		t1, a1, 2f #if i >= size break

	slli	t2, t1, 3 #scale t2 to i * 8 (for double array position)
	add		t3, a0, t2 #t3 = array + i
	addi	t4, t3, -8 #t4 = array + (i-1)

	ld		t5, 0(t4) #t5 = array[i-1]
	ld		t6, 0(t3) #t6 = array[i]

	ble		t5, t6, 3f #if array[i-1] is <= array[i], skip if statement

	li		t0, 1 #swapped = true
	sd		t5, 0(t3)
	sd		t6, 0(t4) #swap values

3:
	addi	t1, t1, 1 #i++
	j		2b

2:
	bne		t0, zero, 1b #if swapped != false, go back to do loop
	ret
