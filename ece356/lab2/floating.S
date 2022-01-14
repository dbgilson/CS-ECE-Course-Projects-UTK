# floating.S
# The point of this lab is to give us experience in using floating point registers
# and operations.  We have a distance function and a square function, and we need
# to call the other functions when necessary.
# David Gilson
# 10/02/2020
# ECE-356

.section .text
.global _Z8distancedddd
# double distance(double x1, double x2, double y1, double y2);
_Z8distancedddd:
	
	# X1 - X2 and Y1 - Y2
	fsub.d		fa0, fa0, fa1
	fsub.d		fa2, fa2, fa3

	#Convert and call first square function on (X1 - X2) (used _Z6squaref)
	fcvt.s.d   fa0, fa0
	
	addi	sp, sp, -8
	sd		ra, 0(sp)
	call	_Z6squaref
	ld		ra, 0(sp)
	addi	sp, sp, 8

	fcvt.d.s	fa0, fa0
	fmv.d		ft0, fa0

	#Convert and call second square function on (Y1 - Y2) (used _Z6squaref)
	fcvt.s.d	fa2, fa2
	fmv.s		fa0, fa2

	addi	sp, sp, -8
	sd		ra, 0(sp)
	call	_Z6squaref
	ld		ra, 0(sp)
	addi	sp, sp, 8

	fcvt.d.s	fa0, fa0

	#Add the squared results together
	fadd.d		fa0, fa0, ft0

	#Squareroot the sum and return it.
	fsqrt.d		fa0, fa0
	ret

.global _Z8distancePdS_S_S_S_
# void distance(double *result, double *x1, double *x2, double *y1, double *y2);
_Z8distancePdS_S_S_S_:
	
	#Convert pointers to the proper double values to go
	#into the other distance function.
	fld		fa0, 0(a1)
	fld		fa1, 0(a2)
	fld		fa2, 0(a3)
	fld		fa3, 0(a4)

	#Call the other distance function
	addi	sp, sp, -8
	sd		ra, 0(sp)
	call    _Z8distancedddd 
	ld		ra, 0(sp)
	addi	sp, sp, 8

	#Store function result at address pointed to by a0, then return
	fsd		fa0, 0(a0)
	ret


.global _Z6squarePfS_
# void square(float *r, float *x);
_Z6squarePfS_:

	#Convert pointers to the proper float values to go
	#into the other square function.
	flw		fa0, 0(a1)

	#Call the other square function
	addi	sp, sp, -8
	sd		ra, 0(sp)
	call	_Z6squaref
	ld		ra, 0(sp)
	addi	sp, sp, 8

	#Store function result at address pointed to by a0, then return
	fsw		fa0, 0(a0)
	ret

.global _Z6squaref
# float square(float x);
_Z6squaref:

	#Simply multiply the given value by itself to 
	#square the value, then return.
	fmul.s		fa0, fa0, fa0
	ret
