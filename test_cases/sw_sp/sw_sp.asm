.data
.text
main:
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	
	li $t0, 32700
	
	addi $sp, $sp, -4
	sw $t0, 0($sp)
	
	lw $t1, 0($sp)
	addi $sp, $sp, 4
	
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	
	jr $ra
