# Piplined MIPS CPU Simulator #

This project implements a basic single-cycle processor simulator for a reduced MIPS instruction set and extends this single cycle implementation to simulate a pipelined processor. 

## Features: ##
  + The single-cycle processor is implemented using the classical 5-stages—fetch, decode, execute, memory, writeback—with only one instruction going through the stages at a time.
  + The pipelined processor implemented by extending the single cycle processor detects and takes care of piepline hazards.
  + Branch and jump instructions are implemented with forwarding as appropriate in the decode stage of the pipelined processor and static branch prediction is used to assume branches are not taken. 
  + The instructions that the simulator supports are: add, addi, and, andi, beq, bne, j, jal, jr, lui, lw, nor, or, ori, slt, slti, sll, srl, sw, sub, sra, xor, xori, syscall 	

## Requirements: ##
  + A C compiler is required to run this program. 
  + 2 text files are required, one containing the text segment of the instructions and one containing the data segment of the instructions in hexadecimal are generated using QtSpim. 
  + The text files are later converted to a binary file using create-prog.
  + The simulator takes the binary file as a command line argument.
  + If you don't have the Linux OS, you will be required to have the Linux sub-system installed on your system to run the program from the command line.

### Download Links: ###
  + QtSpim	http://spimsimulator.sourceforge.net/
  + Linux subsystem https://docs.microsoft.com/en-us/windows/wsl/install-win10

# Instructions: ##

## Making a new test case ##

#### QtSpim: ####
  + Open QtSpim
  + Load the asm code for the instruction being tested
  + Run the program
  + Copy the user text and user data segments to a text editor and save as text.txt and data.txt respectively
  + **Note: You must reinitialize and load the file before running the program again.**

#### Text.txt: ####
  + Example line: [00400000] 8fa40000  lw $4, 0($29)            ; 183: lw $a0 0($sp) # argc 
  + Only keep the hexadecimal 8fa40000
  + Do this for all the lines
  + Replace the first 5 hexadecimal with 00000000
  + Make sure there is a new line after the last hexadecimal

#### Data.txt ####
  + Example line: [10010000] 6c617473 676e696c 726f7720 2164656b  s t a l l i n g   w o r k e d ! 
  + Only keep the hexadecimals -- 6c617473 676e696c 726f7720 2164656b
  + Place each hexadecimal on a new line in the same order
  + Do the same for all the lines while making sure there are no whitespaces following each hexadecimal
  + Make sure there is a new line after the last hexadecimal
  + If there are no such lines, a simple 00000000 and a new line will suffice
  
#### Creating input file for simulator: ####
  + Compile the text and data files to form a binary file
  + Be in the directory of the text and data files of the test case
  + The create-prog file required to compile the text and data files into a binary file is under util directory
  + Hence, the command ```./../util/create-prog data.txt text.txt test_file.sim``` is used

## Running a test case ##

#### Compiling the Simulator: ####
  + Compile the .c and .h files to form an executable simulator binary file (using gcc command)
  + Since the simulator source files for single-cycle and pipeline are in different folders, the gcc command will be different
  + Be in the directory of all the other .c and .h files
  + To get a single-cycle simulator file, use command -- gcc *.c *.h singlecycle/singlecycle.c -o simulator
  + To get a pipeline simulator file, use command -- gcc *.c *.h pipeline/pipeline.c -o simulator
  
#### Running the test files: ####
  + Be in the directory of the simulator binary file
  + (Assuming name of test case directory is test1)
  + Use command -- ./simulator test_cases/test1/test1.sim  

# Test Cases: #
## Details of test case CPU STATS: ##
  + Flushes and Stalls are counted as useless instructions
  + nop is used to mean 0x00000000 that was either included by the programmer or from the QTSpim conversion and is counted as a useful instruction

### Name of test case: one_stall ###
### Purpose of test case: one_stall ###
To test for a stall. 
Since lw $t5, 0($sp) is the 2nd preceding instruction of beq $t5, $t1, win , a stall is necessary (lw is still in MEM and beq is in ID).
Also a stall will be needed for addi $t1, $0, 5 (addi is in EX and beq is in ID).
If stalling is successful, the beq should work and hence output "stalling worked!!!" as well.
### ASM code of test case: one_stall ###
```
.data
	success:	.asciiz "stalling worked!!!"
	failure:	.asciiz "stalling didn't work..."
.text
main:
	addi $t0, $0, 5
	addi $sp, $sp, -4
	sw $t0, 0($sp)
	lw $t5, 0($sp)
	addi $t1, $0, 5
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
```
### Data.txt of test case: one_stall ###
```
6c617473
676e696c
726f7720
2164656b
73002121
6c6c6174
20676e69
6e646964
77207427
2e6b726f
00002e2e
00000000

```
### Text.txt of test case: one_stall ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
20080005
23bdfffc
afa80000
8fad0000
20090005
11a90007
3c011001
34240013
34020004
0000000c
3402000a
0000000c
3c041001
34020004
0000000c
3402000a
0000000c

```
### CPU STATS of test case: one_stall ###
For single-cycle processor:
```
stalling worked!!!


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 1
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 1
        beq = 1
        bne = 0
        nop = 5

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 17
        Useful Instructions = 17
        CPI (Cycles per Instruction) = 1.00  
```
For pipeline processor:
```
stalling worked!!!


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 1
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 1
        beq = 1
        bne = 0
        nop = 5

HAZARD CORRECTIONS:
        Stalls = 1
        Flushes = 2

CPI PERFORMANCE:
        Cycles = 24
        Useful Instructions = 17
        CPI (Cycles per Instruction) = 1.41
```
### Name of test case: two_stall ###
### Purpose of test case: two_stall ###
To test for 2 stalls.
Since lw $t5, 0($sp) is the immediately preceding instruction of beq $t5, $t1, win , 2 stalls are necessary (lw is still in EX when beq is in ID).
If stalls are successful, the beq should work and hence output "stalling worked!!!" as well.
### ASM code of test case: two_stall ###
```
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
```
### Data.txt of test case: two_stall ###
```
6c617473
676e696c
726f7720
2164656b
73002121
6c6c6174
20676e69
6e646964
77207427
2e6b726f
00002e2e
00000000

```
### Text.txt of test case: two_stall ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
20080005
20090005
23bdfffc
afa80000
8fad0000
11a90007
3c011001
34240013
34020004
0000000c
3402000a
0000000c
3c041001
34020004
0000000c
3402000a
0000000c

```
### CPU STATS of test case: two_stall ###
For single-cycle processor:
```
stalling worked!!!


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 1
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 1
        beq = 1
        bne = 0
        nop = 5

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 17
        Useful Instructions = 17
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
stalling worked!!!


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 1
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 1
        beq = 1
        bne = 0
        nop = 5

HAZARD CORRECTIONS:
        Stalls = 2
        Flushes = 2

CPI PERFORMANCE:
        Cycles = 25
        Useful Instructions = 17
        CPI (Cycles per Instruction) = 1.47
```
### Name of test case: bne ###
### Purpose of test case: bne ###
To test if the bne instructions work.
Will branch and continue to loop as long as $t0 != $t1, which has the value of 25.
If successful, the program will end with register $t0 holding 25.
### ASM code of test case: bne ###
```
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
```
### Data.txt of test case: bne ###
```
00000000

```
### Text.txt of test case: bne ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
20080005
20090019
21080005
200b00c8
1509fffe
03e00008

```
### CPU STATS of test case: bne ###
For single-cycle processor:
```
                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 10
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 4
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 24
        Useful Instructions = 24
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 10
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 4
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 5

CPI PERFORMANCE:
        Cycles = 33
        Useful Instructions = 24
        CPI (Cycles per Instruction) = 1.38
```
### Name of test case: jump_pipeline ###
### Purpose of test case: jump_pipeline ###
To test the jump instruction. If jump is successful, the program will output "jump worked" as well.
### ASM code of test case: jump_pipeline ###
```
.data
	worked: .asciiz "jump worked"
	fail: .asciiz "jump didn't work"
.text
	main:
		j loop
		addi $v0, $0, 4
		la $a0, fail
		syscall
		jr $ra
	loop:
		addi $v0, $0, 4
		la $a0, worked
		syscall
		jr $ra

```
### Data.txt of test case: jump_pipeline ###
```
706d756a
726f7720
0064656b
706d756a
64696420
2074276e
6b726f77
00000000

```
### Text.txt of test case: jump_pipeline ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
0810000f
20020004
3c011001
3424000c
0000000c
03e00008
20020004
3c041001
0000000c
03e00008

```
### CPU STATS of test case: jump_pipeline ###
For single-cycle processor:
```
jump worked


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 1
        andi = 0
        lui = 1
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 1
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 14
        Useful Instructions = 14
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
jump worked


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 1
        andi = 0
        lui = 1
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 1
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 4

CPI PERFORMANCE:
        Cycles = 22
        Useful Instructions = 14
        CPI (Cycles per Instruction) = 1.57
```
### Name of test case: addi ###
### Purpose of test case: addi ###
To test if the addi instructions work. If successful, it will also output 5, which is stored in register $t0.
### ASM code of test case: addi ###
```
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
```
### Data.txt of test case: addi ###
```
00000000

```
### Text.txt of test case: addi ###
```
00000000            
00000000
00000000
00000000
00000000
00000000
00000000
00000000
00000000
20080005
20090006
200a000b
200d0007
00082020
20020001
200e0015
200f0016
20180017
20110008
0000000c
200b0001
200c000c
2002000a
20120009
0000000c

```
### CPU STATS of test case: addi ###
For single-cycle processor:
```
5


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 1
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 13
        andi = 0
        lui = 0
        ori = 0
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 0

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 9

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 25
        Useful Instructions = 25
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
5


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 1
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 13
        andi = 0
        lui = 0
        ori = 0
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 0

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 9

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 29
        Useful Instructions = 25
        CPI (Cycles per Instruction) = 1.16
```
### Name of test case: lw_sw ###
### Purpose of test case: lw_sw ###
To test if the lw and sw instructions work. If successful, then it will store correctly the value read by lw from stack memory into another position in the stack memory.
### ASM code of test case: lw_sw ###
```
.data
.text
main:
	addi $t0, $0, 5
	addi $sp, $sp, -8
	sw $t0, 0($sp)
	lw $t5, 0($sp)
	sw $t5, 4($sp)
	addi $sp, $sp, 8
	li $v0, 10
	syscall

```
### Data.txt of test case: lw_sw ###
```
00000000

```
### Text.txt of test case: lw_sw ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
20080005
23bdfff8
afa80000
8fad0000
afad0004
23bd0008
3402000a
0000000c

```
### CPU STATS of test case: lw_sw ###
For single-cycle processor:
```
                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 2
        beq = 0
        bne = 0
        nop = 5

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 14
        Useful Instructions = 14
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 0
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 3
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 1
        sw = 2
        beq = 0
        bne = 0
        nop = 5

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 1

CPI PERFORMANCE:
        Cycles = 19
        Useful Instructions = 14
        CPI (Cycles per Instruction) = 1.36
```
### Name of test case: la ###
### Purpose of test case: la ###
To test if the la instruction works. If successful, it will correctly print the string "awesome" stored in data memory.
### ASM code of test case: la ###
```
.data
	name:	.asciiz "awesome"
.text
	main:
		la $a0, name
		addi $v0, $0, 4
		syscall
		jr $ra
```
### Data.txt of test case: la ###
```
73657761
00656d6f

```
### Text.txt of test case: la ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
3c041001
20020004
0000000c
03e00008

```
### CPU STATS of test case: la ###
For single-cycle processor:
```
awesome


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 1
        andi = 0
        lui = 1
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 13
        Useful Instructions = 13
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
awesome


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 1
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 2
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 1
        andi = 0
        lui = 1
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 1

Special Type Instructions:
        lw = 0
        sw = 0
        beq = 0
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 2

CPI PERFORMANCE:
        Cycles = 19
        Useful Instructions = 13
        CPI (Cycles per Instruction) = 1.46
```
### Name of test case: hw3 ###
### Purpose of test case: hw3 ###
This is a general test case that has most instructions of the simulators. If successful, it will terminate successfully since it uses jr to exit from main.
### ASM code of test case: hw3 ###
```
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
```
### Data.txt of test case: hw3 ###
```
00000000

```
### Text.txt of test case: hw3 ###
```
00000000
00000000
00000000
00000000
00000000
0c100017
00000000
3402000a
0000000c
23bdfff8
afbf0000
afa40004
0004402a
11000004
2084ffff
0c100009
08100012
20020000
8fbf0000
8fa40004
23bd0008
00441020
03e00008
23bdfff8
afbf0000
afb00004
20100000
20040002
0004402a
11000005
0c100009
02028020
2084ffff
0810001c
22020000
8fb00004
8fbf0000
23bd0008
03e00008

```
### CPU STATS of test case: hw3 ###
For single-cycle processor:
```
                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 7
        and = 0
        jr = 6
        nor = 0
        or = 0
        slt = 8
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 22
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 5
        jal = 6

Special Type Instructions:
        lw = 12
        sw = 12
        beq = 8
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 94
        Useful Instructions = 94
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 7
        and = 0
        jr = 6
        nor = 0
        or = 0
        slt = 8
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 1
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 22
        andi = 0
        lui = 0
        ori = 1
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 5
        jal = 6

Special Type Instructions:
        lw = 12
        sw = 12
        beq = 8
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 9
        Flushes = 20

CPI PERFORMANCE:
        Cycles = 127
        Useful Instructions = 94
        CPI (Cycles per Instruction) = 1.35
```
### Name of test case: jal_pipeline ###
### Purpose of test case: jal_pipeline ###
To test the jal and jr instructions for both the single-cycle and pipeline processors. If successful, it outputs the strings "we jumped to loop2" and "jal worked for god's sake", as well as the integers 11 and 11.
### ASM code of test case: jal_pipeline ###
```
.data
	worked: .asciiz "jal worked for god's sake\n"
	jumped: .asciiz "we jumped to loop2\n"
.text
	main:
		addi $sp, $sp, -8
		sw $ra, 0($sp)
		jal loop1
		lw $ra, 0($sp)
		addi $v0, $0, 1
		addi $a0, $0, 11
		syscall
		jr $ra
		loop1:
			sw $ra, 4($sp)
			jal loop2
			addi $v0, $0, 4 
			la $a0, worked
			syscall
			lw $ra, 4($sp)
			addi $v0, $0, 1
			addi $a0, $0, 11
			syscall
			jr $ra
	loop2:
		addi $v0, $0, 4
		la $a0, jumped
		syscall
		jr $ra
```
### Data.txt of test case: jal_pipeline ###
```
206c616a
6b726f77
66206465
6720726f
7327646f
6b617320
77000a65
756a2065
6465706d
206f7420
706f6f6c
00000a32

```
### Text.txt of test case: jal_pipeline ###
```
00000000
00000000
00000000
00000000
00000000
0c100009
00000000
3402000a
0000000c
23bdfff8
afbf0000
0c100011
8fbf0000
20020001
2004000b
0000000c
03e00008
afbf0004
0c10001b
20020004
3c041001
0000000c
8fbf0004
20020001
2004000b
0000000c
03e00008
20020004
3c011001
3424001b
0000000c
03e00008

```
### CPU STATS of test case: jal_pipeline ###
For single-cycle processor:
```
we jumped to loop2
jal worked for god's sake
1111


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 3
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 5
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 7
        andi = 0
        lui = 2
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 3

Special Type Instructions:
        lw = 2
        sw = 2
        beq = 0
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 0

CPI PERFORMANCE:
        Cycles = 32
        Useful Instructions = 32
        CPI (Cycles per Instruction) = 1.00
```
For pipeline processor:
```
we jumped to loop2
jal worked for god's sake
1111


                CPU STATS:

        INSTRUCTION COUNTER:

R-Format Instructions:
        add = 0
        and = 0
        jr = 3
        nor = 0
        or = 0
        slt = 0
        sll = 0
        srl = 0
        sub = 0
        sra = 0
        xor = 0
        syscall = 5
        addu = 0
        Unimplemented R Types = 0

I-Format Instructions:
        addi = 7
        andi = 0
        lui = 2
        ori = 2
        slti = 0
        xori = 0
        Unimplemented I Types = 0

J-Format Instructions:
        j = 0
        jal = 3

Special Type Instructions:
        lw = 2
        sw = 2
        beq = 0
        bne = 0
        nop = 6

HAZARD CORRECTIONS:
        Stalls = 0
        Flushes = 6

CPI PERFORMANCE:
        Cycles = 42
        Useful Instructions = 32
        CPI (Cycles per Instruction) = 1.31
```

# Errors: #
  + As of now, the program is running without any errors.

# Problems: #
  + As of now, there aren't any problems in the program. 
  
# Workload Distribution: #
  + Work was distributed equally among all team members.
  + Kaleshwar Singh implemented Fetch and Decode stages and moved branch instructions to the ID stage in the pipelined version.
  + Hencching Limbu implemented the Execute stage and the Hazard Detection unit for the pipelined version.
  + Zaykha Kyaw San implemented the Memory stage and forwarding for the pipelined version. 
  + Mahia Tasneem implemented the Writeback stage and forwarding for the pipelined version.

# Contributors: #
  + Kaleshwar Singh 
  + Mahia Tasneem
  + Henchhing Limbu
  + Zaykha Kyaw San
