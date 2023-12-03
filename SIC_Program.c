#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define opCount 59

//  optable
struct optab 
{
	char   code[10], objCode[10];
};

static struct optab optable[opCount] = 
{
	{ "ADD","18" },
	{ "ADDF","58" },
	{ "ADDR","90" },
	{ "AND","40" },
	{ "CLEAR","B4" },
	{ "COMP","28" },
	{ "COMPF","88" },
	{ "COMPR","A0" },
	{ "DIV","24" },
	{ "DIVF","64" },
	{ "DIVR","9C" },
	{ "FIX","C4" },
	{ "FLOAT","C0" },
	{ "HIO","F4" },
	{ "J","3C" },
	{ "JEQ","30" },
	{ "JGT","34" },
	{ "JLT","38" },
	{ "JSUB","48" },
	{ "LDA","00" },
	{ "LDB","68" },
	{ "LDCH","50" },
	{ "LDF","70" },
	{ "LDL","08" },
	{ "LDS","6C" },
	{ "LDT","74" },
	{ "LDX","04" },
	{ "LPS","D0" },
	{ "MUL","20" },
	{ "MULF","60" },
	{ "MULR","98" },
	{ "NORM","C8" },
	{ "OR","44" },
	{ "RD","D8" },
	{ "RMO","AC" },
	{ "RSUB","4C" },
	{ "SHIFTL","A4" },
	{ "SHIFTR","A8" },
	{ "SIO","F0" },
	{ "SSK","EC" },
	{ "STA","0C" },
	{ "STB","78" },
	{ "STCH","54" },
	{ "STF","80" },
	{ "STI","D4" },
	{ "STL","14" },
	{ "STS","7C" },
	{ "STSW","E8" },
	{ "STT","84" },
	{ "STX","10" },
	{ "SUB","1C" },
	{ "SUBF","5C" },
	{ "SUBR","94" },
	{ "SVC","B0" },
	{ "TD","E0" },
	{ "TIO","F8" },
	{ "TIX","2C" },
	{ "TIXR","B8" },
	{ "WD","DC" }
};

int ByteSize(char opcode[])
{
	if((strcmp(opcode, "LDCH") == 0)
		|| (strcmp(opcode, "STCH") == 0)
		|| (strcmp(opcode, "RESB") == 0)
		|| (strcmp(opcode, "BYTE") == 0)
		)
		return 1;
	return 3;
}
bool IsRESOPCode(char opcode[])
{
	if ((strcmp(opcode, "RESB") == 0)
		|| (strcmp(opcode, "RESW") == 0)
		)
		return true;
	return false;
}

typedef struct Instruction
{
	char code[4][10];
} Instruction;

Instruction instruction;
Instruction symtab[100];
int lineNum;

int startAddr;
int PC;
bool isEnd = false;

void PASS1();
void P1_read_source();
void SourceStatements();
void P1_assign_loc();
void P1_assign_sym();
void P1_write_source();
void ErrorFlags(char errorCode[]);

int main()
{
	PASS1();
}

void PASS1()
{
	P1_read_source();
	P1_write_source();
}
void P1_read_source()
{
	FILE* sourceFile = fopen("input.txt", "r");
	if (sourceFile == NULL)
		ErrorFlags("파일이 존재하지 않습니다.");

	lineNum = 0;
	while (!isEnd)
	{
		char line[30] = "\0";
		fgets(line, 30, sourceFile);

		char temp[10] = "\0";
		int tidx = 0;
		int idx = 1;

		for (int i = 0; line[i] != '\0'; i++)
		{
			if (line[i] != '\t' && line[i] != '\n')
				temp[tidx++] = line[i];
			else
			{
				temp[tidx] = '\0';
				strcpy(instruction.code[idx++], temp);

				for (int t = 0; t < tidx; t++)
					temp[t] = '\0';
				tidx = 0;
			}
		}
		if (lineNum == 0)
		{
			startAddr = strtol(instruction.code[3], NULL, 16);
			PC = startAddr;
		}

		isEnd = (strcmp(instruction.code[2], "END") == 0);
		SourceStatements();
		lineNum++;
	}
}
void SourceStatements()
{
	P1_assign_loc();
	P1_assign_sym();
}
void P1_assign_loc()
{
	char location[10];
	sprintf(location, "%X", PC);
	strcpy(instruction.code[0], location);

	if (lineNum == 0)
		return;

	PC += ByteSize(instruction.code[2]) 
		* (IsRESOPCode(instruction.code[2]) ? strtol(instruction.code[3], NULL, 10) : 1);
}
void P1_assign_sym()
{
	for (int i = 0; i < 4; i++)
		strcpy(symtab[lineNum].code[i], instruction.code[i]);
	if (isEnd)
	{
		strcpy(symtab[lineNum].code[0], "");
		strcpy(symtab[lineNum].code[3], symtab[0].code[1]);
	}
}
void P1_write_source()
{
	printf("LOCATION LABEL\tOPCODE\tOPERAND\n");
	printf("_____________________________________\n");

	FILE* resultFile = fopen("output.txt", "w");
	if(resultFile == NULL)
		ErrorFlags("메모리가 부족하여 파일을 할당하지 못 했습니다.");

	for (int i = 0; i < lineNum; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%s\t", symtab[i].code[j]);
			fprintf(resultFile, "%s\t", symtab[i].code[j]);
		}
		printf("\n");
		fprintf(resultFile, "\n");
	}
	fclose(resultFile);
}
void ErrorFlags(char errorCode[])
{
	printf("%s\n", errorCode);
	exit(1);
}