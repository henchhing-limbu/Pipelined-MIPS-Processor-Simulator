#ifndef CPU_H
#define CPU_H
#include "cpu.h"
#endif

typedef struct {
	uint8_t opcode;
	uint8_t shamt;
	uint8_t funct;
	uint8_t mem_read;
	uint8_t mem_to_reg;
	uint8_t mem_write;
	uint8_t alu_src;
	uint8_t reg_write;
	uint8_t reg_dst;
	uint8_t branch;
	uint8_t jump;
} CONTROL_UNIT;

typedef struct {
	uint8_t read_reg1;
	uint8_t read_reg2;
	uint8_t write_reg;
	uint8_t write_data;
	uint32_t read_data1;
	uint32_t read_data2;
} REG_FILE;

typedef struct {
	uint16_t immediate;
	uint32_t ext_immediate;
} SIGN_EXTEND;

typedef struct {
	uint8_t rt;
	uint8_t rd;
	uint8_t write_reg;
} REG_DST_MUX;


typedef struct {
	uint8_t opcode;
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
	uint8_t shamt;
	uint8_t funct;
	uint16_t immediate;
	uint32_t jump_28lower;
} INSTRUCTION;

typedef struct{
    uint32_t ext_immediate;
    uint32_t pc_plus_4;
	uint32_t branch_address;
} BRANCH_ADD;

typedef struct{
    uint8_t branch;
    uint8_t zero;
    uint8_t pc_src;
} AND_GATE;

typedef struct{
	uint32_t branch_address;
	uint32_t PC4_address;
	uint8_t pc_src;
	uint32_t output;
} PC_SRC_MUX;

typedef struct{
	uint32_t pc_src_mux_address;
	uint32_t jump_address;
	uint8_t jump;
	uint32_t output;
} JUMP_MUX;

typedef struct {
	uint32_t input1;
	uint32_t input2;
	uint8_t output;
} COMPARATOR;

typedef struct {
	uint32_t read_data1;
	uint32_t EX_MEM_alu_result;
	uint32_t MEM_WB_write_data;
	uint32_t output;
} FORWARD_C_MUX;

typedef struct {
	uint32_t read_data2;
	uint32_t EX_MEM_alu_result;
	uint32_t MEM_WB_write_data;
	uint32_t output;
} FORWARD_D_MUX;

typedef struct {
	uint8_t branch;
	uint8_t jr;
	uint8_t EX_MEM_write_reg;
	uint8_t MEM_WB_write_reg;
	uint8_t EX_MEM_reg_write;
	uint8_t MEM_WB_reg_write;
	uint8_t IF_ID_rs;
	uint8_t IF_ID_rt;
	uint8_t forward_C;
	uint8_t forward_D;
} ID_FORWARD_UNIT;

typedef struct {
	// For Flushing
	uint8_t ID_branch;
	uint8_t pc_src;			// branch taken signal
	uint8_t ID_jump;
	uint8_t ID_jr;

	// For Stalling
	uint8_t ID_rs;
	uint8_t ID_rt;
	uint8_t ID_mem_write;		// for detecting lw -> sw i.e no stalls needed
	uint8_t ID_EX_mem_read;
	uint8_t EX_MEM_mem_read;
	uint8_t ID_EX_reg_write;
	uint8_t ID_EX_write_reg;
	uint8_t EX_MEM_reg_write;
	uint8_t EX_MEM_write_reg;

	// Signals
	uint8_t pc_write;
	uint8_t IF_ID_write;
	uint8_t IF_flush;
	uint8_t noop;
	
} HAZARD_DETECTION_UNIT;


typedef struct {
	CONTROL_UNIT* control;
	uint8_t noop;
} NOOP_MUX;


int parse_instruction(IF_ID_buffer *in, INSTRUCTION *instruction);
uint32_t calc_jump_address(IF_ID_buffer *in, INSTRUCTION *instruction, CONTROL_UNIT* control, FORWARD_C_MUX* forwardC_mux);
int control_unit(INSTRUCTION *instruction, CONTROL_UNIT *control);
int reg_dst(INSTRUCTION *instruction, CONTROL_UNIT *control, REG_DST_MUX *reg_dst_mux);
int register_file(INSTRUCTION *instruction, REG_DST_MUX *reg_dst_mux, REG_FILE *reg_file);
int sign_extender(INSTRUCTION *instruction, SIGN_EXTEND *sign_extend);

int branch_adder(IF_ID_buffer *in, SIGN_EXTEND *sign_extend, BRANCH_ADD *branch_add);
int set_and(AND_GATE* and_gate, CONTROL_UNIT* control, COMPARATOR* comparator);

int set_pc_src_mux(IF_ID_buffer* in, AND_GATE* and_gate, BRANCH_ADD *branch_add, PC_SRC_MUX* pc_src_mux);
int set_jump_mux(CONTROL_UNIT* control, PC_SRC_MUX* pc_src_mux, uint32_t jump_address, JUMP_MUX* j_mux);

int id_forward_unit(ID_FORWARD_UNIT* forward_unit, CONTROL_UNIT* control, INSTRUCTION* instruction);
int set_forwardC_mux(ID_FORWARD_UNIT *forward_unit, FORWARD_C_MUX *forwardC_mux, REG_FILE* reg_file);
int set_forwardD_mux(ID_FORWARD_UNIT *forward_unit, FORWARD_D_MUX *forwardD_mux, REG_FILE* reg_file);
int set_comparator(COMPARATOR* comparator, FORWARD_C_MUX *forwardC_mux, FORWARD_D_MUX *forwardD_mux, CONTROL_UNIT* control);

int id_ex_buffer(IF_ID_buffer *in, ID_EX_buffer *out, CONTROL_UNIT *control, REG_FILE *reg_file, SIGN_EXTEND *sign_extend, INSTRUCTION *instruction);

// Hazard Detection
int detect_hazards(HAZARD_DETECTION_UNIT* hazard_unit, CONTROL_UNIT* control, AND_GATE* and_gate, INSTRUCTION* instruction, JUMP_MUX* j_mux);
int set_signals(HAZARD_DETECTION_UNIT* hazard_unit, NOOP_MUX* noop_mux);
int set_noop_mux(NOOP_MUX* noop_mux, CONTROL_UNIT* control, REG_FILE* reg_file);