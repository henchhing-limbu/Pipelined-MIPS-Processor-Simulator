#ifndef DECODE_H
#define DEOCDE_H
#include "decode.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

// Decode Stage Helper Functions
int parse_instruction(IF_ID_buffer *in, INSTRUCTION *instruction){
	instruction->opcode = (in->instruction >> 26);		// Opcode
    	uint32_t temp = (in->instruction) << 6;
    	instruction->rs = temp >> 27;				// rs
    	temp = (in->instruction) << 11;
    	instruction->rt = temp >> 27;				// rt
    	temp = (in->instruction) << 16;
    	instruction->rd = temp >> 27;				// rd
    	temp = (in->instruction) << 16;
    	instruction->immediate = temp >> 16;			// immediate
    	temp = (in->instruction) << 21;
    	instruction->shamt = temp >> 27;			// shamt
    	temp = (in->instruction) << 26;
    	instruction->funct = temp >> 26;			// funct
	temp = (in->instruction) << 6;
	instruction->jump_28lower = temp >> 6;			// lower 28 bits of the jump address
	// No need to shift left 2, because our memory is instruction addressable not byte addressable
	return 0;
}

uint32_t calc_jump_address(IF_ID_buffer *in, INSTRUCTION *instruction, CONTROL_UNIT* control, FORWARD_C_MUX* forwardC_mux){
	if ((control->opcode == 0x00) && (control->funct == 0x08))	// jr
		return forwardC_mux->output;

	// Returns the calculated jump address
	uint32_t pc_high_4 = in->next_pc >> 28;
	pc_high_4 = pc_high_4 << 28;					// upper 4 bits of the jump address
	uint32_t jump_address = pc_high_4 | instruction->jump_28lower;	// jump_address
	jump_address = jump_address - (0x00400000 / 4);		
	
	return jump_address;
}

int control_unit(INSTRUCTION *instruction, CONTROL_UNIT *control){
	control->opcode = instruction->opcode;
	control->shamt = instruction->shamt;
	control->funct = instruction->funct;
	
	control->mem_read = 0;
	control->mem_to_reg = 0;
	control->mem_write = 0;
	control->alu_src = 0;
	control->reg_write = 0;
	control->reg_dst = 0;
	control->branch = 0;
	control->jump = 0;

	switch (control->opcode) {		
		case 0x00:		// R-Format
			control->reg_dst = 1;		
			// if not nop, jr, or syscall, only then assert reg_write 
			if (!((control->funct == 0 && instruction->rd == 0) 
				|| control->funct == 0x08 || control->funct == 0x0C))																															
				control->reg_write = 1;																						
			if (control->funct == 0x08)		// jr
				control->jump = 1;
			break;
		case 0x23:		// lw
			control->alu_src = 1;
			control->mem_to_reg = 1;
			control->reg_write = 1;
			control->mem_read = 1;
			break;

		case 0x2B:		// sw
			control->alu_src = 1;
			control->mem_write = 1;
			break;

		case 0x04:		// beq
		case 0x05:		// bne
			control->branch = 1;
			break;

		case 0x02:		// j
			control->jump = 1;
			break;
		case 0x03:		// jal
			control->jump = 1;
			cpu_ctx.GPR[31] = if_id.next_pc;
			break;

		case 0x08:		// addi
		case 0x0C:		// andi
		case 0x0F:		// lui
		case 0x0D:		// ori
		case 0x0A:		// slti
		case 0x0E:		// xori
			control->alu_src = 1;
			control->reg_write = 1;
			break;
		default:
			break;
	}
	return 0;
}

int reg_dst(INSTRUCTION *instruction, CONTROL_UNIT *control, REG_DST_MUX *reg_dst_mux){
	// Set the input of mux
	reg_dst_mux->rt = instruction->rt;
	reg_dst_mux->rd = instruction->rd;
	if (control->reg_dst)
		reg_dst_mux->write_reg = reg_dst_mux->rd;
	else
		reg_dst_mux->write_reg = reg_dst_mux->rt;
	return 0;
}

int register_file(INSTRUCTION *instruction, REG_DST_MUX *reg_dst_mux, REG_FILE *reg_file){
	reg_file->read_reg1 = instruction->rs;
	reg_file->read_reg2 = instruction->rt;
	
	// Set the write reg input of the register file
	reg_file->write_reg = reg_dst_mux->write_reg;

	// Set the outputs of the register file
	reg_file->read_data1 = cpu_ctx.GPR[reg_file->read_reg1];
	reg_file->read_data2 = cpu_ctx.GPR[reg_file->read_reg2];

	return 0;
}

int sign_extender(INSTRUCTION *instruction, SIGN_EXTEND *sign_extend){
	// Set the outputs of sign extend
	uint8_t sign = (instruction->immediate) >> 15;		// The sign of the immediate
	sign_extend->ext_immediate = instruction->immediate;
	if (sign) {
		sign_extend->ext_immediate = sign_extend->ext_immediate | 0xFFFF0000;
	}
	return 0;
}

int id_ex_buffer(IF_ID_buffer *in, ID_EX_buffer *out, CONTROL_UNIT *control, REG_FILE *reg_file, SIGN_EXTEND *sign_extend, INSTRUCTION *instruction){
	out->next_pc = in->next_pc;
	out->opcode = control->opcode;
	out->shamt = control->shamt;
	out->funct = control->funct;
	out->mem_read = control->mem_read;
	out->mem_write = control->mem_write;
	out->mem_to_reg = control->mem_to_reg;
	out->alu_src = control->alu_src;
	out->reg_write = control->reg_write;;
	out->write_reg = reg_file->write_reg;
	out->read_data1 = reg_file->read_data1;
	out->read_data2 = reg_file->read_data2;
	out->ext_immediate = sign_extend->ext_immediate;
	out->rs = instruction->rs;
	out->rt = instruction->rt;
	out->rd = instruction->rd;

	return 0;
}

int branch_adder(IF_ID_buffer *in, SIGN_EXTEND *sign_extend, BRANCH_ADD *branch_add) {
	// set branch adder input
    branch_add->ext_immediate = sign_extend->ext_immediate;
    branch_add->pc_plus_4 = in->next_pc;
	
	// calculate the branch address
    branch_add->branch_address = branch_add->ext_immediate + branch_add->pc_plus_4 - 1;	// TODO: In QtSpim branch is calculated using PC not PC + 4, hence -1 fixes the problem

	return 0;
}

int set_and(AND_GATE* and_gate, CONTROL_UNIT* control, COMPARATOR* comparator) {
    // set AND gate input
    and_gate->branch = control->branch;
	and_gate->zero = comparator->output;

    // get the output of the and gate (pc_src)
    and_gate->pc_src = and_gate->branch & and_gate->zero;
	
	return 0;
}

int set_pc_src_mux(IF_ID_buffer* in, AND_GATE* and_gate, BRANCH_ADD *branch_add, PC_SRC_MUX* pc_src_mux) {
	// Set the inputs of PC MUX
	pc_src_mux->pc_src = and_gate->pc_src;
	pc_src_mux->PC4_address = in->next_pc;
	pc_src_mux->branch_address = branch_add->branch_address;

	// Set the output of PC MUX
	if (pc_src_mux->pc_src)
		pc_src_mux->output = pc_src_mux->branch_address;
	else
		pc_src_mux->output = pc_src_mux->PC4_address;

	return 0;
}

int set_jump_mux(CONTROL_UNIT* control, PC_SRC_MUX* pc_src_mux, uint32_t jump_address, JUMP_MUX* j_mux) {

	// Set the inputs of jump MUX
	j_mux->pc_src_mux_address = pc_src_mux->output;
	j_mux->jump_address = jump_address;
	j_mux->jump = control->jump;

	// Set the output of jump MUX
	if (j_mux->jump)
		j_mux->output = j_mux->jump_address;
	else
		j_mux->output = j_mux->pc_src_mux_address;

	return 0;
}

int id_forward_unit(ID_FORWARD_UNIT* forward_unit, CONTROL_UNIT* control, INSTRUCTION* instruction) {
	// Set the inputs of the foward unit
	forward_unit->branch = control->branch;
	forward_unit->jr = (control->opcode == 0x00) && (control->funct == 0x08);
	forward_unit->EX_MEM_write_reg = mem_wb.write_reg;			/* 	Since the stages are executed backwards 
																	in the simulator, decode occurs after 
																	MEM stage, therefore what should have 
																	been in EX/MEM buffer would be in MEM/WB 
																	buffer when decode stage is executed.*/
	
	forward_unit->MEM_WB_write_reg = temp_buffer.write_reg;
	forward_unit->EX_MEM_reg_write = mem_wb.reg_write;
	forward_unit->MEM_WB_reg_write = temp_buffer.reg_write;	

	forward_unit->IF_ID_rs = instruction->rs;
	forward_unit->IF_ID_rt = instruction->rt;


	// Set forward C and forward D signals
	if ((forward_unit->EX_MEM_reg_write)
		&& ((forward_unit->branch) || (forward_unit->jr))&& (forward_unit->EX_MEM_write_reg != 0) 
		&& (forward_unit->EX_MEM_write_reg == forward_unit->IF_ID_rs))
		forward_unit->forward_C = 2;
	else if ((forward_unit->MEM_WB_reg_write)
		&& ((forward_unit->branch) || (forward_unit->jr)) && (forward_unit->MEM_WB_write_reg != 0)
		&& (forward_unit->EX_MEM_write_reg != forward_unit->IF_ID_rs) 
		&& (forward_unit->MEM_WB_write_reg == forward_unit->IF_ID_rs))
		forward_unit->forward_C = 1;
	else
		forward_unit->forward_C = 0;

	if ((forward_unit->EX_MEM_reg_write)
		&& (forward_unit->branch) && (forward_unit->EX_MEM_write_reg != 0) 
		&& (forward_unit->EX_MEM_write_reg == forward_unit->IF_ID_rt))
		forward_unit->forward_D = 2;
	else if ((forward_unit->MEM_WB_reg_write)
		&& (forward_unit->branch) && (forward_unit->MEM_WB_write_reg != 0)
		&& (forward_unit->EX_MEM_write_reg != forward_unit->IF_ID_rt) 
		&& (forward_unit->MEM_WB_write_reg == forward_unit->IF_ID_rt))
		forward_unit->forward_D = 1;
	else
		forward_unit->forward_D = 0;

	return 0;
}

int set_forwardC_mux(ID_FORWARD_UNIT *forward_unit, FORWARD_C_MUX *forwardC_mux, REG_FILE* reg_file) {
	// Set the inputs of Forward C MUX
	forwardC_mux->read_data1 = reg_file->read_data1;
	forwardC_mux->MEM_WB_write_data = temp_buffer.write_data;
	forwardC_mux->EX_MEM_alu_result = mem_wb.alu_result;				

	/* 	Since the stages are executed backwards in the simulator, decode occurs after MEM stage, 
		therefore what should have been in EX/MEM buffer would be in MEM/WB buffer 
		when decode stage is executed.	*/
	
	// Set the output of the Forward C MUX
	if (forward_unit->forward_C == 0)
		forwardC_mux->output = forwardC_mux->read_data1;
	else if (forward_unit->forward_C == 1) 
		forwardC_mux->output = forwardC_mux->MEM_WB_write_data;
	else if (forward_unit->forward_C == 2) 
		forwardC_mux->output = forwardC_mux->EX_MEM_alu_result;
	else
		printf("Invalid signal\n"); 

	return 0;
}

int set_forwardD_mux(ID_FORWARD_UNIT *forward_unit, FORWARD_D_MUX *forwardD_mux, REG_FILE* reg_file) {
	// Set the inputs of Forward D MUX
	forwardD_mux->read_data2 = reg_file->read_data2;
	forwardD_mux->MEM_WB_write_data = temp_buffer.write_data;
	forwardD_mux->EX_MEM_alu_result = mem_wb.alu_result;
	
	/* 	Since the stages are executed backwards in the simulator, decode occurs after MEM stage, 
		therefore what should have been in EX/MEM buffer would be in MEM/WB buffer 
		when decode stage is executed.	*/
	
	// Set the output of the Forward C MUX
	if (forward_unit->forward_D == 0)
		forwardD_mux->output = forwardD_mux->read_data2;
	else if (forward_unit->forward_D == 1) 
		forwardD_mux->output = forwardD_mux->MEM_WB_write_data;
	else if (forward_unit->forward_D == 2) 
		forwardD_mux->output = forwardD_mux->EX_MEM_alu_result;
	else
		printf("Invalid signal\n"); 

	return 0;
}

int set_comparator(COMPARATOR* comparator, FORWARD_C_MUX *forwardC_mux, FORWARD_D_MUX *forwardD_mux, CONTROL_UNIT* control) {
	// Set the inputs
	comparator->input1 = forwardC_mux->output;
	comparator->input2 = forwardD_mux->output;

	// Set the output of the comparator
	switch (control->opcode) {
		case 0x04:		// beq
			if (comparator->input1 == comparator->input2) 
				comparator->output = 1;
			else
				comparator->output = 0;
			break;
		case 0x05:		// bne
			if (comparator->input1 != comparator->input2) 
				comparator->output = 1;
			else
				comparator->output = 0;
			break;
	}
	
	return 0;
}

int detect_hazards(HAZARD_DETECTION_UNIT* hazard_unit, CONTROL_UNIT* control, AND_GATE* and_gate, INSTRUCTION* instruction, JUMP_MUX* j_mux) {
	// Set the inputs of the hazard detection unit
	hazard_unit->ID_branch = control->branch;
	hazard_unit->ID_jr = (control->opcode == 0x00) && (control->funct == 0x08);
	hazard_unit->pc_src = and_gate->pc_src;
	hazard_unit->ID_jump = control->jump;
	hazard_unit->ID_rs = instruction->rs;
	hazard_unit->ID_rt = instruction->rt;
	hazard_unit->ID_mem_write = control->mem_write;
	hazard_unit->ID_EX_mem_read = ex_mem.mem_read;
	hazard_unit->EX_MEM_mem_read = mem_wb.mem_read;
	hazard_unit->ID_EX_reg_write = ex_mem.reg_write;
	hazard_unit->ID_EX_write_reg = ex_mem.write_reg;
	hazard_unit->EX_MEM_reg_write = mem_wb.reg_write;
	hazard_unit->EX_MEM_write_reg = mem_wb.write_reg;

	// Set the signals to initially be no stalls and no flushes
	hazard_unit->pc_write = 1;
	hazard_unit->IF_ID_write = 1;
	hazard_unit->IF_flush = 0;
	hazard_unit->noop = 0;
	
	if (pipelined) {

	if (hazard_unit->ID_jump && !hazard_unit->ID_jr) {			// If the instruction in decode is a jump
		// Flush
		cpu_ctx.pc_register.PC = j_mux->output;
		// Update the flush count
		processor_stat.flush += 1;
	} else if (hazard_unit->ID_branch) {	// If the instruction in decode is a branch

		// First detect stalls 
		if ((hazard_unit->ID_EX_reg_write) && (hazard_unit->ID_EX_write_reg != 0) 
			&& ((hazard_unit->ID_rs == hazard_unit->ID_EX_write_reg) || 
			(hazard_unit->ID_rt == hazard_unit->ID_EX_write_reg))) {
			// Stall 
			hazard_unit->pc_write = 0;
			hazard_unit->IF_ID_write = 0;
			hazard_unit->noop = 1;
			// Update the stall count
			processor_stat.stall += 1;
		} else if ((hazard_unit->EX_MEM_mem_read) && (hazard_unit->EX_MEM_reg_write) 
			&& (hazard_unit->EX_MEM_write_reg != 0)
			&& ((hazard_unit->ID_rs == hazard_unit->EX_MEM_write_reg) || 
			(hazard_unit->ID_rt == hazard_unit->EX_MEM_write_reg))) {
			// Stall
			hazard_unit->pc_write = 0;
			hazard_unit->IF_ID_write = 0;
			hazard_unit->noop = 1;
			// Update the stall count
			processor_stat.stall += 1;
		} else { // If no stalls, check if branch is taken to know if a flush is needed
			if (hazard_unit->pc_src) {
				// Flush
				cpu_ctx.pc_register.PC = j_mux->output;
			
				// Update the flush count
				processor_stat.flush += 1;
			}
		}
	} else if (hazard_unit->ID_jr) {	// If the instruction is jr
			// Check if register rs is being written to and stall
			if ((hazard_unit->ID_EX_reg_write) && (hazard_unit->ID_EX_write_reg != 0) 
			&& (hazard_unit->ID_rs == hazard_unit->ID_EX_write_reg)) {
				// Stall 
				hazard_unit->pc_write = 0;
				hazard_unit->IF_ID_write = 0;
				hazard_unit->noop = 1;
				// Update the stall count
				processor_stat.stall += 1;
			} else if ((hazard_unit->EX_MEM_mem_read) && (hazard_unit->EX_MEM_reg_write) 
			&& (hazard_unit->EX_MEM_write_reg != 0)
			&& (hazard_unit->ID_rs == hazard_unit->EX_MEM_write_reg)) {
				// Stall 
				hazard_unit->pc_write = 0;
				hazard_unit->IF_ID_write = 0;
				hazard_unit->noop = 1;
				// Update the stall count
				processor_stat.stall += 1;
			} else {
				// Flush
				cpu_ctx.pc_register.PC = j_mux->output;
				// Update the flush count
				processor_stat.flush += 1;
			}

	} else {	// If the instruction in decode is not a branch or jump

		// // lw followed by sw
		if ((hazard_unit->ID_EX_mem_read) && (hazard_unit->ID_mem_write)) {
			//no stall
		// If it's a load use hazard and it's not a load followed by a store
		} else if ((hazard_unit->ID_EX_mem_read)
			&& (hazard_unit->ID_EX_reg_write) && (hazard_unit->ID_EX_write_reg != 0)
			&& ((hazard_unit->ID_rs == hazard_unit->ID_EX_write_reg) || 
			(hazard_unit->ID_rt == hazard_unit->ID_EX_write_reg))) {
			
			// Stall
			hazard_unit->pc_write = 0;
			hazard_unit->IF_ID_write = 0;
			hazard_unit->noop = 1;
			// Update the stall count
			processor_stat.stall += 1;
		}
	}	
	}
	else{
		hazard_unit->pc_write = 1;
		hazard_unit->IF_ID_write = 1;
		hazard_unit->IF_flush = 0;
		hazard_unit->noop = 0;
		if (hazard_unit->ID_jump || hazard_unit->pc_src) {
			// Flush
			cpu_ctx.pc_register.PC = j_mux->output;
		}
	}	
	return 0;
}

int set_signals(HAZARD_DETECTION_UNIT* hazard_unit, NOOP_MUX* noop_mux) {
	cpu_ctx.pc_register.pc_write = hazard_unit->pc_write;
	if_id.IF_ID_write = hazard_unit->IF_ID_write;
	flush_mux.IF_flush = hazard_unit->IF_flush;
	noop_mux->noop = hazard_unit->noop;

	return 0;
}

int set_noop_mux(NOOP_MUX* noop_mux, CONTROL_UNIT* control, REG_FILE* reg_file) {
	if (noop_mux->noop) {
		control->opcode = 0;
		control->shamt = 0;
		control->funct = 0;
		control->mem_read = 0;
		control->mem_to_reg = 0;
		control->mem_write = 0;
		control->alu_src = 0;
		control->reg_write = 0;
		control->reg_dst = 0;
		control->branch = 0;
		control->jump = 0;
		reg_file->write_reg = 0;
	}
}

