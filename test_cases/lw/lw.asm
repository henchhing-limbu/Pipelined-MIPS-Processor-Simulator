.data
my_word: .asciiz "abcd"
.text
main:
la $t0, my_word
lw $t1, 0($t0)
addi $v0, $0, 10
syscall
