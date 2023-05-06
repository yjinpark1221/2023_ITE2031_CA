/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);
int getRtypeRegisters(int instruction, int* regA, int* regB, int* destReg);
int getItypeRegisters(int instruction, int* regA, int* regB, int* offsetField);
int getJtypeRegisters(int instruction, int* regA, int* regB);
void panic();
int getOpcode(int instruction);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }
    
    for (int i = 0; i < NUMREGS; ++i) {
        state.reg[i] = 0;
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    for (state.pc = 0; ; state.pc++) {
        printState(&state);
        int instruction = state.mem[state.pc];
        int opcode = getOpcode(instruction);
        if (opcode == 0) { // add
            int regA, regB, destReg;
            if (getRtypeRegisters(instruction, &regA, &regB, &destReg)) panic();
            state.reg[destReg] = state.reg[regA] + state.reg[regB];
        }
        else if (opcode == 1) { // nor
            int regA, regB, destReg;
            if (getRtypeRegisters(instruction, &regA, &regB, &destReg)) panic();
            state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
        }
        else if (opcode == 2) { // lw
            int regA, regB, offsetField;
            if (getItypeRegisters(instruction, &regA, &regB, &offsetField)) panic();
            state.reg[regB] = state.mem[offsetField + state.reg[regA]];
        }
        else if (opcode == 3) { // sw
            int regA, regB, offsetField;
            if (getItypeRegisters(instruction, &regA, &regB, &offsetField)) panic();
            state.mem[offsetField + state.reg[regA]] = state.reg[regB];
        }
        else if (opcode == 4) { // beq
            int regA, regB, offsetField;
            if (getItypeRegisters(instruction, &regA, &regB, &offsetField)) panic();
            if (state.reg[regA] == state.reg[regB]) state.pc += offsetField; 
        }
        else if (opcode == 5) {
            int regA, regB;
            if (getJtypeRegisters(instruction, &regA, &regB)) panic();
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regB] - 1;
        }
        else if (opcode == 6) {
            ++state.pc;
            printf("machine halted\ntotal of 17 instructions executed\nfinal state of machine:\n");
            printState(&state);
            break;
        }
    }
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}

int getOpcode(int instruction) {
    return (instruction >> 22) & 0x7;
}

int getRtypeRegisters(int instruction, int* regA, int* regB, int* destReg) {
    int opcode = getOpcode(instruction);
    if (opcode != 0 && opcode != 1) {
        return 1;
    } 
    *regA = (instruction >> 19) & 0x7;
    *regB = (instruction >> 16) & 0x7;
    *destReg = instruction & 0x7;
    return 0;
}

int getItypeRegisters(int instruction, int* regA, int* regB, int* offsetField) {
    int opcode = getOpcode(instruction);
    if (opcode != 2 && opcode != 3 && opcode != 4) {
        return 1;
    } 
    *regA = (instruction >> 19) & 0x7;
    *regB = (instruction >> 16) & 0x7;
    *offsetField = convertNum(instruction & 0xFFFF);
    return 0;
}

int getJtypeRegisters(int instruction, int* regA, int* regB) {
    int opcode = getOpcode(instruction);
    if (opcode != 5) {
        return 1;
    } 
    *regA = (instruction >> 19) & 0x7;
    *regB = (instruction >> 16) & 0x7;
    return 0;
}

void panic() {
    exit(1);
}