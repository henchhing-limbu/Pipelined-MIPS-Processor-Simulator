#ifndef EXECUTE_H
#define EXECUTE_H
#include "execute.h"
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#define CONTROL
#define DEBUG
#undef DEBUG
#undef CONTROL

int alu_src(FORWARD_B_MUX *forwardB_mux, ALU_SRC_MUX *alu_src_mux){
	// set mux inputs
    alu_src_mux->read_data2 = forwardB_mux->output;
    alu_src_mux->ext_immediate = id_ex.ext_immediate;

    // calculate mux output
    if (id_ex.alu_src)
        alu_src_mux->alu_input2 = alu_src_mux->ext_immediate;
    else
        alu_src_mux->alu_input2 = alu_src_mux->read_data2;

	return 0;
}

int alu_operation(ID_EX_buffer *in, ALU *alu, ALU_SRC_MUX *alu_src_mux, FORWARD_A_MUX *forwardA_mux, EX_MEM_buffer *out){

    // set alu input
    alu->input1 = forwardA_mux->output;
    alu->input2 = alu_src_mux->alu_input2;



#if defined(CONTROL)
	printf("\t\tEXECUTE\n");
	printf("INSTRUCTION: ");
#endif
	out->syscall = 0;
    switch (in->opcode)
    {
        case 0x00:
        // R-format
           switch (in->funct)
            {
				case 0x21:
		    		// addu
					alu->alu_result = alu->input1 + alu->input2;
            		#if defined(CONTROL)
            			printf("addu\n");
            		#endif
            		processor_stat.instruction.addu += 1;
		    		break;
                case 0x20:
                    // add
                    alu->alu_result = alu->input1 + alu->input2;
				#if defined(CONTROL)
					printf("add\n");
				#endif
					processor_stat.instruction.add += 1;
                    break;
                case 0x24:
                    // and
                    alu->alu_result = alu->input1 & alu->input2;
				#if defined(CONTROL)
					printf("and\n");
				#endif
					processor_stat.instruction.and_ctr += 1;
                    break;
                case 0x08:
                    // jr
                    // jumps to the address in $rs or input1 of the ALU
                    // Handled in decode stage
				#if defined(CONTROL)
					printf("jr\n");
				#endif
                    processor_stat.instruction.jr += 1;
                    break;
                case 0x27:
                    // nor
                    alu->alu_result = ~(alu->input1 | alu->input2);
				#if defined(CONTROL)
					printf("nor\n");
				#endif
					processor_stat.instruction.nor += 1;
                    break;
                case 0x25:
                    // or
                    alu->alu_result = alu->input1 | alu->input2;
				#if defined(CONTROL)
					printf("or\n");
				#endif
					processor_stat.instruction.or_ctr += 1;
                    break;
                case 0x2A:
                    // slt
                    alu->alu_result = (alu->input1 < alu->input2) ? 1 : 0;
				#if defined(CONTROL)
					printf("slt\n");
				#endif
					processor_stat.instruction.slt += 1;
                    break;
                case 0x00:
					switch (in->write_reg) {
						case 0x00:
							// nop
						#if defined(CONTROL)
							printf("nop\n");
						#endif
						processor_stat.instruction.nop += 1;
							break;
						default:
                    		// sll
                    		alu->alu_result = alu->input2 << in->shamt;
						#if defined(CONTROL)
							printf("sll\n");
						#endif
						processor_stat.instruction.sll += 1;
                    		break;
					}
                    break;
                case 0x02:
                    // srl
                    alu->alu_result = alu->input2 >> in->shamt;
				#if defined(CONTROL)
					printf("srl\n");
				#endif
					processor_stat.instruction.srl += 1;
                    break;
                case 0x22:
                    // sub
                    alu->alu_result = alu->input1 - alu->input2;
				#if defined(CONTROL)
					printf("sub\n");
				#endif
					processor_stat.instruction.sub += 1;
                    break;
                case 0x03:
                    // sra
                    alu->alu_result = alu->input2 >> in->shamt;
                    if (alu->input2 >> 31){ 	// If negative
                        uint32_t temp = 0xFFFFFFFF;
                        temp = temp << (32 - in->shamt);
                        alu->alu_result = alu->alu_result | temp;
                    }
				#if defined(CONTROL)
					printf("sra\n");
				#endif
					processor_stat.instruction.sra += 1;
                    break;
                case 0x26:
                    // xor
                    alu->alu_result = alu->input1 ^ alu->input2;
				#if defined(CONTROL)
					printf("xor\n");
				#endif
					processor_stat.instruction.xor_ctr += 1;
                    break;
                case 0x0C:
                    // syscall
				#if defined(CONTROL)
					printf("syscall\n");
				#endif
					out->syscall = 1;
					processor_stat.instruction.syscall += 1;
                    break;
				default:
				#if defined(CONTROL)
					printf("Unimplemented R-type instruction\n");
				#endif
				    processor_stat.instruction.unimp_r_type += 1;
					break;
            }
		break;

        case 0x08:
            // addi
            alu->alu_result = alu->input1 + alu->input2;
		#if defined(CONTROL)
			printf("addi\n");
		#endif
			processor_stat.instruction.addi += 1;
            break;
        case 0x23:
            // lw
            alu->alu_result = alu->input1 + alu->input2;
		#if defined(CONTROL)
			printf("lw\n");
		#endif
			processor_stat.instruction.lw += 1;
            break;
        case 0x2B:
            // sw
            alu->alu_result = alu->input1 + alu->input2;
		#if defined(CONTROL)
			printf("sw\n");
		#endif
			processor_stat.instruction.sw += 1;
            break;
        case 0x0C:
            // andi
            alu->alu_result = alu->input1 & alu->input2;
		#if defined(CONTROL)
			printf("andi\n");
		#endif
			processor_stat.instruction.andi += 1;
            break;
        case 0x04:
            // beq
            alu->alu_result = alu->input1 - alu->input2;
            if (alu->alu_result == 0)
                alu->zero = 1;
            else
                alu->zero = 0;
		#if defined(CONTROL)
			printf("beq\n");
		#endif
			processor_stat.instruction.beq += 1;
			break;
        case 0x05:
            // bne
            alu->alu_result = alu->input1 - alu->input2;
            if (alu->alu_result == 0)
                alu->zero = 0;
            else
                alu->zero = 1;
		#if defined(CONTROL)
			printf("bne\n");
		#endif
			processor_stat.instruction.bne += 1;
            break;
        case 0x02:
            // j
            // do nothing (already calculated in decode stage)
            // mem stage will deal with it
			processor_stat.instruction.j += 1;
		#if defined(CONTROL)
			printf("j\n");
		#endif
            break;
        case 0x03:
            // jal
            // storing PC + 4 in $ra (which is register $31)`
            // Handled in decode
		#if defined(CONTROL)
			printf("jal\n");
		#endif
			processor_stat.instruction.jal += 1;
            break;
        case 0x0F:
            // lui
			// input2 = 0x1001
            alu->alu_result = alu->input2 << 16;// TODO: Change lui implementation to handle lw
		#if defined(CONTROL)
			printf("lui\n");
		#endif
			processor_stat.instruction.lui += 1;
            break;
        case 0x0D:
            // ori
            alu->alu_result = alu->input1 | alu->input2;
		#if defined(CONTROL)
			printf("ori\n");
		#endif
			processor_stat.instruction.ori += 1;
            break;
        case 0x0A:
            //slti
            alu->alu_result = (alu->input1 < alu->input2) ? 1 : 0;
		#if defined(CONTROL)
			printf("slti\n");
		#endif
			processor_stat.instruction.slti += 1;
        case 0x0E:
            // xori
            alu->alu_result = alu->input1 ^ alu->input2;
		#if defined(CONTROL)
			printf("xori\n");
		#endif
			processor_stat.instruction.xori += 1;
            break;
		default:
		#if defined(CONTROL)
			printf("Unimplemented I Type Instruction\n");
		#endif
			processor_stat.instruction.unimp_i_type += 1;
            break;
    }
#if defined (DEBUG)
	printf("ALU input 1 = %X\n", alu->input1);
	printf("ALU input 2 = %X\n", alu->input2);
#endif
	return 0;
}

int ex_mem_buffer(ID_EX_buffer* in, FORWARD_B_MUX *forwardB_mux, ALU *alu, EX_MEM_buffer *out)
{
    out->mem_read = in->mem_read;
    out->mem_write = in->mem_write;
    out->mem_to_reg = in->mem_to_reg;
    out->reg_write = in->reg_write;
    out->write_reg = in->write_reg;
    out->write_data = forwardB_mux->output;
    out->alu_result = alu->alu_result;
	out->rt = in->rt;

	return 0;
}

int set_forwardA_mux(EX_FORWARD_UNIT *forward_unit, FORWARD_A_MUX *forwardA_mux){
    // set mux inputs
    forwardA_mux->IF_ID_read_data1 = id_ex.read_data1;
    forwardA_mux->MEM_WB_write_data = temp_buffer.write_data;
    forwardA_mux->EX_MEM_alu_result = mem_wb.alu_result;

    // calculate mux output
    if (forward_unit->forward_A == 0)
        forwardA_mux->output = forwardA_mux->IF_ID_read_data1;
    else if  (forward_unit->forward_A == 1)
        forwardA_mux->output = forwardA_mux->MEM_WB_write_data;
    else if (forward_unit->forward_A == 2)
        forwardA_mux->output = forwardA_mux->EX_MEM_alu_result;
	else
		printf("Invalid signal\n");

    return 0;
}

int set_forwardB_mux(EX_FORWARD_UNIT *forward_unit, FORWARD_B_MUX *forwardB_mux) {
    // set mux inputs
    forwardB_mux->IF_ID_read_data2 = id_ex.read_data2;
	forwardB_mux->MEM_WB_write_data = temp_buffer.write_data;
    forwardB_mux->EX_MEM_alu_result = mem_wb.alu_result;
	
    // calculate mux output
    if (forward_unit->forward_B == 0)
        forwardB_mux->output = forwardB_mux->IF_ID_read_data2;
    else if (forward_unit->forward_B == 1)
        forwardB_mux->output = forwardB_mux->MEM_WB_write_data;
    else if (forward_unit->forward_B == 2)
        forwardB_mux->output = forwardB_mux->EX_MEM_alu_result;
	else
		printf("Invalid signal\n");

    return 0;
}

int ex_forward_unit(EX_FORWARD_UNIT *forward_unit) {
    // EX forward unit
	if ((mem_wb.reg_write) && (mem_wb.write_reg != 0)
		&& (mem_wb.write_reg == id_ex.rs))		
		forward_unit->forward_A = 2;
	else if ((temp_buffer.reg_write) && (temp_buffer.write_reg != 0)
		&& (mem_wb.write_reg != id_ex.rs)
		&& (temp_buffer.write_reg == id_ex.rs))
		forward_unit->forward_A = 1;
	else
		forward_unit->forward_A = 0;
		
	// MEM forward unit		
	if ((mem_wb.reg_write) && (mem_wb.write_reg != 0)
		&& (mem_wb.write_reg == id_ex.rt))
		forward_unit->forward_B = 2;
	else if ((temp_buffer.reg_write) && (temp_buffer.write_reg != 0)
		&& (mem_wb.write_reg != id_ex.rt)
		&& (temp_buffer.write_reg == id_ex.rt))
		forward_unit->forward_B = 1;
	else
		forward_unit->forward_B = 0;

    return 0;
}
