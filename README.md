# RISCV

I created an emulator that is able to execute a subset of the RISC-V instruction set architecture, under the guidance of Randy Katz. 
Provided the machinery to decode and execute a subset RISC-V instructions shown below. 

In addition to executing the instructions provided, the system is capable of soring and loading values at specified addresses as defined below. 

execute_instruction() - executes the instruction provided as a parameter. This should modify the appropriate registers, make any necessary calls to memory, and updatge the program counter to refer to the next instruction to execute.

execute()'s - various helper files to be called in certain conditions for certain instructions. Whether you use these functions is up to you, but they will greatly help you organize your code.

store() - takes an address, a size, and a value and stores the first -size- bytes of the given value at the given address. The check_align parameter will enforce alignment constraints when the parameter is 1. We include this parameter to enforce that instructions are word-aligned. 

load() - takes an address and a size and returns the next -size- bytes starting at the given address. The check_align is the same as that of store().

<p align="center">
  <img width="571" height="831" src="https://github.com/YFateen/RISCV/blob/master/Photos/ISA.png">
</p>
