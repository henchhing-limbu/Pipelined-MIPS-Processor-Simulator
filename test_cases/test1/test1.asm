.data
.text
main:
	addi $t0, $0, 0
	addi $t1, $0, 25
	addi $t2, $0, 5

	loop:
		add $t0, $t0, $t2
		bne $t0, $t1, loop

	jr $ra
