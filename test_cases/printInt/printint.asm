.data
	num: .word 4
.text
	main:
		lw $a0, num
		addi $v0, $0, 1
		syscall
		jr $ra
