.data
.text
	main:
		addi $t0, $0, 5
		addi $t1, $0, 8
		loop1:
			addi $t0, $t0, 1
			beq $t0, $t1, loop2
			j loop1
		loop2:
			addi $v0, $0, 1
			addi $a0, $0, 32111
			syscall
			jr $ra