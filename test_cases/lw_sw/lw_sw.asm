.data
.text
main:
	addi $t0, $0, 5
	addi $sp, $sp, -8
	sw $t0, 0($sp)
	lw $t5, 0($sp)
	sw $t5, 4($sp)
	addi $sp, $sp, 8
	li $v0, 10
	syscall
