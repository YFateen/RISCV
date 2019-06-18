#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"

void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction); 
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);
void write_lui(Instruction);
void write_jal(Instruction);
void write_ecall(Instruction);

void decode_instruction(Instruction instruction) {

switch(instruction.opcode) { 

        case 0x33: 
            write_rtype(instruction);
            break;
        case 0x03:
            write_load(instruction);
            break;
        case 0x13:
            write_itype_except_load(instruction);
            break;
        case  0x73:
            write_ecall(instruction);
            break;
        case 0x23:
            write_store(instruction);
            break;
        case 0x63:
            write_branch(instruction);
            break;
        case 0x37:
            write_lui(instruction);
            break;
        case 0x6f:
            write_jal(instruction);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_rtype(Instruction instruction) {
switch(instruction.rtype.funct3) { 

        case 0x0:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    print_rtype("add", instruction);
                    break;
                case 0x01:
                    print_rtype("mul", instruction);
                    break;
                case 0x20:
                    print_rtype("sub", instruction);
                    break;
            }
            break;
        case 0x1: 
            switch(instruction.rtype.funct7) {
                case 0x00:
                    print_rtype("sll", instruction);
                    break;
                case 0x01:
                    print_rtype("mulh", instruction);
                    break;
            }
            break;
        case 0x2:
            print_rtype("slt", instruction);
            break;
        case 0x4:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    print_rtype("xor", instruction);
                    break;
                case 0x01:
                    print_rtype("div", instruction);
                    break;
            }
            break;
        case 0x5:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    print_rtype("srl", instruction);
                    break;
                case 0x20:
                    print_rtype("sra", instruction);
                    break;
            }
            break;
        case 0x6:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    print_rtype("or", instruction);
                    break;
                case 0x01:
                    print_rtype("rem", instruction);
                    break;
            }
            break;
        case 0x7:
            print_rtype("and", instruction);
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_itype_except_load(Instruction instruction) {
    unsigned int imm = bitSigner(instruction.itype.imm, 12);
    unsigned int mask = 0x0000001f;
    unsigned int imm7 = imm >> 5;
    switch(instruction.itype.funct3) { 

        case 0x0:
            print_itype_except_load("addi", instruction, imm);
            break;
        case 0x1:
            print_itype_except_load("slli", instruction, imm);
            break;
        case 0x2:
            print_itype_except_load("slti", instruction, imm);
            break;
        case 0x4:
            print_itype_except_load("xori", instruction, imm);
            break;
        case 0x5:
            switch(imm7) {
                case 0x00:
                    print_itype_except_load("srli", instruction, imm);
                    break;
                case 0x20:
                    imm = imm & mask;
                    print_itype_except_load("srai", instruction, imm);
                    break;
            }
            break;
        case 0x6:
            print_itype_except_load("ori", instruction, imm);
            break;
        case 0x7:
            print_itype_except_load("andi", instruction, imm);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;  
    }
}

void write_load(Instruction instruction) {
    switch(instruction.itype.funct3) { 

        case 0x0:
            print_load("lb", instruction);
            break;
        case 0x1:
            print_load("lh", instruction);
            break;
        case 0x2:
            print_load("lw", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_store(Instruction instruction) {
    switch(instruction.stype.funct3) { 

        case 0x0:
            print_store("sb", instruction);
            break;
        case 0x1:
            print_store("sh", instruction);
            break;
        case 0x2:
            print_store("sw", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_branch(Instruction instruction) {
    switch(instruction.sbtype.funct3) { 

        case 0x0:
            print_branch("beq", instruction);
            break;
        case 0x1:
            print_branch("bne", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

/* For the writes, probably a good idea to take a look at utils.h */

void write_lui(Instruction instruction) {
    unsigned int rd = instruction.utype.rd;
    unsigned int immediate = instruction.utype.imm;
    printf(LUI_FORMAT, rd, immediate);
}

void write_jal(Instruction instruction) {
    unsigned int rd = instruction.ujtype.rd;
    int immediate = get_jump_offset(instruction);
    printf(JAL_FORMAT, rd, immediate);
}

void write_ecall(Instruction instruction) {
    printf(ECALL_FORMAT);
}

void print_rtype(char *name, Instruction instruction) {
    unsigned int rd = instruction.rtype.rd;
    unsigned int rs1 = instruction.rtype.rs1;
    unsigned int rs2 = instruction.rtype.rs2;
    printf(RTYPE_FORMAT, name, rd, rs1, rs2);
}

void print_itype_except_load(char *name, Instruction instruction, int imm) {
    unsigned int rd = instruction.itype.rd;
    unsigned int rs1 = instruction.itype.rs1;   
    printf(ITYPE_FORMAT, name, rd, rs1, imm);
}

void print_load(char *name, Instruction instruction) {
    unsigned int rd = instruction.itype.rd;
    unsigned int rs1 = instruction.itype.rs1;
    int offset = bitSigner(instruction.itype.imm,12);
    printf(MEM_FORMAT, name, rd, offset, rs1);
}

void print_store(char *name, Instruction instruction) {
    unsigned int rs1 = instruction.stype.rs1;
    unsigned int rs2 = instruction.stype.rs2;
    int offset = get_store_offset(instruction);
    printf(MEM_FORMAT, name, rs2, offset, rs1);
}

void print_branch(char *name, Instruction instruction) {
    unsigned int rs1 = instruction.stype.rs1;
    unsigned int rs2 = instruction.stype.rs2;
    int offset = get_branch_offset(instruction);
    printf(BRANCH_FORMAT, name, rs1, rs2, offset);
}
