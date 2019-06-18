#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {    
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    switch(instruction.opcode) { 

        case 0x33: 
            execute_rtype(instruction, processor);
            processor->PC += 4;
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            processor->PC += 4;
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            processor->PC += 4;
            break;
        case  0x73:
            execute_ecall(processor, memory);
            processor->PC += 4;
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            processor->PC += 4;
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x37:
            execute_lui(instruction, processor);
            processor->PC += 4;
            break;
        case 0x6f:
            execute_jal(instruction, processor);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    unsigned int imm1 = instruction.rtype.rs1;
    unsigned int imm2 = instruction.rtype.rs2;
    unsigned int rd = instruction.rtype.rd;
    sWord rs1 = (processor->R)[imm1]; //signed 
    sWord rs2 = (processor->R)[imm2];

    unsigned int intOne = (processor->R)[imm1]; //unsigned 
    unsigned int intTwo = (processor->R)[imm2];
    unsigned int shift = intTwo & 0x0000001f;

    // int sign1 = rs1 >> 31;
    // int sign2 = rs2 >> 31;
    // if (sign1) {
    //     rs1 += 0xffffffe0;
    // }
    // if (sign2) {
    //     rs2 += 0xffffffe0;
    // }


    // sWord wTemp;
    // sWord wTemp2;
    sDouble dTemp;
    int64_t mulhReturn;
    switch(instruction.rtype.funct3) { 
        case 0x0:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    (processor->R)[rd] = rs1 + rs2; //add
                    break;
                case 0x01:
                    (processor->R)[rd] = rs1 * rs2; //mul
                    
                    break;
                case 0x20:
                    (processor->R)[rd] = rs1 - rs2; //sub
                    
                    break;
            }
            break;
        case 0x1: 
            switch(instruction.rtype.funct7) {
                case 0x00:
                    (processor->R)[rd] = rs1 << shift; //sll
                    
                    break;
                case 0x01:
                    mulhReturn = (sDouble) rs1 * (sDouble) rs2; //mulh
                    mulhReturn = mulhReturn >> 32; 
                    (processor->R)[rd] = mulhReturn;
                    
                    break;
            }
            break;
        case 0x2:
            if (rs1 < rs2) { //slt
                (processor->R)[rd] = 1;
            }else {
                (processor->R)[rd] = 0;
            }
            
            break;
        case 0x4:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    (processor->R)[rd] = intOne ^ intTwo; // xor
                    
                    break;
                case 0x01:
                    (processor->R)[rd] = rs1 / rs2; //div
                    
                    break;
            }
            break;
        case 0x5:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    (processor->R)[rd] = intOne >> shift; //srl
                    
                    break;
                case 0x20:
                    (processor->R)[rd] = rs1 >> shift; //sra 
                                        
                    break;
            }
            break;
        case 0x6:
            switch(instruction.rtype.funct7) {
                case 0x00:
                    (processor->R)[rd] = intOne | intTwo; //or
                     
                    break;
                case 0x01:
                    (processor->R)[rd] = rs1 % rs2; //rem
                     
                    break;
            }
            break;
        case 0x7:
            (processor->R)[rd] = intOne & intTwo; //and
             
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }

}

void execute_itype_except_load(Instruction instruction, Processor *processor) {

    sWord imm = instruction.itype.imm;
    unsigned int shift = imm & 0x0000001f;
    sWord registers1 = (processor->R)[instruction.itype.rs1];
    Word uregister1 = (processor->R)[instruction.itype.rs1];
    unsigned int rd = instruction.itype.rd;
    unsigned int rs1 = instruction.itype.rs1;
    unsigned int funct7 = imm >> 5;
    // unsigned int shiftLength = (imm & 0x01f);
    // unsigned int shiftMask = 0xffffffff << (32 - shiftLength);
    // int rs1Sign = (processor->R)[rs1] >> 31;

    int sign = imm >> 11;
    if (sign) {
        imm += 0xfffff000;
    }

    switch(instruction.itype.funct3) { 

        case 0x0:
            processor->R[rd] =  registers1 + imm;   
            break;
        case 0x1:
            processor->R[rd] = processor->R[rs1] << shift;
            break;
        case 0x2:
            if (registers1 < imm) {
                processor->R[rd] = 1;
            } else {
                processor->R[rd] = processor->R[0];
            }
            break;
        case 0x4:
            processor->R[rd] = (processor->R[rs1]) ^ imm;
            break;
        case 0x5:
            switch(funct7) {
                case 0x00:
                    processor->R[rd] = uregister1 >> shift; //srli
                    break;
                case 0x20:
                    (processor->R)[rd] = registers1 >> shift; //srla
                    // if (rs1Sign) {
                    //     (processor->R)[rd] = (processor->R)[rd] + shiftMask;
                    // }
                    break;
            }
            break;
        case 0x6:
            processor->R[rd] = processor->R[rs1] | imm;
            break;
        case 0x7:
            processor->R[rd] = processor->R[rs1] & imm;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    switch(p->R[10]) { // What do we switch on?
        case (1):
            printf("%d", p->R[11]);
            break;
        case (10):
            printf("%s\n", "exiting the simulator");
            exit(0);
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", -1); // What stores the ecall arg?
            exit(-1);
            break;
    }
}

void execute_branch(Instruction instruction, Processor *processor) {
    // int branchaddr;
    // branchaddr = 0;
    /* Remember that the immediate portion of branches
       is counting half-words, so make sure to account for that. */

        unsigned int rs1 = instruction.sbtype.rs1;
        unsigned int rs2 = instruction.sbtype.rs2;
        unsigned int imm5 = instruction.sbtype.imm5;
        imm5 = (imm5 >> 1) << 1; //turns first bit to 0, same as slli 
        Word imm7 = instruction.sbtype.imm7;
        Word imm12 = (imm7 << 5) + imm5; 
        int sign = (imm12 >> 11);
        // printf("%i", imm12);
        if (sign) {
            Word one = 0xffffffff;
            one = one << 12;
            imm12 += one;  
        } 
        signed int offset = (int) imm12;
        switch(instruction.sbtype.funct3) { 
            case 0x0:
                if (((processor->R)[rs1])== ((processor->R)[rs2])) {
                    processor->PC += offset;
                } else {
                    processor->PC += 4;
                }
                break;
            case 0x1:
                if (processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]) {
                    processor->PC += offset;
                } else {
                    processor->PC += 4;
                }
                break;
            default:
                handle_invalid_instruction(instruction);
                exit(-1);
                break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    unsigned int rd = instruction.itype.rd;
    unsigned int imm1 = instruction.itype.rs1;
    sWord rs1 = (processor->R)[imm1]; 
    signed int imm = instruction.itype.imm;
    signed int sign = imm >> 11; 
    if (sign) {
        imm = imm | 0xfffff000;
    }
    rs1 += imm;  
    switch(instruction.itype.funct3) { 

        case 0x0:
            (processor->R)[rd] = load(memory, rs1, LENGTH_BYTE, 0); //lb
            
            break;
        case 0x1:
            (processor->R)[rd] = load(memory, rs1, LENGTH_HALF_WORD, 0); //lh
            
            break;
        case 0x2:
            (processor->R)[rd] =  load(memory, rs1, LENGTH_WORD, 0); //lw
            
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    sWord imm7 = instruction.stype.imm7;
    unsigned int imm5 = instruction.stype.imm5;
    sWord imm12 = (imm7 << 5) + imm5;
    unsigned int imm1 = instruction.stype.rs1;
    unsigned int imm2 = instruction.stype.rs2;
    sWord rs1 = (processor->R)[imm1]; 
    Word rs2 = (processor->R)[imm2];  
    unsigned int sign = imm12 >> 11;
    if (sign) {
        imm12 = imm12 | 0xfffff000;
    }
    rs1 += imm12;
    switch(instruction.stype.funct3) { 
        case 0x0:
            store(memory, rs1, LENGTH_BYTE, rs2, 0);
            break;
        case 0x1:
            store(memory, rs1, LENGTH_HALF_WORD, rs2, 0);
            break;
        case 0x2:
            store(memory, rs1, LENGTH_WORD, rs2, 0);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_jal(Instruction instruction, Processor *processor) {
    unsigned int rd = instruction.ujtype.rd;
    processor->R[rd] = processor->PC + 4;
    Word imm = instruction.ujtype.imm;
    unsigned int sign = (imm & 0x80000);
    unsigned int tenToOne = (((imm & 0x7fe00) << 1) >> 10);
    unsigned int eleven = (imm & 0x00100) << 2;
    unsigned int twelveToNineteen = (imm & 0x000ff) << 11;
    sWord imm20 = sign + tenToOne + eleven + twelveToNineteen;
    imm20 = imm20 << 1;
    if (sign) { //bitsigner
        imm20 = imm20 | 0xffe00000;
    }
    processor->PC += imm20; 
}

void execute_lui(Instruction instruction, Processor *processor) {
    Word upper = instruction.utype.imm;
    upper = upper << 12;
    (processor->R)[instruction.utype.rd] = upper;  
}

/* Checks that the address is aligned correctly */
int check(Address address,Alignment alignment) {
    if(address>0 && address < MEMORY_SPACE){
        if(alignment == LENGTH_BYTE){
            return 1;
        }
        else if( alignment == LENGTH_HALF_WORD ){
            return address%2 == 0;
        }
        else if (alignment == LENGTH_WORD){
            return address%4 ==0;
        }
    }   
    return 0;
}

void store(Byte *memory,Address address,Alignment alignment,Word value, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_write(address);
    }
    unsigned int curr = 0; 
    if (alignment == LENGTH_BYTE){
        curr = 1;
    }
    else if (alignment == LENGTH_HALF_WORD ){
        curr = 2;
    }
    else if (alignment == LENGTH_WORD){
        curr = 4;
    }
    int counter = 0;
    while (counter != curr) {
        *(memory + (signed int) address + counter) = value & 0x00000ff; 
        value = value >> 8; 
        counter ++;
    }
}

Word load(Byte *memory,Address address,Alignment alignment, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_read(address);
    }
    
    Word ret = 0;
    unsigned int curr = 0; 
    unsigned int curr1 = 0;

    if (alignment == LENGTH_BYTE){
        curr = 1;
        curr1 = 1;
    }
    else if (alignment == LENGTH_HALF_WORD ){
        curr = 2;
        curr1 = 2;
    }
    else if (alignment == LENGTH_WORD){
        curr = 4;
        curr1 = 4;
    }

    while(curr) {

        ret += (*(memory + (signed int)address + curr - 1)) & 0x00000ff; 
        if (curr != 1) {
            ret = ret << 8; //make room for next byte 
        }
        curr--; 
    }
    int sign = ret >> ((curr1 * 8) - 1);
    if (sign) { //bitSigner 
        if (alignment == LENGTH_BYTE) {
            ret = ret | 0xffffff00;
        }
        if (alignment == LENGTH_HALF_WORD) {
            ret = ret | 0xffff0000;
        }
    }
    return ret;
}


