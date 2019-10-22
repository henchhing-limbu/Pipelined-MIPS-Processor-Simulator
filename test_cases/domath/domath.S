# domath.S 
# Kaleshwar Singh
# Mahia Tasneem
# Gedare Bloom
#

# Variables in the .data section.
.data
    prompt1:    .asciiz     "Please enter an integer: "
	prompt2:    .asciiz     "Please enter an operator (+, -, *, /): "
	error_msg:  .asciiz     "Error: invalid arithmetic operation 'OP'." 
	largeNum:	.space		20
	nan:		.asciiz		"NaN"
	overflow_error:	.asciiz	" (Error: Arithmetic overflow)" 

# Instructions in the .text section.
.text

# main() is the program entry point. It is responsible for
# reading two integers and an operator, and passing the read
# values to do_math
main:
	# Maintain a proper call stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)

    # Prompt the user to enter the 1st integer
	li $v0, 4
	la $a0, prompt1
	syscall

	# Read the 1st integer
	li $v0, 5
	syscall

	# Put input integer into argument register
	ori $a1, $v0, 0
    
	# Prompt the user to enter the sign
	li $v0, 4
	la $a0, prompt2
	syscall
	
	# Read the the sign character
	li $v0, 12
	syscall
	
	# Put the input sign into argument register
	ori $a2, $v0, 0
		
	# Print a new line character
	li $v0, 11
	li $a0, 10      # 10 == '\n'
	syscall

	# Prompt the user to enter the 2nd integer
	li $v0, 4
	la $a0, prompt1
	syscall 

	# Read the 2nd integer
	li $v0, 5
	syscall
	
	# Put input integer into argument register
	ori $a3, $v0, 0
	
	# Call do_math
	jal do_math

	# Restore return address and unwind the stack
	lw $ra, 0($sp)
	addi $sp, $sp, 4

	# return 0 from main using jr (do not use the 'exit' syscall)

	li $v0, 10
	jr $ra

# do_math takes three register arguments containing two integers (X, Y)
# and a character indicating an arithmetic operator (OP),
# invokes a subroutine to calculate X OP Y, and
# prints the result or an error message.

do_math:
	# Maintain a proper call stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)

	# store values into the stack
	addi $sp, $sp, -4
	sw $a1, 0($sp)
	addi $sp, $sp, -4
	sw $a2, 0($sp)
	addi $sp, $sp, -4
	sw $a3, 0($sp)

	li $s2, 0		# Initialize division by zero check to false

    addition:
		bne $a2, '+', subtraction
		jal do_add
		j print_result
	
	subtraction:
		bne $a2, '-', multiplication
		jal do_subtract
		j print_result

	multiplication:
		bne $a2, '*', division
		
		li $s0, 0			# We will use $s0 to manage the sign of our multiplication
		
		bge $a1, 0, secondNumNegative
		sub $a1, $zero, $a1		# Convert to absolute value
		addi $s0, $s0, 1

		secondNumNegative:
			bge $a3, 0, bothPosOrNeg
			sub $a3, $zero, $a3		# Convert to absolute value
			addi $s0, $s0, 1

		bothPosOrNeg:
			jal do_multiply
			j print_result

    division:
		bne $a2, '/', error
		# -----------------------------------------------------------------------
		li $s0, 0		# We will use $s0 to store the sign of the dividend
		li $s1, 0		# We will use $s1 to store teh sign of the divisor
		
		bge $a1, 0, divisorNegative
		sub $a1, $zero, $a1
		li $s0, 1

		divisorNegative:
			bge $a3, 0, callDivide
			sub $a3, $zero, $a3
			li $s1, 1

		callDivide:
		# -----------------------------------------------------------------------
			beq $a3, 0, divisorZero
			jal do_divide
			j print_result
			
		divisorZero:
			li $s2, 1
			j print_result
	error:
		li $v0, 4
		la $a0, error_msg
		syscall

	print_result:
		# retrieve values from the stack
		lw $a3, 0($sp)
		addi $sp, $sp, 4
		lw $a2, 0($sp)
		addi $sp, $sp, 4
		lw $a1, 0($sp)
		addi $sp, $sp, 4

		# Get the result returned from the operation
		add $t0, $v0, $zero
		add $t1, $v1, $zero

        # Print the 1st integer
		li $v0, 1
		add $a0, $a1, $zero
		syscall

        # Print a space
		li $v0, 11
		li $a0, ' '      # 32 - Is the ASCII decimal value of ' '
		syscall

		# Print the sign character
		li $v0, 11
		add $a0, $a2, $zero
		syscall

		# Print a space
		li $v0, 11
		li $a0, ' '      # 32 - Is the ASCII decimal value of ' '
		syscall

		# Print the 2nd integer
		li $v0, 1
		add $a0, $a3, $zero
		syscall

		# Print space
		li $v0, 11
		li $a0, ' '      # 32 - Is the ASCII decimal value of ' '
		syscall

		# Print the equal sign charcter
        li $v0, 11
		li $a0, '='      # 61 - Is the ASCII decimal value of '='
		syscall

		# Print space
		li $v0, 11
		li $a0, ' '      # 32 - Is the ASCII decimal value of ' '
		syscall

		beq $s2, 1, notANumber

		# Print the integer result
		beq $a2, '*', multiply	# If sign is '*' --> multiply

		bne $a2, '/', result_32bits
		beq $s0, $s1, result_32bits
		beq $t0, 0, result_32bits

		# Print a negative sign
		li $v0, 11
		li $a0, '-'
		syscall

		result_32bits:
			li $v0, 1
			or $a0, $t0, $zero
			syscall
		
		bne $a2, '/', overflowCheck		# If sign argument is not '/' --> overflowCheck

		# Print space
		li $v0, 11
		li $a0, ' '      # 32 - Is the ASCII decimal value of ' '
		syscall

		beq $t1, 0, overflowCheck 			# Don't print a remainder if it is zero

		# Print the remainder for division
		# Print 'R' for remainder
		li $v0, 11
		li $a0, 'R'		# 82 == 'R'
		syscall

		# Print space
		li $v0, 11
		li $a0, ' '      # 32 - Is the ASCII decimal value of ' '
		syscall

		beq $s0, 0, remainderValue

		# Print a negative sign
		li $v0, 11
		li $a0, '-'
		syscall
		
		remainderValue:		
			# Print the remainder from $t1
			li $v0, 1
			ori $a0, $t1, 0
			syscall

		j overflowCheck

		multiply:
			
			beq $t0, 0, result_32bits		# If li == 0 --> print 32bits

			# Check and print the sign of the product
			bne $s0, 1, setArgs				# If the number of negative signs is not 1 --> Set args
			li $v0, 11
			li $a0, '-'
			syscall

			beq $t1, 0, result_32bits
			
			setArgs:
				ori $a0, $t0, 0
				ori $a1, $t1, 0
				jal print64bitNum

			j overflowCheck

	notANumber:
		li $v0, 4
		la $a0, nan
		syscall 

	overflowCheck:
		beq $a2, '*', return
		beq $a2, '/', return
		beq $t1, 0, return
		
		li $v0, 4		# Print overflow error message
		la $a0, overflow_error
		syscall 

	return:
		# Restore return address and unwind the stack
		lw $ra, 0($sp)
		addi $sp, $sp, 4
		jr $ra

do_multiply:
	# Store the return value of do_multiply on the stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)

	li $v0, 0					# Initialize the lo register ($v0) to 0
	li $v1, 0					# Initialize the hi register ($v1) to 0

	ori $t0, $a1, 0			# Initialize add0 register to the multiplicand
	li $t1, 0					# Initialize add1 register to 0
	
	ori $t5, $a3, 0			# Copy the multiplier

	beq $a3, 0, return3			# If the multiplier or multiplicand is zero
	beq $a1, 0, return3			# --> return3
		
	li $t2, 0					# Initialize a counter to 0

	loop4:
		# Check if the multiplier bit is 1
		andi $t3, $t5, 1		# If $t3 is 1 then the multiplier bit was 1
		bne $t3, 1, next		# Else --> next
		
		# Handles for addition and overflow form lo
		ori $t3, $v0, 0			# Get the MSB of $v0
		srl $t3, $t3, 31

		ori $t4, $t0, 0			# Get the MSB of $t0
		srl $t4, $t4, 31
		
		# Store values on the stack
		addi $sp, $sp, -4
		sw $v0, 0($sp)
		addi $sp, $sp, -4
		sw $v1, 0($sp)
		addi $sp, $sp, -4
		sw $t0, 0($sp)
		addi $sp, $sp, -4
		sw $t1, 0($sp)
		addi $sp, $sp, -4
		sw $t2, 0($sp)
		addi $sp, $sp, -4
		sw $t5, 0($sp)

		# Get the sum of the 2 MSBs
		ori $a1, $t3, 0
		ori $a3, $t4, 0
		jal do_add
		ori $t3, $v0, 0
		
		# Retrieving the values from the stack

		lw $t5, 0($sp)
		addi $sp, $sp, 4
		lw $t2, 0($sp)
		addi $sp, $sp, 4
		lw $t1, 0($sp)
		addi $sp, $sp, 4
		lw $t0, 0($sp)
		addi $sp, $sp, 4
		lw $v1, 0($sp)
		addi $sp, $sp, 4
		lw $v0, 0($sp)
		addi $sp, $sp, 4
		

		# Store values on the stack
		addi $sp, $sp, -4
		sw $v1, 0($sp)
		addi $sp, $sp, -4
		sw $t0, 0($sp)
		addi $sp, $sp, -4
		sw $t1, 0($sp)
		addi $sp, $sp, -4
		sw $t2, 0($sp)
		addi $sp, $sp, -4
		sw $t5, 0($sp)

		# Add the multiplicand (add0/ $t0) to lo ($v0)
		ori $a1, $v0, 0
		ori $a3, $t0, 0
		jal do_add

		# Retrieving the values from the stack
		lw $t5, 0($sp)
		addi $sp, $sp, 4
		lw $t2, 0($sp)
		addi $sp, $sp, 4
		lw $t1, 0($sp)
		addi $sp, $sp, 4
		lw $t0, 0($sp)
		addi $sp, $sp, 4
		lw $v1, 0($sp)
		addi $sp, $sp, 4


		beq $t3, 0, next2			# If the sum of MSBs== 0 --> noOverflow
		beq $t3, 2, overflow			# If the sum of MSBs== 2 --> overflow

		ori $t4, $v0, 0			# Get the MSB of $v0
		srl $t4, $t4, 31
		
		bne $t4, 0, next2			# If the 2 MSBs are different and MSB of sum is not 0 --> noOverflow
			
		overflow:
			# ---------------------------------------------------------------
			# Store the values onto the stack
			addi $sp, $sp, -4
			sw $t0, 0($sp)
			addi $sp, $sp, -4
			sw $t1, 0($sp)
			addi $sp, $sp, -4
			sw $t2, 0($sp)
			addi $sp, $sp, -4
			sw $t3, 0($sp)
			addi $sp, $sp, -4
			sw $v0, 0($sp)

			# Add 1 to the hi ($v1) register
			ori $a1, $v1, 0
			jal increment
			ori $v1, $v0, 0

			# Retrieve the values from the stack
			lw $v0, 0($sp)
			addi $sp, $sp, 4
			lw $t3, 0($sp)
			addi $sp, $sp, 4
			lw $t2, 0($sp)
			addi $sp, $sp, 4
			lw $t1, 0($sp)
			addi $sp, $sp, 4
			lw $t0, 0($sp)
			addi $sp, $sp, 4
			# ---------------------------------------------------------------
			
		next2:
			# Store values on the stack
			addi $sp, $sp, -4
			sw $v0, 0($sp)
			addi $sp, $sp, -4
			sw $t0, 0($sp)
			addi $sp, $sp, -4
			sw $t1, 0($sp)
			addi $sp, $sp, -4
			sw $t2, 0($sp)
			addi $sp, $sp, -4
			sw $t5, 0($sp)

			# Add add1 to hi ($v1) 
			ori $a1, $v1, 0
			ori $a3, $t1, 0
			jal do_add
			ori $v1, $v0, 0
		
			# Retrieving the values from the stack
			lw $t5, 0($sp)
			addi $sp, $sp, 4
			lw $t2, 0($sp)
			addi $sp, $sp, 4
			lw $t1, 0($sp)
			addi $sp, $sp, 4
			lw $t0, 0($sp)
			addi $sp, $sp, 4
			lw $v0, 0($sp)
			addi $sp, $sp, 4

		next:
			ori $t3, $t0, 0		# Get the MSB of the add0 register
			srl $t3, $t3, 31

			sll $t0, $t0, 1			# Shift add0 register left by 1
			sll $t1, $t1, 1			# Shift add1 register left by 1

			or $t1, $t1, $t3		# Add the MSB of add0 reg to add1 reg
		
			srl $t5, $t5, 1			# Shift the multiplier right by 1
			
			# ---------------------------------------------------------------
			# Store the values onto the stack
			addi $sp, $sp, -4
			sw $t0, 0($sp)
			addi $sp, $sp, -4
			sw $t1, 0($sp)
			addi $sp, $sp, -4
			sw $t3, 0($sp)
			addi $sp, $sp, -4
			sw $v0, 0($sp)

			# Increment the counter
			ori $a1, $t2, 0
			jal increment
			ori $t2, $v0, 0

			# Retrieve the values from the stack
			lw $v0, 0($sp)
			addi $sp, $sp, 4
			lw $t3, 0($sp)
			addi $sp, $sp, 4
			lw $t1, 0($sp)
			addi $sp, $sp, 4
			lw $t0, 0($sp)
			addi $sp, $sp, 4
			# ---------------------------------------------------------------

			beq $t2, 32, return3		# If shift == 32 --> return3
			j loop4

	 return3:
		# Retrieve the retrun address from the stack
		lw $ra, 0($sp)
		addi $sp, $sp, 4
		jr $ra


print64bitNum:
	# Get the arguments
	ori $t0, $a0, 0
	ori $t1, $a1, 0

	li $t2, 0				# Initialize the digit count to 0
	li $t3, 19				# Set the index of the least significant digit to 19
	li $t4, 16				# Set the divisor to 16
	li $t5, 0				# Initialize a counter to 0

	loop1:
		addi $t5, $t5, 1					# Increment the counter
		divu $t0, $t4						# Divide the number in the lower register by 16
		mflo $t0							# Set the dividend to the quotient
		mfhi $t6							# Get the remainder
		addi $sp, $sp, -1					# Store the reaminder on the stack
		sb $t6, 0($sp)						# We can use a byte to store since the largest resminder is 15
		beq $t5, 8, loop2					# If we have divided by 16 eight time --> loop2
		j loop1

	loop2:
		addi $t5, $t5, 1					# Increment the counter
		divu $t1, $t4						# Divide the number in the higher register by 16
		mflo $t1							# Set the dividend to the quotient
		mfhi $t6							# Get the remainder
		addi $sp, $sp, -1					# Store the remainder on the stack
		sb $t6, 0($sp)	
		beq $t1, 0, decimal					# If dividend == 0 --> decimal
		j loop2

	decimal:
		# Ensure that the remainder register is set to 0 before beginning
		divu $t4, $t4						# Sets hi to 0
		li $t0, 10							# Set the divisor to 10
		li $t1, 0							# Initialize a second counter to 0

		loop3:
			mfhi $t6						# Get the remainder
			mult $t6, $t4					# Multiply the remainder by 16
			mflo $t6						# Get the resulting product
			
			lb $t7, 0($sp)					# Get the hexadecimal digit
			add $t7, $t7, $t6				# Add (remainder * 16) to the hex digit
			divu $t7, $t0					# Divide the result by 10
			mflo $t6						# Get the quotient
			sb $t6, 0($sp)					# Store the quotient on the stack
			addi $sp, $sp, 1

			addi $t1, $t1, 1				# Increment the second counter

			beq $t5, $t1, saveDigit			# If the 2 counters are equal --> saveDigit

			j loop3
		
		saveDigit:
			la $t6, largeNum				# Get the address of the space to store the number
			add $t6, $t6, $t3				# Add the digit index to the address
			mfhi $t7						# Get the remainder
			sb $t7, 0($t6)					# Store the digit in its correct position
			addi $t3, $t3, -1				# Decrement the digit index
			beq $t3, -1, continue			# If digit index == -1 --> continue
			sub $sp, $sp, $t1				# Reset the stack pointer to the position of the first digit
			j decimal

	continue:
		la $t6, largeNum					# Get the address of the space where the digits were stored
	
	digits:
		lb $t7, 0($t6)						# Get the stored digit
		bne $t7, 0, printDigits				# If digit > 0 --> printDigits
		addi $t2, $t2, 1					# Increment the digit count
		addi $t6, $t6, 1					# Increment the address
		j digits

	printDigits:
		lb $a0, 0($t6)						# Load the digit to print
		li $v0, 1							# Print the digit
		syscall

		addi $t2, $t2, 1					# Increment the digit count
		addi $t6, $t6, 1					# Increment the address
		beq $t2, 20, return2 				# If digit count == 20 --> return
		j printDigits

	return2:
		jr $ra



half_adder:
	ori $t1, $a1, 0		# moving arguments to registers
	ori $t2, $a2, 0

	xor $v0, $t1, $t2	# return the sum through v0
	and $v1, $t1, $t2	# return the carry out through v1

	jr $ra

full_adder:
	#store return address of full_adder on the stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)

	ori $t1, $a1, 0		# bit1
	ori $t2, $a2, 0		# bit2
	ori $t3, $a3, 0		# carry in

	ori $a1, $t1, 0		# settign arguments to call half adder
	ori $a2, $t2, 0		

	jal half_adder

	ori $t4, $v1, 0		# storing carryout1 in t4

	ori $a1, $v0, 0		# setting arguments for second half adder
	ori $a2, $t3, 0

	jal half_adder		# final sum returned in $v0

	or $v1, $v1, $t4	# return final carry out

	# retrieve the return address from the stack
	lw $ra, 0($sp)
	addi $sp, $sp, 4

	jr $ra


do_add:
	#store return address of full_adder on the stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)

	ori $t1, $a1, 0		# get arguments
	ori $t2, $a3, 0
	li $t3, 0			# initial carry in to a 32 bit adder is 0

	# storing the signs of the arguments
	ori $t6, $t1, 0		# moving first argument into t6
	srl $t6, $t6, 31	# getting the sign of the first argument
	ori $t7, $t2, 0		# moving second argument to t7	
	srl $t7, $t7, 31	# getting the sign of the second argument

	li $t4, 0			# initialize counter to 0
	li $t5, 0			# register to accumulate the final sum

	loop6:
		and $a1, $t1, 1		# extracting the least significant bits
		and $a2, $t2, 1
		ori $a3, $t3, 0

		# store values onto stack 
		addi $sp, $sp, -4
		sw $t1, 0($sp)
		addi $sp, $sp, -4
		sw $t2, 0($sp)
		addi $sp, $sp, -4
		sw $t3, 0($sp)
		addi $sp, $sp, -4
		sw $t4, 0($sp)

		jal full_adder

		# retrieve values from the stack
		lw $t4, 0($sp)
		addi $sp, $sp, 4
		lw $t3, 0($sp)
		addi $sp, $sp, 4
		lw $t2, 0($sp)
		addi $sp, $sp, 4
		lw $t1, 0($sp)
		addi $sp, $sp, 4

		sllv $v0, $v0, $t4	# shift the sum left by the counter value
		or $t5, $v0, $t5	# or the bitwise sum with the accumulator

		srl $t1, $t1, 1		# getting the next least significant bits
		srl $t2, $t2, 1

		ori $t3, $v1, 0		# storing the carry out as the next carry in 
		
		# ---------------------------------------------------------------
		# Store the values onto the stack
		addi $sp, $sp, -4
		sw $t0, 0($sp)
		addi $sp, $sp, -4
		sw $t1, 0($sp)
		addi $sp, $sp, -4
		sw $t2, 0($sp)
		addi $sp, $sp, -4
		sw $t3, 0($sp)


		# Increment the counter
		ori $a1, $t4, 0
		jal increment
		ori $t4, $v0, 0

		# Retrieve the values from the stack
		lw $t3, 0($sp)
		addi $sp, $sp, 4
		lw $t2, 0($sp)
		addi $sp, $sp, 4
		lw $t1, 0($sp)
		addi $sp, $sp, 4
		lw $t0, 0($sp)
		addi $sp, $sp, 4
	
		# ---------------------------------------------------------------
		beq $t4, 32, continue2		# exit loop if counter = 32
		j loop6

	continue2:
		ori $v0, $t5, 0		# return sum in v0 

		# check for overflow with signed integers : 
		# overflow only occurs when both the operands have the same sign 
		# and the sum has a different sign

		bne $t6, $t7, no_overflow1	# If the signs of the operands are not equalt --> no_overflow1

		ori $t8, $t5, 0		# moving sum into t8
		srl $t8, $t8, 31	# getting the sign of the sum

		beq $t6, $t8, no_overflow1 

		li $v1, 1			# set overflow return value to 1
		j return4

	no_overflow1:
		li $v1, 0			# set overflow return value to 0

	return4:
		# retrieve the return address from the stack
		lw $ra, 0($sp)
		addi $sp, $sp, 4
		jr $ra

do_subtract:
		#store return address of full_adder on the stack
		addi $sp, $sp, -4
		sw $ra, 0($sp)

		ori $t1, $a1, 0		# get arguments
		ori $t2, $a3, 0
		
		lui $t8, 0xFFFF
		ori $t8, $t8, 0xFFFF

		# Negating the second argument
		xor $t2, $t2, $t8 	# xor second operand with -1 to flip the bits

		ori $a1, $t2, 0		# send the arguments for addition
		li $a3, 1			# store 1 to a3

		addi $sp, $sp, -4	# storing value of t1 on stack
		sw $t1, 0($sp)

		jal do_add

		lw $t1, 0($sp)		# retreiving the value from the stack
		addi $sp, $sp, 4

		ori $a1, $t1, 0		# set the arguments for addition
		ori $a3, $v0, 0	

		jal do_add			# the result will be returned in v0 and overflow checked in v1

		# retrieve the return address from the stack
		lw $ra, 0($sp)
		addi $sp, $sp, 4
		jr $ra


increment:
	ori $t1, $a1, 0		# Get the argument
	li $v0, 0			# Initialize the return value to 0
	li $t0, 0  # set $t0 to 1, once first 0 is found
 	li $t3, -1			# Initialize a register to all 1s we will use it 
						# to determine when the the loop has run 32 times 

	loop7:
		andi $t2, $t1, 1		# Get the LSB of number
		sll $t2, $t2, 31
		
		beq $t0, 1, noFlip		# Already seen first 0 
		beq $t2, 0, zeroFlip

		li $t2, 0				# Flips 1 to 0
		j noFlip

		zeroFlip:
			li $t0, 1			# Set seen 0 to 1
			li $t2, 1
			sll $t2, $t2, 31

		noFlip:
			srl $v0, $v0, 1
			or $v0, $v0, $t2
			srl $t1, $t1, 1

		sll $t3, $t3, 1
		beq $t3, 0, return5
		j loop7
	
	return5:
		jr $ra

do_divide:
	# Store the return address on the stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	
	# Get the arguments
	ori $t1, $a1, 0
	ori $t2, $a3, 0
	
	li $t0, 0		# Initialize a counter to 0

	loop8:
		blt $t1, $t2, return8

		# ------------------------------------------------
		# Store the values onto the stack
		addi $sp, $sp, -4
		sw $t0, 0($sp)
		addi $sp, $sp, -4
		sw $t2, 0($sp)

		ori $a1, $t1, 0		# Set the arguments		
		ori $a3, $t2, 0
		jal do_subtract
		ori $t1, $v0, 0

		# Retrieve the values from the stack
		lw $t2, 0($sp)
		addi $sp, $sp, 4
		lw $t0, 0($sp)
		addi $sp, $sp, 4
		# ------------------------------------------------
		
		# Store values onto the stack
		addi $sp, $sp, -4
		sw $t1, 0($sp)
		addi $sp, $sp, -4
		sw $t2, 0($sp)
		addi $sp, $sp, -4
		sw $t3, 0($sp)

		# Increment the counter
		ori $a1, $t0, 0
		jal increment
		ori $t0, $v0, 0

		# Retrieve values from the stack
		lw $t3, 0($sp)
		addi $sp, $sp, 4
		lw $t2, 0($sp)
		addi $sp, $sp, 4
		lw $t1, 0($sp)
		addi $sp, $sp, 4
		# -----------------------------------------------

		j loop8
	
	return8:
		li $v0, 0
		ori $v0, $t0, 0
		li $v1, 0
		ori $v1, $t1, 0
	
		# Retrieve the return address from the stack
		lw $ra, 0($sp)
		addi $sp, $sp, 4
		jr $ra



