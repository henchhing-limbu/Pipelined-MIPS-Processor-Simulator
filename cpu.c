/*
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * cpu.c
 *
 * Implementation of simulated processor.
 */
 
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

#ifndef DECODE_H
#define DEOCDE_H
#include "decode.h"
#endif

#ifndef EXECUTE_H
#define EXECUTE_H
#include "execute.h"
#endif

#ifndef MEMORY_H
#define MEMORY_H
#include "memory.h"
#endif

#ifndef WRITEBACK_H
#define WRITEBACK_H
#include "writeback.h"
#endif

#ifndef SYSCALL_H
#define SYSCALL_H
#include "syscall.h"
#endif

cpu_context cpu_ctx;
PROC_stats processor_stat = {0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						 	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						 	     0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0};

IF_ID_buffer if_id = {0, 0, 0};
ID_EX_buffer id_ex = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
EX_MEM_buffer ex_mem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
MEM_WB_buffer mem_wb = {0, 0, 0, 0, 0, 0, 0};
TEMP_buffer temp_buffer = {0, 0, 0};

FLUSH_MUX flush_mux;

bool pipelined = false;

#define CONTROL
#define DEBUG
#undef DEBUG
#undef CONTROL
int fetch( IF_ID_buffer *out )
{
	// Set the inputs of flush mux
	flush_mux.zero = 0;
	flush_mux.instruction = instruction_memory[cpu_ctx.pc_register.PC];

	// Set the output of flush mux
	if (flush_mux.IF_flush)
		flush_mux.output = flush_mux.zero;
	else
		flush_mux.output = flush_mux.instruction;
	
#if defined(DEBUG)
	printf("\t\tFETCH\n");
	printf("PC = %d\n", cpu_ctx.pc_register.PC);
#endif
	if (out->IF_ID_write)
		out->instruction = flush_mux.output;
#if defined(DEBUG)
	printf("fetched instruction = %X\n", flush_mux.output);
#endif
	if (cpu_ctx.pc_register.pc_write)
		cpu_ctx.pc_register.PC += 1;				// Increment the pc (PC + 4)
	if (out->IF_ID_write)	
		out->next_pc = cpu_ctx.pc_register.PC;				
#if defined(DEBUG)
	printf("Next PC = %d\n", cpu_ctx.pc_register.PC);
#endif
	/*
	// Update the cycle count
	processor_stat.cycle += 1;
	*/
	
#if defined (DEBUG)
	printf("____________________________________\n");
#endif

	return 0;
}

int decode( IF_ID_buffer *in, ID_EX_buffer *out )
{
	REG_DST_MUX reg_dst_mux;
	REG_FILE reg_file;
	SIGN_EXTEND sign_extend;
	CONTROL_UNIT control;
	INSTRUCTION instruction;
    BRANCH_ADD branch_add;
    AND_GATE and_gate;
	PC_SRC_MUX pc_src_mux;
	JUMP_MUX j_mux;
	COMPARATOR comparator;
	FORWARD_C_MUX forwardC_mux;
	FORWARD_D_MUX forwardD_mux;
	ID_FORWARD_UNIT forward_unit;
	HAZARD_DETECTION_UNIT hazard_unit;
	NOOP_MUX noop_mux;

	// Parse the 32-bit instruction
	parse_instruction(in, &instruction);
	
	// Determine the control signals
	control_unit(&instruction, &control);

	// Set the reg dst must
	reg_dst(&instruction, &control, &reg_dst_mux);

	// Get the outputs from the register file
	register_file(&instruction, &reg_dst_mux, &reg_file);

	// Sign extend the immediate value
	sign_extender(&instruction, &sign_extend);

	// Calculate the branch adress
	branch_adder(in, &sign_extend, &branch_add);

	// Set the ID Forward Unit 
	id_forward_unit(&forward_unit, &control, &instruction);

	// Set the Forward C MUX
	set_forwardC_mux(&forward_unit, &forwardC_mux, &reg_file);

#if defined (DEBUG)
	if (control.opcode == 0x00 && control.funct ==0x08)
		printf("Value read from rs = %X\n", forwardC_mux.output);
#endif
	// Set the Forward D MUX
	set_forwardD_mux(&forward_unit, &forwardD_mux, &reg_file);

	// Do the comparison
	set_comparator(&comparator, &forwardC_mux, &forwardD_mux, &control);

	// Calculate the jump address
	uint32_t jump_address = calc_jump_address(in, &instruction, &control, &forwardC_mux);
#if defined (DEBUG)
	if (control.jump)
		printf("The calculated jump address = %d\n", jump_address);

#endif

#if defined(DEBUG)
	printf("\t\tDECODE\n");
	printf("Comparator input 1 = %d\n", comparator.input1);
	printf("Comparator input 2 = %d\n", comparator.input2);
	printf("Comparator output = %d\n", comparator.output);
#endif

	// Get the pc src signal
	set_and(&and_gate, &control, &comparator);

#if defined(DEBUG)
	printf("AND Gate output = %d\n", and_gate.pc_src);
#endif

	// Set the pc src mux

	set_pc_src_mux(in, &and_gate, &branch_add, &pc_src_mux);

#if defined(DEBUG)
	printf("PC_SRC Mux output = %d\n", pc_src_mux.output);
#endif

	// Set the jump mux
	set_jump_mux(&control, &pc_src_mux, jump_address, &j_mux);

#if defined(DEBUG)
	printf("JUMP Mux output = %d\n", j_mux.output);
#endif

	// Set the next address of the PC the output of Jump MUX
	// cpu_ctx.pc_register.PC = j_mux.output;

	// Detect Hazards
	detect_hazards(&hazard_unit, &control, &and_gate, &instruction, &j_mux);

#if defined(DEBUG)
	printf("Checked for Hazards\n");
#endif

	// Set Hazard Signals
	set_signals(&hazard_unit, &noop_mux);

#if defined(DEBUG)
	printf("Hazard signals set\n");
#endif

	// Set noop MUX
	set_noop_mux(&noop_mux, &control, &reg_file);

#if defined(DEBUG)
	printf("Nop mux set\n");
#endif

	// Set ID_EX_buffer
	id_ex_buffer(in, out, &control, &reg_file, &sign_extend, &instruction);



#if defined(DEBUG)
	printf("Opcode = %X\n", out->opcode);
	printf("Shamt = %d\n", out->shamt);
	printf("Funct = %X\n", out->funct);
	printf("mem_read = %d\n", out->mem_read);
	printf("mem_write = %d\n", out->mem_write);
	printf("mem_to_reg = %d\n", out->mem_to_reg);
	printf("alu_src = %d\n", out->alu_src);
	printf("reg_write = %d\n", out->reg_write);
	printf("write_reg = %d\n", out->write_reg);
	printf("read_data1 = %d\n", out->read_data1);
	printf("read_data2 = %d\n", out->read_data2);
	printf("ext_immediate = %d\n", out->ext_immediate);
#endif

#if defined (DEBUG)
	printf("____________________________________\n");
#endif
	return 0;
}

int execute( ID_EX_buffer *in, EX_MEM_buffer *out )
{
    ALU alu;
    ALU_SRC_MUX alu_src_mux;
	FORWARD_A_MUX forwardA_mux;
	FORWARD_B_MUX forwardB_mux;
	EX_FORWARD_UNIT forward_unit;
	
	// Determine the Forward A and Forward B signals
	ex_forward_unit(&forward_unit);

	// Set the Forward A Mux
	set_forwardA_mux(&forward_unit, &forwardA_mux);

	// Set the Forward B Mux
	set_forwardB_mux(&forward_unit, &forwardB_mux);

	// Set the alu src mux
	alu_src(&forwardB_mux, &alu_src_mux);

	// Perform the alu operation
	alu_operation(in, &alu, &alu_src_mux, &forwardA_mux, out);
#if defined (DEBUG)
	printf("ALU result in execute stage %X\n", alu.alu_result);
#endif
    // Set EX_MEM_buffer
    ex_mem_buffer(in, &forwardB_mux, &alu, out);

	// Update the cycle count
	processor_stat.cycle += 1;

#if defined (DEBUG)
	printf("____________________________________\n");
#endif

    return 0;
}


int memory( EX_MEM_buffer *in, MEM_WB_buffer *out )
{
	DATA_MEM data_mem;
	FORWARD_E_MUX forwardE_mux;
	MEM_FORWARD_UNIT forward_unit;

	// Check if instruction is syscall
	if(in->syscall)
		syscall(cpu_ctx.GPR[2]);
#if defined (DEBUG)
	printf("\t\tMEMORY\n");
	printf("Checked for syscall\n");
#endif
	
	// Set the forward E signal
	mem_forward_unit(in, out, &forward_unit);
#if defined (DEBUG)
	printf("Set mem forwarding unit\n");
#endif

	// Set the forward E MUX
	set_fowardE_mux(in, out, &forward_unit, &forwardE_mux);
#if defined (DEBUG)
	printf("Set forward E Mux\n");
#endif

	// Read or write to data/stack memory 
	set_data_memory(in, &forwardE_mux, &data_mem);
#if defined (DEBUG)
	printf("Accessed data memory\n");
#endif

	// Set MEM_WB buffer
	mem_wb_buffer(in, out, &data_mem);

#if defined (DEBUG)
	printf("____________________________________\n");
#endif

	return 0;
}

int writeback( MEM_WB_buffer *in)
{
#if defined (DEBUG)
	printf("____________________________________________________________________\n\n");
	printf("\t\tWRITEBACK\n");
#endif
    MEMTOREG_MUX mem_to_reg_mux;
/*
	// Check for syscall 
	if(in->syscall)
		syscall(cpu_ctx.GPR[2]);
*/
	// Set the mem to reg mux
	set_mem_to_reg(in, &mem_to_reg_mux);

    // Write to the register file if reg wrtie is asserted
    if (in->reg_write)
        cpu_ctx.GPR[in->write_reg] = mem_to_reg_mux.output;

	// Set the temp buffer
	set_temp_buffer(in, &mem_to_reg_mux, &temp_buffer);
	
#if defined (DEBUG)
	printf("alu result in write back for mux = %X\n", mem_to_reg_mux.alu_result);
	printf("read data for mux = %X\n", mem_to_reg_mux.read_data);
	printf("mem_to_reg mux output = %X\n", mem_to_reg_mux.output);
	printf("$zero or $0 = %d\n", cpu_ctx.GPR[0]);
	printf("$at or $1 = %X\n", cpu_ctx.GPR[1]);
	printf("Data to be written: %d\n", mem_to_reg_mux.output);
	printf("____________________________________\n");
	//printf("____________________________________________________________________\n\n");
#endif

	return 0;
}
