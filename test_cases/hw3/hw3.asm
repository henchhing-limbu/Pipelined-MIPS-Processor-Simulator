.data
.text
fun:     # int fun(int s) { 
	addi $sp, $sp, -8 
	sw $ra, 0($sp) 
	sw $a0, 4($sp) 
	slt $t0, $0, $a0 # if (s > 0) 
	beq $t0, $0, base # { 
	addi $a0, $a0, -1 # s--; 
	jal fun   #  r = fun(s); 
	j done   # } 
base:
	addi $v0, $zero, 0 # else r = 0;
done:
	lw $ra, 0($sp)   
	lw $a0, 4($sp) 
	addi $sp, $sp, 8 
	add $v0, $v0, $a0 # r = r + s;
	jr $ra   # return r; }
main:    # int main() { 
	addi $sp, $sp, -8  
	sw $ra, 0($sp) 
	sw $s0, 4($sp) 
	addi $s0, $zero, 0 # x = 0 
	addi $a0, $zero, 2 # i = 2
loop:
	slt $t0, $0, $a0 # while ( i > 0) 
	beq $t0, $0, end # { 
	jal fun 
	add $s0, $s0, $v0 #   x = x + fun(i); 
	addi $a0, $a0, -1 #   i--; 
	j loop   # }
end:
	addi $v0, $s0, 0 
	lw $s0, 4($sp) 
	lw $ra, 0($sp) 
	addi $sp, $sp, 8 
	jr $ra # return x; }