.data
	success:	.asciiz "branch forwarding worked!!!"
	failure:	.asciiz "branch forwarding didn't work..."
.text
main:
	addi $t0, $0, 10
	addi $t1, $0, 10
	addi $t2, $0, 5
	beq $t0, $t1, win
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
