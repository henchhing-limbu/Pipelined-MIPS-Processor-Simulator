#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

typedef struct{
	uint32_t data_address;
	uint32_t write_data;
	uint32_t read_data;
} DATA_MEM;

typedef struct{
	uint32_t EX_MEM_write_data;
	uint32_t MEM_WB_read_data;
	uint8_t forward_E;
	uint32_t output;
} FORWARD_E_MUX;

typedef struct{
	uint8_t EX_MEM_rt;
	uint8_t EX_MEM_mem_write;
	uint8_t MEM_WB_mem_read;
	uint8_t MEM_WB_write_reg;
	uint8_t forward_E;
} MEM_FORWARD_UNIT;

uint32_t calculate_mem_address(EX_MEM_buffer* in);
int set_data_memory(EX_MEM_buffer* in, FORWARD_E_MUX *forwardE_mux, DATA_MEM* data_mem);
int mem_wb_buffer(EX_MEM_buffer *in, MEM_WB_buffer *out, DATA_MEM* data_mem);
int set_fowardE_mux(EX_MEM_buffer *in, MEM_WB_buffer *out, MEM_FORWARD_UNIT *forward_unit, FORWARD_E_MUX *forwardE_mux);
int mem_forward_unit(EX_MEM_buffer *in, MEM_WB_buffer *out, MEM_FORWARD_UNIT *forward_unit);

