.data
.text
main:
	addi $t0, $0, 5
    addi $t1, $0, 6
    addi $t2, $0, 11
    addi $t5, $0, 7
    add $a0, $0, $t0
    addi $v0, $0, 1
    addi $t6, $0, 21
    addi $t7, $0, 22
    addi $t8, $0, 23
    addi $s1, $0, 8
    syscall
    addi $t3, $0, 1
    addi $t4, $0, 12
	addi $v0, $0, 10
    addi $s2, $0, 9
    syscall