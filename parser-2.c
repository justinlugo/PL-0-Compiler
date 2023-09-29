// Justin Lugo, Nuridean Mubarak
// COP 3402, Fall 2021
// 19 November 2021

// While we wrote our owns, we copied them over into the skeleton just to make sure.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

// Declaring our global variables to use in our functions.
instruction *code;
symbol *table;
int cIndex, halt, listidx, level, tIndex;

// Our provided functions.
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();


// We divvy up all of the steps of the parser into their respective functions for ease of coding
// as well as following the steps within the document, according to their pseudocode.
void program(lexeme *list);
void block(lexeme *list);
void const_declaration(lexeme *list);
int var_declaration(lexeme *list);
void proc_declaration(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
int mult_declaration(lexeme *list);
int find_symbol(lexeme *list, int kind);
void mark(lexeme *list);

void program(lexeme *list)
{
	// This function is the meat of the program, going so far as to control the beginning and end of
	// the program.
	int i;
	halt = 0;
	emit(7, 0, 0);

	addToSymbolTable(3, "main", 0, 0, 0, 0);

	level = -1;

	block(list);

	if (halt == 1)
	{
		return;
	}

	// The program must be closed by a period, else we print error message #1.
	if (list[listidx].type != periodsym)
	{
		printparseerror(1);
		halt = 1;
		return;
	}

	emit(9, 0, 3);

	for (i = 0; i < cIndex; i++)
	{
		if (code[i].opcode == 5)
		{
			code[i].m = table[code[i].m].addr;
		}
	}

	code[0].m = table[0].addr;
}

void block(lexeme *list)
{
	// This function is called by program() and works as our control room, sending our list into the
	// various other functions step-by-step so that we can parse our text files.
	int procedure_idx, x;

	level++;

	procedure_idx = tIndex - 1;

	const_declaration(list);

	if (halt == 1)
	{
		return;
	}

	x = var_declaration(list);

	if (halt == 1)
	{
		return;
	}

	proc_declaration(list);

	if (halt == 1)
	{
		return;
	}

	table[procedure_idx].addr = cIndex * 3;

	if (level == 0)
	{
		emit(6, 0, x);
	}
	else
	{
		emit(6, 0, x + 4);
	}

	statement(list);

	if (halt == 1)
	{
		return;
	}

	mark(list);

	level--;
}

void const_declaration(lexeme *list)
{
	// This function takes care of the constant declarations, and adds them to the symbol table or
	// else returns errors if what is in the index is not applicable.
	int identidx, symidx;

	if (list[listidx].type == constsym)
	{
		do {
			listidx++;

			// Constant declarations must be followed by an identifier, else we print error #2.
			if (list[listidx].type != identsym)
			{
				printparseerror(2);
				halt = 1;
				return;
			}

			symidx = mult_declaration(list);

			// If our identifier is already present within the symbol table, and if it is unmarked,
			// we print error #18.
			if (symidx != -1)
			{
				printparseerror(18);
				halt = 1;
				return;
			}

			identidx = listidx;

			listidx++;

			// If assignsym is missing, print error #2.
			if (list[listidx].type != assignsym)
			{
				printparseerror(2);
				halt = 1;
				return;
			}

			listidx++;

			// If number is missing, print error #2.
			if (list[listidx].type != numbersym)
			{
				printparseerror(2);
				halt = 1;
				return;
			}

			addToSymbolTable(1, list[identidx].name, list[listidx].value, level, 0, 0);
			listidx++;

		} while(list[listidx].type == commasym);

		if (list[listidx].type != semicolonsym)
		{
			// If we check for a semicolon and find an identifier instead, we print error #13.
			if (list[listidx].type == identsym)
			{
				printparseerror(13);
				halt = 1;
				return;
			}
			// If we check for semicolon and don't find either an identifier nor a semicolon, we print
			// error #14.
			else
			{
				printparseerror(14);
				halt = 1;
				return;
			}
		}
		listidx++;
	}
}

int var_declaration(lexeme *list)
{
	// This function takes care of the variable declarations, and adds them to the symbol table or
	// else returns errors if what is in the index is not applicable.
	int numVars, symidx;

	numVars = 0;
	if (list[listidx].type == varsym)
	{
		do {
			numVars++;
			listidx++;

			// If we don't find an indentifier in our index, we print error #3.
			if (list[listidx].type != identsym)
			{
				printparseerror(3);
				halt = 1;
				return 1;
			}

			symidx = mult_declaration(list);

			// If our identifier is already present within the symbol table, and if it is unmarked,
			// we print error #18.
			if (symidx != -1)
			{
				printparseerror(18);
				halt = 1;
				return 1;
			}

			if (level == 0)
			{
				addToSymbolTable(2, list[listidx].name, 0, level, numVars - 1, 0);
			}
			else
			{
				addToSymbolTable(2, list[listidx].name, 0, level, numVars + 3, 0);
			}
			listidx++;
		} while(list[listidx].type == commasym);

		if (list[listidx].type != semicolonsym)
		{
			// If we check for a semicolon and find an identifier instead, we print error #13.
			if (list[listidx].type == identsym)
			{
				printparseerror(13);
				halt = 1;
				return 1;
			}
			// Otherwise we havent find a semicolon nor an identifier, and thus we print error #14.
			else
			{
				printparseerror(14);
				halt = 1;
				return 1;
			}
		}
		listidx++;
	}

	return numVars;
}

void proc_declaration(lexeme *list)
{
	// This function takes care of the procedure declarations, and adds them to the symbol table or
	// else returns errors if what is in the index is not applicable.
	int symidx;

	while (list[listidx].type == procsym)
	{
		listidx++;

		// If identifier or semicolon is missing before we enter block(), print error #4.
		if (list[listidx].type != identsym)
		{
			printparseerror(4);
			halt = 1;
			return;
		}

		symidx = mult_declaration(list);

		// If our symbol is already present and unmarked at the same level, we print error #18.
		if (symidx != -1)
		{
			printparseerror(18);
			halt = 1;
			return;
		}

		addToSymbolTable(3, list[listidx].name, 0, level, 0, 0);

		listidx++;

		// If identifier or semicolon is missing before we enter block(), print error #4.
		if (list[listidx].type != semicolonsym)
		{
			printparseerror(4);
			halt = 1;
			return;
		}

		listidx++;

		block(list);

		// If we don't find a semicolon after going through block(), we print error #14.
		if (list[listidx].type != semicolonsym)
		{
			printparseerror(14);
			halt = 1;
			return;
		}

		listidx++;

		emit(2, 0, 0);

	}
}

void statement(lexeme *list)
{
	// This function takes care of the jump, loop, and action operations, and if what is in the index
	// does not correspond to what we require, return an error.
	int jpcidx, jmpidx, loopidx, symidx;
	if (list[listidx].type == identsym)
	{
		symidx = find_symbol(list, 2);

		if (symidx == -1)
		{
			// We print error #6, was not sure which error to put here honestly.
			if (find_symbol(list, 1) != find_symbol(list, 3))
			{
				printparseerror(6);
				halt = 1;
				return;
			}
			// Otherwise, it means we could not find out identifier, thus we print error #19.
			else
			{
				printparseerror(19);
				halt = 1;
				return;
			}
		}

		listidx++;

		// If we are missing assignsym, we print error #5.
		if (list[listidx].type != assignsym)
		{
			printparseerror(5);
			halt = 1;
			return;
		}

		listidx++;
		expression(list);
		if (halt == 1)
		{
			return;
		}
		emit(4, level - table[symidx].level, table[symidx].addr);
		return;
	}

	if (list[listidx].type == dosym)
	{
		do {
			listidx++;
			statement(list);
			if (halt == 1)
			{
				return;
			}
		} while(list[listidx].type == semicolonsym);

		if (list[listidx].type != odsym)
		{
			// If we expect to find the end symbol but find any of the ones below instead, we print error
			// #15.
			if (list[listidx].type == identsym || list[listidx].type == dosym ||
					list[listidx].type == whensym || list[listidx].type == whilesym ||
					list[listidx].type == readsym || list[listidx].type == writesym ||
					list[listidx].type == callsym)
			{
				printparseerror(15);
				halt = 1;
				return;
			}
			// Otherwise, we still expected to find end symbol but what we found was none of the above,
			// so we print error number #16.
			else /*if (list[listidx].type != identsym && list[listidx].type != dosym &&
					list[listidx].type != whensym && list[listidx].type != whilesym &&
					list[listidx].type != readsym && list[listidx].type != writesym &&
					list[listidx].type != callsym && list[listidx].type != odsym)*/
			{
				printparseerror(16);
				halt = 1;
				return;
			}
		}
		listidx++;
		return;
	}

	if (list[listidx].type == whensym)
	{
		listidx++;
		condition(list);
		if (halt == 1)
		{
			return;
		}

		jpcidx = cIndex;
		emit(8, 0, jpcidx);

		// If our if symbol returns from condition() and the current symbol is not a then symbol, we
		// print out error #8.
		if (list[listidx].type != dosym)
		{
			printparseerror(8);
			halt = 1;
			return;
		}

		listidx++;

		statement(list);
		if (halt == 1)
		{
			return;
		}

		if (list[listidx].type == elsedosym)
		{
			jmpidx = cIndex;
			emit(7, 0, jmpidx);
			code[jpcidx].m = cIndex * 3;
			listidx++;
			statement(list);
			if (halt == 1)
			{
				return;
			}
			code[jmpidx].m = cIndex * 3;
		}
		else
		{
			code[jpcidx].m = cIndex * 3;
		}
		return;
	}

	if (list[listidx].type == whilesym)
	{
		listidx++;
		loopidx = cIndex;
		condition(list);
		if (halt == 1)
		{
			return;
		}

		// If our while symbol returns from conditioin() and the current symbol is not do, we print
		// error number #9.
		if (list[listidx].type != dosym) // != dosym doesnt catch the error for some reason
		{
			printparseerror(9);
			halt = 1;
			return;
		}

		listidx++;
		jpcidx = cIndex;

		emit(8, 0, jpcidx);
		statement(list);
		if (halt == 1)
		{
			return;
		}

		emit(7, 0, loopidx * 3);
		code[jpcidx].m = cIndex * 3;
		return;
	}

	if (list[listidx].type == readsym)
	{
		listidx++;

		// If the read case's identifier is missing, we print out error #6.
		if (list[listidx].type != identsym)
		{
			printparseerror(6);
			halt = 1;
			return;
		}

		symidx = find_symbol(list, 2);

		if (symidx == -1)
		{
			// If our identifier symbol is not a procedure (kind 2), we also print out error #6.
			if (find_symbol(list, 1) != find_symbol(list, 3))
			{
				printparseerror(6);
				halt = 1;
				return;
			}
			// Otherwise our identifier is again unidentified, thus we print out error #19.
			else
			{
				printparseerror(19);
				halt = 1;
				return;
			}
		}

		listidx++;
		emit(9, 0, 2);
		emit(4, level - table[symidx].level, table[symidx].addr);
		return;
	}

	if (list[listidx].type == writesym)
	{
		listidx++;
		expression(list);
		if (halt == 1)
		{
			return;
		}
		emit(9, 0, 1);
		return;
	}

	if (list[listidx].type == callsym)
	{
		listidx++;
		symidx = find_symbol(list, 3);

		if (symidx == -1)
		{
			// If our next symbol is not kind 3, we print error #7.
			if (find_symbol(list, 1) != find_symbol(list, 3))
			{
				printparseerror(7);
				halt = 1;
				return;
			}
			// As well, if our symbol does not have an identifier, we also print error #7.
			else
			{
				printparseerror(7);
				halt = 1;
				return;
			}
		}

		listidx++;
		emit(5, level - table[symidx].level, symidx);
	}
}

void condition(lexeme *list)
{
	// This function takes care of all the conditional operations, and if what is in the index is not
	// a conditional, prints an error.
	if (list[listidx].type == oddsym)
	{
		listidx++;
		expression(list);
		if (halt == 1)
		{
			return;
		}
		emit(2, 0, 6);
	}

	else
	{
		expression(list);
		if (halt == 1)
		{
			return;
		}

		if (list[listidx].type == eqlsym)
		{
			listidx++;
			expression(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 8);
		}
		else if (list[listidx].type == neqsym)
		{
			listidx++;
			expression(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 9);
		}
		else if (list[listidx].type == lsssym)
		{
			listidx++;
			expression(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 10);
		}
		else if (list[listidx].type == leqsym)
		{
			listidx++;
			expression(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 11);
		}
		else if (list[listidx].type == gtrsym)
		{
			listidx++;
			expression(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 12);
		}
		else if (list[listidx].type == geqsym)
		{
			listidx++;
			expression(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 13);
		}
		// If we've gotten this far, that means that the odd symbol was not found and the current
		// symbol is not a relational operator, so we return error #10.
		else
		{
			printparseerror(10);
			halt = 1;
			return;
		}
	}
}

void expression(lexeme *list)
{
	// This function carries out the addition and subtraction operations, and prints an error if the
	// symbol in the table at index is an identifier, a number, or an odd operator.
	if (list[listidx].type == subsym)
	{
		listidx++;
		term(list);
		if (halt == 1)
		{
			return;
		}
		emit(2, 0, 1);

		while (list[listidx].type == addsym || list[listidx].type == subsym)
		{
			if (list[listidx].type == addsym)
			{
				listidx++;
				term(list);
				if (halt == 1)
				{
					return;
				}
				emit(2, 0, 2);
			}
			else
			{
				listidx++;
				term(list);
				if (halt == 1)
				{
					return;
				}
				emit(2, 0, 3);
			}
		}
	}
	else
	{
		if (list[listidx].type == addsym)
		{
			listidx++;
		}
		term(list);
		if (halt == 1)
		{
			return;
		}

		while (list[listidx].type == addsym || list[listidx].type == subsym)
		{
			if (list[listidx].type == addsym)
			{
				listidx++;
				term(list);
				if (halt == 1)
				{
					return;
				}
				emit(2, 0, 2);
			}
			else
			{
				listidx++;
				term(list);
				if (halt == 1)
				{
					return;
				}
				emit(2, 0, 3);
			}
		}
	}

	// If the current symbol is one of the ones below, we return error #17.
	if (list[listidx].type == addsym || list[listidx].type == subsym ||
			list[listidx].type == multsym || list[listidx].type == divsym ||
			list[listidx].type == modsym || list[listidx].type == lparensym ||
			list[listidx].type == identsym || list[listidx].type == numbersym ||
			list[listidx].type == oddsym)
	{
			printparseerror(17);
			halt = 1;
			return;
	}
}

void term(lexeme *list)
{
	// This function carries out the multiplication, division, and modulus operations.
	factor(list);
	if (halt == 1)
	{
		return;
	}

	while (list[listidx].type == multsym || list[listidx].type == divsym  || list[listidx].type == modsym)
	{
		if (list[listidx].type == multsym)
		{
			listidx++;
			factor(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 4);
		}
		else if (list[listidx].type == divsym)
		{
			listidx++;
			factor(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 5);
		}
		else
		{
			listidx++;
			factor(list);
			if (halt == 1)
			{
				return;
			}
			emit(2, 0, 7);
		}
	}
}

void factor(lexeme *list)
{
	// This function checks the syntax of identifiers and parentheses to make sure that it is correct,
	// and returns errors if they are not.
	int symidx_const, symidx_var;

	if (list[listidx].type == identsym)
	{
		symidx_var = find_symbol(list, 2);
		symidx_const = find_symbol(list, 1);

		if (symidx_var == -1 && symidx_const == -1)
		{
				// If our symbol is neither a number, identifier, nor a (, we print error #11.
				if (find_symbol(list, 3) != -1)
				{
					printparseerror(11);
					halt = 1;
					return;
				}
				// If our symbol cannot be returned unmarked, we print error number #19.
				else
				{
					printparseerror(19);
					halt = 1;
					return;
				}
		}

		if (symidx_var == -1)
		{
			emit(1, 0, table[symidx_const].val);
		}
		else if (symidx_const == -1 || table[symidx_var].level > table[symidx_const].level)
		{
			emit(3, level - table[symidx_var].level, table[symidx_var].addr);
		}
		else
		{
			emit(1, 0, table[symidx_const].val);
		}
		listidx++;
	}
	else if (list[listidx].type == numbersym)
	{
		emit(1, 0, list[listidx].value);
		listidx++;
	}
	else if (list[listidx].type == lparensym)
	{
		listidx++;
		expression(list);
		if (halt == 1)
		{
			return;
		}
		// If we return from expression without a ), we print error #12.
		if (list[listidx].type != rparensym)
		{
			printparseerror(12);
			halt = 1;
			return;
		}
		listidx++;
	}
	// Or also if our identifier is found but if it's a procedure, we also print error #11.
	else
	{
		printparseerror(11);
		halt = 1;
		return;
	}
}

int mult_declaration(lexeme *list)
{
	// This function searches through the symbol table to see if the name we are looking for is
	// already in the table, and if it is on the same level and is unmarked, we return that level.
	// Otherwise, we return -1.
	int i;
	for (i = 0; i <= tIndex; i++)
	{
		if (strcmp(table[i].name, list[listidx].name) == 0)
		{
			if (table[i].mark == 0)
			{
				if (table[i].level == level)
				{
					return i;
				}
			}
		}
	}

	return -1;
}

int find_symbol(lexeme *list, int kind)
{
	// This function searches through the symbol table to search for the symbol and only natches if
	// the entry has the same name, kind value, and is unmarked, returning the maximum level if it is
	// allowed, otherwise returning -1. To maximize, we start from the end of the table.
	int i;
	for (i = tIndex; i >= 0; i--)
	{
		if (strcmp(table[i].name, list[listidx].name) == 0 && table[i].kind == kind &&
				table[i].mark == 0)
		{
			return i;
		}
	}
	return -1;
}

void mark(lexeme *list)
{
	// This function starts from the back of the table and then makes it's way to the front, looking
	// at the entry's level and marks it if it is equal to the current level, it stops when it
	// finds an unmarked entry whose level is less than the current one.
	int i;
	for (i = tIndex; i >= 0; i--)
	{
		if (table[i].level == level)
		{
			table[i].mark = 1;
		}
	}
}

instruction *parse(lexeme *list, int printTable, int printCode)
{
	// Our main function, which will call upon program() and start our parsing process, proceeding
	// to printing our table and code if asked and finally ending our program.

	// Initialize our indices and level to 0.
	cIndex = 0, level = 0, listidx = 0, tIndex = 0;

	code = (instruction*)malloc(MAX_CODE_LENGTH * sizeof(instruction));
	table = (symbol*)malloc(MAX_SYMBOL_COUNT * sizeof(symbol));

	program(list);

	if (halt == 1)
	{
		return NULL;
	}

	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();

	code[cIndex].opcode = -1;
	return code;
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: when must be followed by do\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within do-od must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: do must be followed by od\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
