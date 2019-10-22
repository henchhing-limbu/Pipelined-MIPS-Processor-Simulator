#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

typedef struct {
	uint32_t read_data2;
	uint32_t ext_immediate;
	uint32_t alu_input2;            // output of this MUX
} ALU_SRC_MUX;

typedef struct{
    uint32_t input1;
    uint32_t input2;
    uint32_t alu_result;
    uint8_t zero;
} ALU;

typedef struct {
    uint32_t IF_ID_read_data1;
    uint32_t MEM_WB_write_data;
    int32_t EX_MEM_alu_result;
    uint8_t forward_A;
    uint32_t output;
} FORWARD_A_MUX;

typedef struct {
    uint32_t IF_ID_read_data2;
    uint32_t MEM_WB_write_data;
    int32_t EX_MEM_alu_result;             // From EX_MEM_buffer
    uint8_t forward_B;
    uint32_t output;
} FORWARD_B_MUX;

typedef struct {
    uint32_t EX_MEM_write_reg;
    uint32_t MEM_WB_write_reg;
    uint32_t ID_EX_rs;
    uint32_t ID_EX_rt;
    uint8_t forward_A;
    uint8_t forward_B;
}EX_FORWARD_UNIT;

int alu_src(FORWARD_B_MUX *forwardB_mux, ALU_SRC_MUX *alu_src_mux);
int alu_operation(ID_EX_buffer *in, ALU *alu, ALU_SRC_MUX *alu_src_mux, FORWARD_A_MUX *forwardA_mux, EX_MEM_buffer *out);
int ex_mem_buffer(ID_EX_buffer* in, FORWARD_B_MUX *forwardB_mux, ALU *alu, EX_MEM_buffer *out);
int set_forwardA_mux(EX_FORWARD_UNIT *forward_unit, FORWARD_A_MUX *forwardA_mux);
int set_forwardB_mux(EX_FORWARD_UNIT *forward_unit, FORWARD_B_MUX *forwardB_mux);
int ex_forward_unit(EX_FORWARD_UNIT *forward_unit) ;
