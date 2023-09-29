// Justin Lugo, Nuridean Mubarak
// COP 3402, Fall 2021
// 19 November 2021

// While we wrote our owns, we copied them over into the skeleton just to make sure.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define MAX_PAS_LENGTH 3000

instruction controls;
int pas[MAX_PAS_LENGTH];
int PC, BP, SP, DP, GP, IC, FREE;

void print_execution(int line, char *opname, struct instruction controls, int PC, int BP, int SP, int DP, int *pas, int GP)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, controls.l, controls.m, PC, BP, SP, DP);

	// print data section
	for (i = GP; i <= DP; i++)
		printf("%d ", pas[i]);
	printf("\n");

	// print stack
	printf("\tstack : ");
	for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
		printf("%d ", pas[i]);
	printf("\n");
}

int base(int L, int BP, int *pas)
{
	int ctr = L;
	int rtn = BP;
	while (ctr > 0)
	{
		rtn = pas[rtn-1];
		ctr--;
	}
	return rtn;
}

void execute_program(instruction *code, int printFlag)
{
	// variables
	int *pas = calloc(MAX_PAS_LENGTH, sizeof(int));
	int IC, i;
	int flag = 0;
	int line = PC;
	char opName[4];

	// read in program
	IC = 0;
	i = 0;
	while (code[i].opcode != -1)
	{
		pas[IC] = code[i].opcode;
		IC++;
		pas[IC] = code[i].l;
		IC++;
		pas[IC] = code[i].m;
		IC++;
		i++;
	}

	// Time to set up the CPU registers, as given to us.
	GP = IC; // Global Pointer
	DP = IC - 1; // Data Pointer
	FREE = IC + 40; // Points to Heap
	BP = IC; // Points to Data base or AR
	PC = 0; // Stack pointer
	SP = MAX_PAS_LENGTH;

	// this will print the header in the output, make sure you put your
	//		execution loop after it
	if (printFlag)
	{
		printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
		printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);
	}

	while (flag == 0)
  {
    controls.opcode = pas[PC];
    controls.l = pas[PC + 1];
    controls.m = pas[PC + 2];
    line = PC / 3;

    // Update the PC.
    PC += 3;

    // Now we set up all the values of our registers and go through whatever functions get called
    // through the text file.
    switch (controls.opcode)
    {
      // LIT
      case 1:
        strcpy(opName, "LIT");
        if (BP == GP)
        {
          DP = DP + 1;
          pas[DP] = controls.m;
        }
        else
        {
          SP = SP - 1;
          pas[SP] = controls.m;
        }
        break;

      // OPR
      case 2:
        switch (controls.m)
        {
          // RTN; Return.
          case 0:
            strcpy(opName, "RTN");
            SP = BP + 1;
            BP = pas[SP - 3]; // -4
            PC = pas[SP - 4]; // -3
            break;

          // NEG; Negation.
          case 1:
            strcpy(opName, "NEG");
            if (BP == GP)
              pas[DP] = -1 * pas[DP];
            else
              pas[SP] = -1 * pas[SP];
            break;

          // ADD; Addition.
          case 2:
            strcpy(opName, "ADD");
            if (BP == GP)
            {
              DP = DP - 1;
              pas[DP] = pas[DP] + pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] + pas[SP - 1];

            }
            break;

          // SUB; Subtraction.
          case 3:
            strcpy(opName, "SUB");
            if (BP == GP)
            {
              DP = DP - 1;
              pas[DP] = pas[DP] - pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] - pas[SP - 1];
            }
            break;

          // MUL; Multiplication.
          case 4:
            strcpy(opName, "MUL");
            if (BP == GP)
            {
              DP = DP - 1;
              pas[DP] = pas[DP] * pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] * pas[SP - 1];
            }
            break;

          // DIV; Division.
          case 5:
            strcpy(opName, "DIV");
            if (BP == GP)
            {
              DP = DP - 1;
              pas[DP] = pas[DP] / pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] / pas[SP - 1];
            }
            break;

          // ODD; Determine if odd.
          case 6:
            strcpy(opName, "ODD");
            if (BP == GP)
              pas[DP] = pas[DP] % 2;
            else
              pas[SP] = pas[SP] % 2;
            break;

          // MOD; Modulo by next value.
          case 7:
            strcpy(opName, "MOD");
            if (BP == GP)
            {
              DP = DP - 1;
              pas[DP] = pas[DP] % pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] % pas[SP - 1];
            }
            break;

          // EQL; Determine if equal.
          case 8:
            strcpy(opName, "EQL");
            if (BP == GP)
            {
              DP = DP - 1;
              pas[DP] = pas[DP] == pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] == pas[SP - 1];
            }
            break;

          // NEQ; Determine if not equal.
          case 9:
            strcpy(opName, "NEQ");
            if (BP == GP)
            {
              DP = DP -1;
              pas[DP] = pas[DP] != pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] != pas[SP - 1];
            }
            break;

          // LSS; Check if less than.
          case 10:
            strcpy(opName, "LSS");
            if (BP == GP)
            {
              DP = DP -1;
              pas[DP] = pas[DP] < pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] < pas[SP - 1];
            }
            break;

          // LEQ; Check if less than or equal too.
          case 11:
            strcpy(opName, "LEQ");
            if (BP == GP)
            {
              DP = DP -1;
              pas[DP] = pas[DP] <= pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] <= pas[SP - 1];
            }
            break;

          // GTR; Check if greater than.
          case 12:
            strcpy(opName, "GTR");
            if (BP == GP)
            {
              DP = DP -1;
              pas[DP] = pas[DP] > pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] > pas[SP - 1];
            }
            break;

          // GEQ; Check if greater than or equal to.
          case 13:
            strcpy(opName, "GEQ");
            if (BP == GP)
            {
              DP = DP -1;
              pas[DP] = pas[DP] >= pas[DP + 1];
            }
            else
            {
              SP = SP + 1;
              pas[SP] = pas[SP] >= pas[SP - 1];
            }
            break;
        }
        break;

        // LOD
        case 3:
          strcpy(opName, "LOD");
          if (BP == GP)
          {
            DP += 1;
            pas[DP] = pas[GP + controls.m];
          }
          else
          {
            if (base(controls.l, BP, pas) == GP)
            {
              SP -= 1;
              pas[SP] = pas[GP + controls.m];
            }
            else
            {
              SP -= 1;
              pas[SP] = pas[base(controls.l, BP, pas) - controls.m];
            }
          }
          break;

        // STO
        case 4:
          strcpy(opName, "STO");
          if (BP == GP)
          {
            pas[GP + controls.m] = pas[DP];
            DP = DP - 1;
          }
          else
          {
            if (base(controls.l, BP, pas) == GP)
            {
              pas[GP + controls.m] = pas[SP];
              SP = SP + 1;
            }
            else
            {
              pas[base(controls.l, BP, pas) - controls.m] = pas[SP];
              SP = SP + 1;
            }
          }
          break;

        // CAL
        case 5:
          strcpy(opName, "CAL");
          pas[SP - 1] = 0;                          // Functional Value
          pas[SP - 2] = base(controls.l, BP, pas);  // Static Link (SL)
          pas[SP - 3] = BP;                         // Dynamic Link (DL)
		      pas[SP - 4] = PC;                         // Return Address (RA)
          BP = SP - 1;
          PC = controls.m;
          break;

        // INC
        case 6:
          strcpy(opName, "INC");
          if (BP == GP)
            DP = DP + controls.m;
          else
            SP = SP - controls.m;
          break;

        // JMP
        case 7:
          strcpy(opName, "JMP");
          PC = controls.m;
          break;

        // JPC
        case 8:
          strcpy(opName, "JPC");
          if (BP == GP)
          {
            if (pas[DP] == 0)
              PC = controls.m;
            DP = DP -1;
          }
          else
          {
            if (pas[SP] == 0)
              PC = controls.m;
            SP = SP + 1;
          }
          break;

        // SYS
        case 9:
          switch(controls.m)
          {
            // m = 1; show the top value of the stack.
            case 1:
            {
              strcpy(opName, "SYS");
              if (BP == GP)
              {
                printf("Top of stack value: %d\n", pas[DP]);
                DP = DP - 1;
              }
              else
              {
                printf("Top of stack value: %d\n", pas[SP]);
                SP = SP + 1;
              }
              break;
            }

            // m = 2; take an input.
            case 2:
            {
              strcpy(opName, "SYS");
              if (BP == GP)
              {
                DP = DP + 1;
                printf("Please enter an integer: ");
                scanf("%d", &pas[DP]);
              }
              else
              {
                SP = SP - 1;
                printf("Please enter an integer:");
                scanf("%d", &pas[SP]);
              }
              break;
            }

            // m = 3; halt completely.
            case 3:
              strcpy(opName, "SYS");
              flag = 1;
              break;
          }
          break;
    }
    if (printFlag == 1)
    {
      print_execution(line, opName, controls, PC, BP, SP, DP, pas, GP);
    }
  }

	free(pas);
}
