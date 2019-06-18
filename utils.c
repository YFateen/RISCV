#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitSigner( unsigned int field, unsigned int size){
    int sign = (field >> (size-1));
    if (sign) {
        Word one = 0xffffffff;
        one = one << size;
        field += one;  
    } 
    return (int)field;
}

/* Remember that the offsets should return the offset in BYTES */

int get_branch_offset(Instruction instruction) {
    unsigned int imm5 = instruction.sbtype.imm5;
    imm5 = (imm5 >> 1) << 1; //turns first bit to 0, same as slli 
    Word imm7 = instruction.sbtype.imm7;
    Word imm12 = (imm7 << 5) + imm5; 
    return bitSigner(imm12, 12);
}

int get_jump_offset(Instruction instruction) {
    Word imm = instruction.ujtype.imm;
    unsigned int sign = (imm & 0x80000);
    unsigned int tenToOne = (((imm & 0x7fe00) << 1) >> 10);
    unsigned int eleven = (imm & 0x00100) << 2;
    unsigned int twelveToNineteen = (imm & 0x000ff) << 11;

    Word imm20 = sign + tenToOne + eleven + twelveToNineteen;

    // unsigned int imm11 = ((imm << 1) >> 10) << 1; //turns first bit to 0, same as slli
    // unsigned int imm9 = (imm & 0x001ff) ;
    // if (sign) { //account for sign extension 
    //     imm9 -= 1;
    // } 
    // imm9 = imm9 << 11;
    // Word imm20 = imm9 + imm11;
    return (bitSigner(imm20, 20) << 1);
}

int get_store_offset(Instruction instruction) {
    Word imm7 = instruction.stype.imm7;
    unsigned int imm5 = instruction.stype.imm5;
    Word imm12 = (imm7 << 5) + imm5;
    return bitSigner(imm12, 12);
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits); 
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}

