.data
.text
main:
	addi $t0, $0, 55
	addi $sp, $sp, -4
	sw $t0, 0($sp)
	lw $t5, 0($sp)
	addi $t6, $t5, 5
	addi $sp, $sp, 4
	li $v0, 10
	syscall