.data
	name:	.asciiz "awesome"
.text
	main:
		la $a0, name
		addi $v0, $0, 4
		syscall
		jr $ra