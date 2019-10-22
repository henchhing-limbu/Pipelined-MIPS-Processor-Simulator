.data
	success:	.asciiz "stalling worked!!!"
	failure:	.asciiz "stalling didn't work..."
.text
main:
	addi $t0, $0, 5
	addi $t1, $0, 5
	addi $sp, $sp, -4
	sw $t0, 0($sp)
	lw $t5, 0($sp)
	beq $t5, $t1, win
fail:
	la $a0, failure
	li $v0, 4
	syscall
	li $v0, 10
	syscall
win:
	la $a0, success
	li $v0, 4
	syscall
	li $v0, 10
	syscall