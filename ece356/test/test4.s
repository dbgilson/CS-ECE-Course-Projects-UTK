
.section .text
.global asmfloat
asmfloat:
	#float asmfloat(float a, float b, float c)

	fmv.s	fa0, fa1
	ret


	#float asmfloat(float a, int b, float c)
	#float asmfloat(fa0, a0, fa1)

	fcvt.s.w	fa0, a0
	ret

	#float asmfloat(float *a, int *b, float *c)
	#float asmfloat(a0, a1, a2)
	#load a

	flw		fa0, 0(a0)

	ret

/* problem

extern "C" {
		float floatadd(float, a, float *b, double c, double *d);
}

float floatadd(float a, float *b, double c, double *d){
	return a + *b + c + *d;
}

*/

.global floatadd
floatadd:
	#float floatadd(fa0, a0, fa1, a1)

	flw		ft0, 0(a0) #pointer to float 
	fld		ft1, 0(a1) #pointer to double

	fadd.s	fa0, fa0, ft0 #add a + b (float)
	fadd.d	fa1, fa1, ft1 #add c + d (double)

	fcvt.s.d ft2, fa1 #covert (double) c + d to float

	fadd.s	fa0, fa0, ft2 #add c + d to a + b

	ret
##################Dr. Marz Impelementation

	flw		fa2, 0(a0)
	fld		fa3, 0(a1)

	fcvt.s.d	fa1, fa1
	fcvt.s.d	fa3, fa3

	fadd.s	fa1, fa1, fa3
	fadd.s	fa0, fa0, fa2
	fadd.s	fa0, fa0, fa1

	ret











