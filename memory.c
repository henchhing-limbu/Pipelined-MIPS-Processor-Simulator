#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif
#ifndef MEMORY_H
#define MEMORY_H
#include "memory.h"
#endif

#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "memory_structures.h"
#endif

uint32_t calculate_mem_address(EX_MEM_buffer* in) {
	// Returns the read/write address of data memory or stack memory
	// Accessing data memory
	if ((in->alu_result >= 0x10010000) && (in->alu_result < 0x1003FFFF)) 			
		return in->alu_result - 0x10010000;
	// Accessing stack memory
	else if ((in->alu_result > 0x10010000) && (in->alu_result <= 0x7FFFF61C))		
		return  8192 - (0x7FFFF61C - in->alu_result);
	// Error
	return -1;		
} 

int set_data_memory(EX_MEM_buffer* in, FORWARD_E_MUX *forwardE_mux, DATA_MEM* data_mem){
	data_mem->data_address = calculate_mem_address(in);
#define DEBUG
#undef DEBUG
#if defined (DEBUG)
	if (in->mem_read)
		printf("Load Address = %d\n", data_mem->data_address);
	else if (in->mem_write)
		printf("Store Address = %d\n", data_mem->data_address);
#endif
	data_mem->write_data = forwardE_mux->output;
	
	uint32_t temp;
	int i;

	// Writing to data/stack memory (x86 -> MIPS i.e Little Endian to Big Endian)
	if (in->mem_write) {
		for (i = 0; i < 4; i++) {
			temp = data_mem->write_data << (8 * i);
			data_memory[data_mem->data_address + (3 - i)] = temp >> 24;
		}
	}

	// Reading from data/stack memory (MIPS -> x86 i.e Big Endian to Little Endian)
	if (in->mem_read) {
		temp = 0;
		for ( i = 3; i >= 0; --i ) {
			temp = temp << 8;
			temp = temp | data_memory[data_mem->data_address + i];
		}
		data_mem->read_data = temp;
	}

	return 0;
}

int mem_wb_buffer(EX_MEM_buffer *in, MEM_WB_buffer *out, DATA_MEM* data_mem) {

	// Passing data to MEM/WB Buffer
	out->alu_result = in->alu_result;
	out->mem_to_reg = in->mem_to_reg;
	out->reg_write = in->reg_write;
	out->write_reg = in->write_reg;
	out->read_data = data_mem->read_data;
	out->syscall = in->syscall;
	out->mem_read = in->mem_read;

	return 0;
}

int set_fowardE_mux(EX_MEM_buffer *in, MEM_WB_buffer *out, MEM_FORWARD_UNIT *forward_unit, FORWARD_E_MUX *forwardE_mux){
	// Set the inputs of the forward E MUX
	forwardE_mux->EX_MEM_write_data = in->write_data;
	forwardE_mux->MEM_WB_read_data = out->read_data;
	forwardE_mux->forward_E = forward_unit->forward_E;
	
	// Set the output of the forward E MUX
	if (forwardE_mux->forward_E)
		forwardE_mux->output = forwardE_mux->MEM_WB_read_data;
	else
		forwardE_mux->output = forwardE_mux->EX_MEM_write_data;
	
	return 0;
}

int mem_forward_unit(EX_MEM_buffer *in, MEM_WB_buffer *out, MEM_FORWARD_UNIT *forward_unit){
	if ((out->write_reg != 0) && (in->mem_write) && (out->mem_read) && (in->rt == out->write_reg))
		forward_unit->forward_E = 1;
	else
		forward_unit->forward_E = 0;
	return 0;
}
