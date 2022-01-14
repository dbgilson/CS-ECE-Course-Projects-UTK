.section .text
.global add
add:
	#void add (int &result, int left, int right)
	#void add(a0, a1, a2)

	add		t0, a1, a2 # left + right
	sw		t0, 0(a0)
	ret
