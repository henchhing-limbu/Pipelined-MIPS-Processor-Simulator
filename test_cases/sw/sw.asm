.data
.text
main:
	addi $t0, $0, 4
	addi $sp, $sp, -4
	sw $t0, 0($sp)
	lw $a0, 0($sp)
	li $v0, 1
	syscall
	addi $sp, $sp, 4
	jr $ra

