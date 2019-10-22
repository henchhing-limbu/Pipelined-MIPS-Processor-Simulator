.data
	name: .asciiz "Group 14\n"
	total_members: .word 4
	group_members: .asciiz "Kalesh \n Henchhing \n Zaykha \n Mahia \n"	
.text
	main:
		la $a0, name
		addi $v0, $0, 4
		syscall
		lw $a0, total_members
		addi $v0, 1
		syscall
		la $a0, group_members
		addi $v0, 4
		syscall
		jr $ra
		 