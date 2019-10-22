.data
.text
main:
	addi $t0, $0, 5
	addi $t1, $0, 25     
	loop:
		addi $t0, $t0, 5
		addi $t3, $0, 200
		bne $t0, $t1, loop		
	jr $ra
