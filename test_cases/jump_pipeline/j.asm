.data
	worked: .asciiz "jump worked"
	fail: .asciiz "jump didn't work"
.text
	main:
		j loop
		addi $v0, $0, 4
		la $a0, fail
		syscall
		jr $ra
	loop:
		addi $v0, $0, 4
		la $a0, worked
		syscall
		jr $ra
