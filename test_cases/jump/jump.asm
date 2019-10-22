.data
.text
main:
	j j2
	
end:
	or $a0, $t2, $0
	li $v0, 1
	syscall
	jr $ra

j1:
	addi $t1, $0, 200
	add $t2, $t1, $t0
	j end
	
j2:
	addi $t0, $0, 500
	j j1