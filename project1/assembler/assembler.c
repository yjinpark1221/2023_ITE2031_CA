/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAX_LABEL_SIZE 6

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *, int *);
int getFileSize(FILE* fp);
void addLabel(char* label, int lineCount);
int labelToNumeric(char* label);
int getRtypeRegisters(char* a0, char* a1, char* a2, int* r0, int* r1, int* r2);
int getRtypeInstruction(int opcode, int regA, int regB, int destReg);
int getItypeRegisters(char* a0, char* a1, char* a2, int* r0, int* r1, int* r2, int);
int getItypeInstruction(int opcode, int regA, int regB, int offsetField);
int getJtypeRegisters(char* a0, char* a1, int* r0, int* r1);
int getJtypeInstruction(int opcode, int regA, int regB);
int getOtypeInstruction(int opcode);
int getFillRegister(char* a0, int* r0);

char* labelArray;
int labelCount, maxLabelCount;

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* TODO: Phase-1 label calculation */
	int fileSize = getFileSize(inFilePtr);
	maxLabelCount = fileSize / 6 + 1;
	labelArray = (char*) calloc(6, maxLabelCount);
	for (int lineCount = 0; ; ++lineCount) {

		/* here is an example for how to use readAndParse to read a line from
			inFilePtr */
		if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
			/* reached end of file */
			break;
		}
		if (label[0]) {
			printf("label[%d] : %s\n", lineCount, label);
			addLabel(label, lineCount);
		}
	}

	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */

	for (int lineCount = 0; ; ++lineCount) {
		if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
			/* reached end of file */
			break;
		}

		/* after doing a readAndParse, you may want to do the following to test the
			opcode */
		int machineCodeBuffer = 0, iopcode = -1;
		if (!strcmp(opcode, "add")) {
			/* do whatever you need to do for opcode "add" */
			iopcode = 0; 
			int regA, regB, destReg;
			if (getRtypeRegisters(arg0, arg1, arg2, &regA, &regB, &destReg)) printf("add register not integer\n");
			machineCodeBuffer = getRtypeInstruction(iopcode, regA, regB, destReg);
		}
		else if (!strcmp(opcode, "nor")) {
			iopcode = 1;
			int regA, regB, destReg;
			if (getRtypeRegisters(arg0, arg1, arg2, &regA, &regB, &destReg)) printf("nor register not integer\n");
			machineCodeBuffer = getRtypeInstruction(iopcode, regA, regB, destReg);
		}
		else if (!strcmp(opcode, "lw")) {
			iopcode = 2;
			int regA, regB, offsetField;
			if (getItypeRegisters(arg0, arg1, arg2, &regA, &regB, &offsetField, -1)) printf("lw register error\n");
			machineCodeBuffer = getItypeInstruction(iopcode, regA, regB, offsetField);

		}
		else if (!strcmp(opcode, "sw")) {
			iopcode = 3;
			int regA, regB, offsetField;
			if (getItypeRegisters(arg0, arg1, arg2, &regA, &regB, &offsetField, -1)) printf("sw register error\n");
			machineCodeBuffer = getItypeInstruction(iopcode, regA, regB, offsetField);
		}
		else if (!strcmp(opcode, "beq")) {
			iopcode = 4;
			int regA, regB, offsetField;
			if (getItypeRegisters(arg0, arg1, arg2, &regA, &regB, &offsetField, lineCount)) printf("beq register error\n");
			machineCodeBuffer = getItypeInstruction(iopcode, regA, regB, offsetField);
		}
		else if (!strcmp(opcode, "jalr")) {
			iopcode = 5;
			int regA, regB;
			if (getJtypeRegisters(arg0, arg1, &regA, &regB)) printf("jalr register error\n");
			machineCodeBuffer = getJtypeInstruction(iopcode, regA, regB);
		}
		else if (!strcmp(opcode, "halt")) {
			iopcode = 6;	
			machineCodeBuffer = getOtypeInstruction(iopcode);
		}
		else if (!strcmp(opcode, "noop")) {
			iopcode = 7;
			machineCodeBuffer = getOtypeInstruction(iopcode);
		}
		else if (!strcmp(opcode, ".fill")) {
			int regA = -1;
			if (getFillRegister(arg0, &regA)) printf("fill register error\n");
			machineCodeBuffer = regA;
		}
		else {
			printf("opcode error\n");
		}
		printf("%s\t(address %d): %d (hex 0x%x)\n", opcode, lineCount, machineCodeBuffer, machineCodeBuffer);
	} 

	free(labelArray);

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string, int* pi)
{
	/* return 1 if string is a number */
	return( (sscanf(string, "%d", pi)) == 1);
}

int getFileSize(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	rewind(fp);
	return fileSize;
}

void addLabel(char* label, int lineCount) {
	for (int i = 0; i < MAX_LABEL_SIZE; ++i) {
		labelArray[i + lineCount * 6] = label[i];
	}
	++labelCount;
}

int labelToNumeric(char* label) {
	for (int i = 0; i < maxLabelCount; ++i) {
		for (int j = 0; j < 6; ++j) {
			if (labelArray[i * 6 + j] != label[j]) break;
			if (label[j] == '\0' || j == 5) return i;
		}
	}
	return -1;
}

int getRtypeRegisters(char* a0, char* a1, char* a2, int* r0, int* r1, int* r2) {
	if (!isNumber(a0, r0) | !isNumber(a1, r1) | !isNumber(a2, r2)) return 1;
	return 0;
}

int getRtypeInstruction(int opcode, int regA, int regB, int destReg) {
	int ret = 0;
	ret |= (opcode & 0x7) << 22;
	ret |= (regA & 0x7) << 19;
	ret |= (regB & 0x7) << 16;
	ret |= (destReg & 0x7) << 0;
	return ret;
}

int getItypeRegisters(char* a0, char* a1, char* a2, int* r0, int* r1, int* r2, int curLine) {
	if (!isNumber(a0, r0) | !isNumber(a1, r1)) return 1;
	if (isNumber(a2, r2)) return 0;
	*r2 = labelToNumeric(a2);
	if (*r2 == -1) return 1;
	*r2 -= curLine + 1;
	return 0;
}

int getItypeInstruction(int opcode, int regA, int regB, int offsetField) {
	int ret = 0;
	ret |= (opcode & 0x7) << 22;
	ret |= (regA & 0x7) << 19;
	ret |= (regB & 0x7) << 16;
	ret |= ((offsetField >> 31) & 0x1) << 15;
	ret |= (offsetField & 0x7FFF);
	return ret;
}

int getJtypeRegisters(char* a0, char* a1, int* r0, int* r1) {
	if (!isNumber(a0, r0) | !isNumber(a1, r1)) return 1;
	return 0;
}

int getJtypeInstruction(int opcode, int regA, int regB) {
	int ret = 0;
	ret |= (opcode & 0x7) << 22;
	ret |= (regA & 0x7) << 19;
	ret |= (regB & 0x7) << 16;
	return ret;
}

int getOtypeInstruction(int opcode) {
	int ret = 0;
	ret |= (opcode & 0x7) << 22;
	return ret;
}

int getFillRegister(char* a0, int* r0) {
	if (isNumber(a0, r0)) return 0;
	*r0 = labelToNumeric(a0);
	if (*r0 == -1) return 1;
	return 0;
}