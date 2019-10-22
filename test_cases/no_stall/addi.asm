.data
.text
main:
	addi $t0, $0, 5
    addi $t1, $0, 6
    add $a0, $0, $t0
    addi $v0, $0, 1
    syscall
	addi $v0, $0, 10
    syscall