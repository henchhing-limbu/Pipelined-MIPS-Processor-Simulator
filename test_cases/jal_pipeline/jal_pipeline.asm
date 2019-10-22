.data
	worked: .asciiz "jal worked for god's sake\n"
	jumped: .asciiz "we jumped to loop2\n"
.text
	main:
		addi $sp, $sp, -8
		sw $ra, 0($sp)
		jal loop1
		lw $ra, 0($sp)
		addi $v0, $0, 1
		addi $a0, $0, 11
		syscall
		jr $ra
		loop1:
			sw $ra, 4($sp)
			jal loop2
			addi $v0, $0, 4 
			la $a0, worked
			syscall
			lw $ra, 4($sp)
			addi $v0, $0, 1
			addi $a0, $0, 11
			syscall
			jr $ra
	loop2:
		addi $v0, $0, 4
		la $a0, jumped
		syscall
		jr $ra