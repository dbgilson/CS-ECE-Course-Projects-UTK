//Here is the assembly code for the Lab6 functions

//We always start with this to specify the type of code we are using
.intel_syntax noprefix

//This allows for the printf function we use in the Print function to be used.  %c is for the key and %d is for the value.
.section .rodata
output:	.asciz "%c = %d\n"

//Here is the main section of the assembly code.  It goes in text since we are writing instructions.  We also include .global .main befor each function label so that the functions 
//can be properly read.
.section .text
.global .main
asm_Construct:
/*void c_Construct(struct mymap *p){
	for(int i = 0; i < 20; i++){
	(*p).mKeyValues[i].key = '*';
	(*p).mKeyValues[i].value = -1;
 }
}*/

//i = 0
mov ecx, 0
movsx rcx, ecx
1:
//For loop

//if i >= 20, end function
cmp rcx, 20
jge 2f

//(*p).mKeyValues[i].key = '*'
mov BYTE PTR[rdi + (8*rcx)], '*'

//(*p).mKeyValues[i].value = -1
mov DWORD PTR[rdi + (8*rcx + 4)], -1

//i++
inc rcx
jmp 1b

2:
ret




.global .main
asm_Set:
/*void c_Set(struct mymap *p, char key, int value){
	    int placeset = -1;
		for(int i = 0; i < 20; i++){
			if((*p).mKeyValues[i].key == key){
			(*p).mKeyValues[i].value = value;
			return;
			}
			if((*p).mKeyValues[i].key == '*'){
			placeset = i;
			}
		 }
	(*p).mKeyValues[placeset].key = key;
	(*p).mKeyValues[placeset].value = value;
}*/

//int placeset = -1, i = 0
mov r13d, -1
mov ecx, 0
movsx r13, r13d
movsx rcx, ecx

1:
//For loop

//if i >=20, end loop
cmp rcx, 20
jge 4f

//if (*p).mKeyValues[i].key == key
cmp BYTE PTR[rdi + 8*rcx], sil
je	2f

//if (*p).mKeyValues[i].key == '*'
cmp BYTE PTR[rdi + 8*rcx], '*'
je	3f

//i++
inc rcx
jmp 1b

2:
//(*p).mKeyValues[i].value = value, then return
mov DWORD PTR[rdi + 8*rcx + 4], edx
ret

3:
//placeset = i
mov r13, rcx

//i++
inc rcx
jmp 1b

4:
//(*p).mKeyValues[placeset].key = key
mov BYTE PTR[rdi + 8*r13], sil

//(*p).mKeyValues[placeset].value = value
mov	DWORD PTR[rdi + 8*r13 + 4], edx

ret



.global .main
asm_Get:
/*int  c_Get(const struct mymap *p, char key){
	    for(int i = 0; i < 20; i++){
			if((*p).mKeyValues[i].key == key){
			return((*p).mKeyValues[i].value);
			}
		}
		return(-1);
	}
}*/

//i = 0
mov ecx, 0
movsx rcx, ecx

1:
//For Loop

//if i >= 20, end loop
cmp rcx, 20
jge 3f

//if ((*p).mKeyValues[i].key == key), return the value
cmp BYTE PTR[rdi+8*rcx], sil
je 2f

//i++
inc rcx
jmp 1b

2:
//set eax to value
mov eax, DWORD PTR[rdi+8*rcx + 4]
ret

3:
//if key wasn't found, return -1
mov eax, -1
ret



.global .main
asm_Erase:
/*void c_Erase(struct mymap *p, char key){
	    for(int i = 0; i < 20; i++){
			if((*p).mKeyValues[i].key == key){
			(*p).mKeyValues[i].key = '*';
		}
	}
}*/
//i = 0
mov ecx, 0
movsx rcx, ecx
1:
//For loop

//if i >= 20, end loop
cmp	rcx, 20
jge 3f

//if ((*p).mKeyValues[i].key == key)
cmp	BYTE PTR[rdi + 8*rcx], sil
je 2f

//i++
inc rcx
jmp 1b

2:
//(*p).mKeyValues[i].key = '*'
mov BYTE PTR[rdi + 8*rcx], '*'

//i++
inc rcx
jmp 1b

3:
ret





.global .main
asm_Print:
/*void c_Print(const struct mymap *p){
	for(int i = 0; i < 20; i++){
		printf("%c = %d", (*p).mKeyValues[i].key, (*p).mKeyValues[i].value);
	}
}*/

//i = 0
mov r13d, 0
movsx r13, r13d

//push the variables
push rdi


1:
//For loop

//if i >=20, end loop
cmp r13, 20
jge	2f

//Call printf
mov	rax, 0
lea	rdi, [rip+output]
mov	sil, BYTE PTR[rdi + 8*r13]
mov edx, DWORD PTR[rdi + 8*r13 + 4]
call printf@plt

//i++
inc r13
jmp 1b

2:
pop rdi
ret
