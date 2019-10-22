#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

typedef struct{
	uint8_t mem_to_reg;
	uint32_t read_data;
	uint32_t alu_result;
	uint32_t output;
} MEMTOREG_MUX;

int set_mem_to_reg(MEM_WB_buffer* in, MEMTOREG_MUX* mem_to_reg_mux);
int set_temp_buffer(MEM_WB_buffer* in, MEMTOREG_MUX* mem_to_reg_mux, TEMP_buffer* out);
