/**
 * Gedare Bloom
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * cpu.h
 *
 * Definitions for the processor.
 */
#ifndef STDBOOL_H
#define STDBOOL_H
#include <stdbool.h>
#endif
 
#ifndef STDINT_H
#define STDINT_H 
#include <stdint.h>
#endif

extern bool pipelined;

typedef struct {
	uint32_t PC;
	uint32_t pc_write;
} PC_REGISTER;

typedef struct {
        PC_REGISTER pc_register;
        uint32_t GPR[32];
} cpu_context;

extern cpu_context cpu_ctx;

typedef struct {
	uint32_t zero;
	uint32_t instruction;
	uint8_t IF_flush;
	uint32_t output;
} FLUSH_MUX;

extern FLUSH_MUX flush_mux;

typedef struct {
    uint16_t add;
    uint16_t addu;
    uint16_t addi;
    uint16_t and_ctr;
    uint16_t andi;
    uint16_t beq;
    uint16_t bne;
    uint16_t j;
    uint16_t jal;
    uint16_t jr;
    uint16_t lui;
    uint16_t lw;
    uint16_t nor;
    uint16_t or_ctr;
    uint16_t ori;
    uint16_t slt;
    uint16_t sll;
    uint16_t srl;
    uint16_t slti;
    uint16_t sub;
    uint16_t sw;
    uint16_t sra;
    uint16_t xor_ctr;
    uint16_t xori;
    uint16_t syscall;
    uint16_t unimp_r_type;
    uint16_t unimp_i_type;
    uint16_t nop;
} INSTR_ctr;

typedef struct {
    uint16_t cycle;
    INSTR_ctr instruction;
	uint16_t flush;
	uint16_t stall;
	uint32_t useful_instructions;
	float cpi;
} PROC_stats;

extern PROC_stats processor_stat;

typedef struct {
	uint32_t instruction;
	uint32_t next_pc;
	uint32_t IF_ID_write;
} IF_ID_buffer;

typedef struct{
	uint32_t next_pc;
	uint8_t opcode;			// Execute
	uint8_t shamt;			// Execute
	uint8_t funct;			// Execute
	uint8_t mem_read;
	uint8_t mem_write;
	uint8_t mem_to_reg;
	uint8_t alu_src;		// Execute
	uint8_t reg_write;
	uint8_t write_reg;
	uint32_t read_data1;	// Execute
	uint32_t read_data2;	// Execute
	uint32_t ext_immediate;	// Execute
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
} ID_EX_buffer;

typedef struct{
    uint8_t mem_read;
    uint8_t mem_write;
    uint8_t mem_to_reg;
    uint8_t reg_write;
    uint8_t write_reg;
    uint32_t write_data;
    uint32_t alu_result;
	uint8_t syscall;
	uint8_t rt;
    // Note PC+4 can be accessed by cpu_ctx.PC
} EX_MEM_buffer;

typedef struct{
	uint32_t read_data;
	uint32_t alu_result;
	uint8_t mem_to_reg;
	uint8_t reg_write;
    uint8_t write_reg;
	uint8_t syscall;
	uint8_t mem_read;
} MEM_WB_buffer;

// Need a Placeholder buffer for forwarding because the stages are executed backwards
typedef struct {
	uint8_t reg_write;
	uint8_t write_reg;
	uint32_t write_data;
} TEMP_buffer;

extern IF_ID_buffer if_id;
extern ID_EX_buffer id_ex;
extern EX_MEM_buffer ex_mem;
extern MEM_WB_buffer mem_wb;
extern TEMP_buffer temp_buffer;

int fetch( IF_ID_buffer *out );
int decode( IF_ID_buffer *in, ID_EX_buffer *out );
int execute( ID_EX_buffer *in, EX_MEM_buffer *out );
int memory( EX_MEM_buffer *in, MEM_WB_buffer *out );
int writeback( MEM_WB_buffer *in );
