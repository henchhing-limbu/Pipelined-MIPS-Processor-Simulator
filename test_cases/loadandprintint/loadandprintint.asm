.data
	myint:	.word 4
.text
main:
	lw $a0, myint
	li $v0, 1
	syscall
	jr $ra