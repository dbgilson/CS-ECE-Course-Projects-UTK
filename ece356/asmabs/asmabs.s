/*int asmabs(int a){
	if(a < 0)
		return -a;
	else
		return a;
}
*/

.section .text
.global asmabs
asmabs:
	
	#int asmabs(int a);
	#a = a0

	#if(a < 0)
	blt a0, zero, is_neg
	
	#else return a
	ret
	
label:
	#negative a0
	neg a0, a0
	ret
	

