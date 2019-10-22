/**
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * single-cycle.c
 *
 * Drives the simulation of a single-cycle processor
 */

#ifndef CPU_H
#define CPU_H
#include "../cpu.h"
#endif

#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "../memory_structures.h"
#endif

#ifndef SYSCALL_H
#define SYSCALL_H
#include "../syscall.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDINT_H
#define STDINT_H
#include <stdint.h>
#endif

#include <assert.h>


void print_output();
int zero_buffers();


int main( int argc, char *argv[] )
{
#define DEBUG
#undef DEBUG
	FILE *f;
	int i;

	/* Initialize registers and memory to 0 */
	cpu_ctx.pc_register.PC = 0;


	for ( i = 0; i < 32; i++ ) {
		cpu_ctx.GPR[i] = 0;
	}

	/* Initialize stack pointer */
	//TODO: QtSpim's stack pointer begins at 7ffff61c
	cpu_ctx.GPR[29] = 0x7FFFF61C;

	for ( i = 0; i < 1024; i++ ) {
		instruction_memory[i] = 0;
	}

	for ( i = 0; i < 8192; i++ ) {
		data_memory[i] = 0;
	}

	/* Read memory from the input file */
	f = fopen(argv[1], "r");
	assert (f);
	for ( i = 0; i < 8192; i++ ) {
		//fread(&data_memory[i], sizeof(uint8_t), 1, f);
		data_memory[i] = fgetc(f);
#if defined(DEBUG)
		//printf("%u\n", data_memory[i]);
#endif
	}

	for ( i = 0; i < 1024; i++ ) {
		fread(&instruction_memory[i], sizeof(uint32_t), 1, f);
#if defined(DEBUG)
		//printf("%u\n", instruction_memory[i]);
#endif
	}
	fclose(f);

#if defined(DEBUG)
	printf("\nINSTRUCTION MEMORY \n\n");
	for (i = 0; i < 30; ++i) {
		printf("%X\n", instruction_memory[i]);
	}
#endif
    
	while(1) {
		zero_buffers();
		fetch( &if_id );
		decode( &if_id, &id_ex );
		execute( &id_ex, &ex_mem );
		memory( &ex_mem, &mem_wb );
		writeback( &mem_wb );
	}
	return 0;
}

void print_output(){
	printf("\n\t\t\tCPU STATS:\n");

	printf("\n\tCYCLE COUNTER:\n");
	printf("Cycles executed = %d\n\n", processor_stat.cycle);

	printf("\n\tINSTRUCTION COUNTER:\n");

	printf("\nR-FORMAT INSTRUCTIONS:\n");
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

	printf("\nI-FORMAT INSTRUCTIONS:\n");
	printf("\taddi = %d\n", processor_stat.instruction.addi);
	printf("\tandi = %d\n", processor_stat.instruction.andi);
	printf("\tlui = %d\n", processor_stat.instruction.lui);
	printf("\tori = %d\n", processor_stat.instruction.ori);
	printf("\tslti = %d\n", processor_stat.instruction.slti);
	printf("\txori = %d\n", processor_stat.instruction.xori);
	printf("\tUnimplemented I Types = %d\n", processor_stat.instruction.unimp_i_type);

	printf("\nJ-FORMAT INSTRUCTIONS:\n");
	printf("\tj = %d\n", processor_stat.instruction.j);
	printf("\tjal = %d\n", processor_stat.instruction.jal);

	printf("\nSPECIAL TYPES:\n");
	printf("\tlw = %d\n", processor_stat.instruction.lw);
	printf("\tsw = %d\n", processor_stat.instruction.sw);
	printf("\tbeq = %d\n", processor_stat.instruction.beq);
	printf("\tbne = %d\n", processor_stat.instruction.bne);
	printf("\tnop = %d\n", processor_stat.instruction.nop);

	printf("\n");
	printf("Finished running program...\n");
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
#endif
}

int zero_buffers(){
    // enabling pc_write
    cpu_ctx.pc_register.pc_write = 1;
    
    // zeroing the IF_ID buffer
    if_id.instruction = 0;
    if_id.next_pc = 0;
    if_id.IF_ID_write = 1;
    
    // zeroing the ID_EX buffer
    id_ex.next_pc = 0;
    id_ex.opcode = 0;
    id_ex.shamt = 0;
    id_ex.funct = 0;
    id_ex.mem_read = 0;
    id_ex.mem_write = 0;
    id_ex.mem_to_reg = 0;
    id_ex.alu_src = 0;
    id_ex.reg_write = 0;
    id_ex.write_reg = 0;
    id_ex.read_data1 = 0;
    id_ex.read_data2 = 0;
    id_ex.ext_immediate = 0;
    id_ex.rs = 0;
    id_ex.rt = 0;
    id_ex.rd = 0;
    
    // zeroing the EX_MEM buffer
    ex_mem.mem_read = 0;
    ex_mem.mem_write = 0;
    ex_mem.mem_to_reg = 0;
    ex_mem.reg_write = 0;
    ex_mem.write_reg = 0;
    ex_mem.write_data = 0;
    ex_mem.alu_result = 0;
    ex_mem.syscall = 0;
    ex_mem.rt = 0;
    
    // zeroing the MEM_WB buffer
    mem_wb.read_data = 0;
    mem_wb.alu_result = 0;
    mem_wb.mem_to_reg = 0;
    mem_wb.reg_write = 0;
    mem_wb.write_reg = 0;
    mem_wb.syscall = 0;
    mem_wb.mem_read = 0;
    
    // zeroing the TEMP buffer
    temp_buffer.reg_write = 0;
    temp_buffer.write_reg = 0;
    temp_buffer.write_data = 0;
        
    return 0;
}
