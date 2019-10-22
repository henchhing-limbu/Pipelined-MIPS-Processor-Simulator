/**
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * syscall.c
 *
 * Implementation of the system calls
 */

#ifndef SYSCALL_H
#define SYSCALL_H
#include "syscall.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "memory_structures.h"
#endif

#define DEBUG
#undef DEBUG

int syscall(int num) {
#if defined(DEBUG)
	printf("Entered syscall switch statement\n");
	printf("$v0 or $2 = %d\n", cpu_ctx.GPR[2]);
#endif

	switch (num) {
		case 1:
			sys_print_int(cpu_ctx.GPR[4]);
			break;
		case 4:
			sys_print_string(cpu_ctx.GPR[4]);
			break;
		case 10:
			sys_exit();
			break;
	}
	return 0;
}

int sys_exit() {

	// Add 2 to the cycle count since the stages are backwards and the cycle counter is incremented in the fetch stage and syscall is executed in mem
	if (pipelined) {
		processor_stat.cycle += 2;

		// Add the number of flushes to the cycle count
		processor_stat.cycle += processor_stat.flush;

		// Subtract the number of flushes and stalls from the number of nops
		processor_stat.instruction.nop -= (processor_stat.stall + 2);
	}
	if (pipelined)
		processor_stat.useful_instructions = processor_stat.cycle - processor_stat.flush - processor_stat.stall - 4; // 4 is the time it takes the pipeline to full
	else
		processor_stat.useful_instructions = processor_stat.cycle;
	processor_stat.cpi = (float) processor_stat.cycle / (float) processor_stat.useful_instructions;

    #if defined (DEBUG)
	printf("\nREGISTERS\n");
	printf("$r0 = %d\n", cpu_ctx.GPR[0]);
	printf("$at = %d\n", cpu_ctx.GPR[1]);
	printf("$v0 = %d\n", cpu_ctx.GPR[2]);
	printf("$v1 = %d\n", cpu_ctx.GPR[3]);
	printf("$a0 = %d\n", cpu_ctx.GPR[4]);
	printf("$a1 = %d\n", cpu_ctx.GPR[5]);
	printf("$a2 = %d\n", cpu_ctx.GPR[6]);
	printf("$a3 = %d\n", cpu_ctx.GPR[7]);
	printf("$t0 = %d\n", cpu_ctx.GPR[8]);
	printf("$t1 = %d\n", cpu_ctx.GPR[9]);
	printf("$t2 = %d\n", cpu_ctx.GPR[10]);
	printf("$t3 = %d\n", cpu_ctx.GPR[11]);
	printf("$t4 = %d\n", cpu_ctx.GPR[12]);
	printf("$t5 = %d\n", cpu_ctx.GPR[13]);
	printf("$t6 = %d\n", cpu_ctx.GPR[14]);
	printf("$t7 = %d\n", cpu_ctx.GPR[15]);
	printf("$s0 = %d\n", cpu_ctx.GPR[16]);
	printf("$s1 = %d\n", cpu_ctx.GPR[17]);
	printf("$s2 = %d\n", cpu_ctx.GPR[18]);
	printf("$s3 = %d\n", cpu_ctx.GPR[19]);
	printf("$s4 = %d\n", cpu_ctx.GPR[20]);
	printf("$s5 = %d\n", cpu_ctx.GPR[21]);
	printf("$s6 = %d\n", cpu_ctx.GPR[22]);
	printf("$s7 = %d\n", cpu_ctx.GPR[23]);
	printf("$t8 = %d\n", cpu_ctx.GPR[24]);
	printf("$t9 = %d\n", cpu_ctx.GPR[25]);
	printf("$k0 = %d\n", cpu_ctx.GPR[26]);
	printf("$k1 = %d\n", cpu_ctx.GPR[27]);
	printf("$gp = %d\n", cpu_ctx.GPR[28]);
	printf("$sp = %d\n", cpu_ctx.GPR[29]);
	printf("$s8 = %d\n", cpu_ctx.GPR[30]);
	printf("$ra = %d\n", cpu_ctx.GPR[31]);

	printf("DATA MEMORY\n");
	int i;
	for (i = 8180; i < 8192; ++i)
		printf("Index = %d\t\t Value = %d\n", i, data_memory[i]);
#endif

	print_cpu_stats();		// Print the cpu stats
#if defined(DEBUG)
	printf("Exiting\n");
#endif
	exit(0);
	return 0;
}

int sys_print_int(uint32_t a0) {
	#if defined(DEBUG)
		printf("Value in register $a0 = %d\n", a0);
		printf("Printing integer\n");
	#endif
	printf("%d", (int) (a0));
	return 0;
}

int sys_print_string(uint32_t a0) {
#if defined(DEBUG)
	printf("------------------------------\n");
	printf("Printing string\n");
#endif
	while (1) {
#if defined(DEBUG)
		// printf("The qtspim address = %X\n", a0);
#endif
		uint32_t index = a0 - 0x10010000;
#if defined(DEBUG)
		// printf("The calculated address = %d\n", index);
#endif
		uint8_t ch = data_memory[index];
		if (ch == 0x00)
			break;
		else {
			printf("%c", ch);
			++a0;
		}
	}
#if defined(DEBUG)
	printf("\n------------------------------\n");
#endif
	return 0;
}

int sys_read_int() {
	scanf("%d", &cpu_ctx.GPR[2]);
	return 0;
}

int sys_read_char() {
	char temp;
	scanf("%c", &temp);
	cpu_ctx.GPR[2] = temp;
	return 0;
}

int sys_print_char(uint32_t a0) {
	printf("%c", data_memory[a0 - 0x10010000]);
	return 0;
}

void print_cpu_stats(){
	printf("\n\n\n\t\tCPU STATS:\n");

	printf("\n\tINSTRUCTION COUNTER:\n");

	printf("\nR-Format Instructions:\n");
	printf("\tadd = %d\n", processor_stat.instruction.add);
	printf("\tand = %d\n", processor_stat.instruction.and_ctr);
	printf("\tjr = %d\n", processor_stat.instruction.jr);
	printf("\tnor = %d\n", processor_stat.instruction.nor);
	printf("\tor = %d\n", processor_stat.instruction.or_ctr);
	printf("\tslt = %d\n", processor_stat.instruction.slt);
	printf("\tsll = %d\n", processor_stat.instruction.sll);
	printf("\tsrl = %d\n", processor_stat.instruction.srl);
	printf("\tsub = %d\n", processor_stat.instruction.sub);
	printf("\tsra = %d\n", processor_stat.instruction.sra);
	printf("\txor = %d\n", processor_stat.instruction.xor_ctr);
	printf("\tsyscall = %d\n", processor_stat.instruction.syscall);
	printf("\taddu = %d\n", processor_stat.instruction.addu);
	printf("\tUnimplemented R Types = %d\n", processor_stat.instruction.unimp_r_type);

	printf("\nI-Format Instructions:\n");
	printf("\taddi = %d\n", processor_stat.instruction.addi);
	printf("\tandi = %d\n", processor_stat.instruction.andi);
	printf("\tlui = %d\n", processor_stat.instruction.lui);
	printf("\tori = %d\n", processor_stat.instruction.ori);
	printf("\tslti = %d\n", processor_stat.instruction.slti);
	printf("\txori = %d\n", processor_stat.instruction.xori);
	printf("\tUnimplemented I Types = %d\n", processor_stat.instruction.unimp_i_type);

	printf("\nJ-Format Instructions:\n");
	printf("\tj = %d\n", processor_stat.instruction.j);
	printf("\tjal = %d\n", processor_stat.instruction.jal);

	printf("\nSpecial Type Instructions:\n");
	printf("\tlw = %d\n", processor_stat.instruction.lw);
	printf("\tsw = %d\n", processor_stat.instruction.sw);
	printf("\tbeq = %d\n", processor_stat.instruction.beq);
	printf("\tbne = %d\n", processor_stat.instruction.bne);
	printf("\tnop = %d\n", processor_stat.instruction.nop);

	printf("\nHAZARD CORRECTIONS:\n");
	printf("\tStalls = %d\n", processor_stat.stall);
	printf("\tFlushes = %d\n", processor_stat.flush);

	printf("\nCPI PERFORMANCE: \n");
	printf("\tCycles = %d\n", processor_stat.cycle);
	printf("\tUseful Instructions = %d\n", processor_stat.useful_instructions);
	printf("\tCPI (Cycles per Instruction) = %0.2f\n\n", processor_stat.cpi);
}
