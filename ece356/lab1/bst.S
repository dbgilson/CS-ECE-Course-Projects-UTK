## Lab 1
## The purpose of this lab is to write the find_key() function for traversing
## a binary search tree in RISC-V assembly.
## David Gilson
## 9/18/20
## ECE-356 Spring 2020

## Instead of having to type the long, mangled name, use find_key
## as an alias.

/*
Node *node = sentinel->right;
    while (node != sentinel) {
        if (key < node->key)
            node = node->left;
        else if (key > node->key)
            node = node->right;
        else
            return node;
    }
return nullptr;
*/

.section .text
.global _ZNK6BSTree8find_keyERKi
_ZNK6BSTree8find_keyERKi:

	## Node * find_key(const int &key) const

	lw		t1,	0(a1) #const int key value

	#Node *node = sentinal->right
	ld 		t2, 8(a0) #t2 = sentinel pointer
	ld 		t3, 8(t2) #t3 = sentinel -> right pointer (Starting Node)

1:
	beq 	t3, t2, 4f #If Node == Sentinel, break
	
	lw		t4, 24(t3) #Put key of current node into t4

	blt		t1, t4, 2f #If Key < node->key, branch
	bgt		t1, t4, 3f #If Key > node->key, branch	

	#If past here, we found node
	
	mv		a0, t3 #a0 = node
	ret
2:
	#If key < node->key, then node = node->left
	ld		t3, 0(t3)
	j		1b
3:
	#If key > node->key, then node = node->right
	ld		t3, 8(t3)
	j		1b
4:
	mv 		a0, zero #If Node == Sentiel, return NULLPTR
	ret
