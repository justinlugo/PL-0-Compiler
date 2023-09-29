// Justin Lugo, Nuridean Mubarak
// COP 3402, Fall 2021
// 19 November 2021

// While we wrote our owns, we copied them over into the skeleton just to make sure.

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include <stdbool.h>
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5


lexeme *list;
int idx, lex_index;

void printlexerror(int type);
void printtokens();

// Boolean function to determine whether or not the provided index i is a comment or not.
bool isComment(char *string, int i)
{
  // If "//" or "/*" are our characters in the first two indices at i passed in, return true, else
  // we do not have a comment.
  if (string[i] == '/' && string[i + 1] == '/')
  {
    return true;
  }
  return false;
}

// Should we run into a comment, we have a function to essentially skip over the comment and move
// the index to its end.
int skipComment(char *string, int i)
{
  int k;
  if (isComment(string, i))
  {
    k = i + 2; // Add 2 to i, as // represents two occupied spaces within our array.
    // Loop through the string to find the comment's new line character.
    while (string[k] != '\n')
    {
      k++;
    }
    // Our string index has moved to "k", as that is the index where the comment ends.
    return k;
  }
  // If we got this far, we return the i value passed in as there is no comment.
  return i;
}

// We are going to delete comments and control characters from our original string array, and then
// copy everything else over into a new array for us to parse through later on.
char *deleteComments(char *string)
{

  // If the string is NULL, then there's nothing for us to really do, so we return NULL.
  if (string == NULL)
  {
    return NULL;
  }
  int i = 0, j = 0;
  char *newString = (char*)malloc(MAX_NUMBER_TOKENS);

  // Until the string reaches it's end, we go through and delete the comments placed within it
  // through out helper functions.
  while (string[i] != '\0')
  {
    if (isComment(string, i))
    {
      i = skipComment(string, i) + 1;
    }

    // If our string is at a newline token, mark it as a space within our new string.
    if (string[i] == '\n')
    {
      newString[j] = ' ';
      i++;
      j++;
    }

    // Since we replaced newline tokens with spaces, we can now delete the spaces we implemented
    // within our new array to get it into a single string.
    if (iscntrl(string[i]))
    {
      if (isComment(string, i + 1))
      {
        i = skipComment(string, i + 1) + 1;
      }
      i++;
      newString[j] = ' ';
      j++;
    }

    // If there are no spaces or control characters within our original string, we now move all of
    // our original string's contents into a new string that will not have any whitespace, control
    // characters nor comments, making it easier to parse in our lexanalyzer function.
    if (!iscntrl(string[i]))
    {
      newString[j] = string[i];
      i++;
      j++;
    }
  }
  // Add a terminating character at the end of the string to signal our parsing that this is the
  // end.
  newString[j + 1] = '\0';
  return newString;
}

// Taking in the string we created in deleteComments(), we're going to go through it until we reach
// a terminating character, a semicolon, or whitespace.
char *newString(char *string)
{
  char *nextString = (char*)malloc((MAX_IDENT_LEN + 1) * sizeof(char));
  int newIDX = 0;

  // If there is a terminating character at the index, we return it because that signifies the end
  // of the string.
  if (string[idx] == '\0')
  {
    return "\0";
  }

  // If there is a semicolon, we return said semicolon because that signifies the end of the word.
  if (string[idx] == ';')
  {
    return ";";
  }

  // If there's whitespace within the index, we keep incrementing until we get out of it.
  if (string[idx] == ' ')
  {
    while (string[idx] == ' ')
    {
      idx++;
    }
  }

  // This while loop loops through the string and updates the index value each time until we reach
  // any of the tokens above, in which case we mark that idx to signify the end of our words within
  // the string.
  while (isdigit(string[idx]) || isalpha(string[idx]))
  {
    nextString[newIDX] = string[idx];
    idx++;
    newIDX++;
  }

  // Need to subtract one index increment because once we reach something that is not a letter or
  // number, it still increments it, thus the need to remove it.
  idx--;
  return nextString;
}

lexeme *lexanalyzer(char *input, int printFlag)
{
	list = malloc(MAX_NUMBER_TOKENS * sizeof(lexeme));
	char *string = deleteComments(input);
  int strLength = strlen(string);
  int count, i, j;
  int halt = 0;
  idx = 0;

  // While our list holds less than 500 tokens, the string doesn't end, we have no errors, and
  // the index is less than the length of the original string passed in, we are going to compare
  // it to the various tokens to determine which type or value to associate to said token.
  while ((lex_index < MAX_NUMBER_TOKENS) && (string[idx] != '\0') && (halt == 0) &&
    (idx < strLength))
  {
    // We start off by checking to see if what is being held in the index is a token, and if so,
    // set the list to it's corresponding type, name, and value.
    switch(string[idx])
    {
      case ' ':
        break;

      case ':':
        if (string[idx + 1] == '=')
        {
          list[lex_index].type = assignsym;
          strcpy(list[lex_index].name, "assignsym");
          list[lex_index].value = 14;
          idx++;
        }
        else
        {
          halt = 1;
        }
        lex_index++;
        break;

      case '+':
        list[lex_index].type = addsym;
        strcpy(list[lex_index].name, "addsym");
        list[lex_index].value = 15;
        lex_index++;
        break;

      case '-':
        list[lex_index].type = subsym;
        strcpy(list[lex_index].name, "subsym");
        list[lex_index].value = 16;
        lex_index++;
        break;

      case '*':
        list[lex_index].type = multsym;
        strcpy(list[lex_index].name, "multsym");
        list[lex_index].value = 17;
        lex_index++;
        break;

      case '/':
        list[lex_index].type = divsym;
        strcpy(list[lex_index].name, "divsym");
        list[lex_index].value = 18;
        lex_index++;
        break;

      case '%':
        list[lex_index].type = modsym;
        strcpy(list[lex_index].name, "modsym");
        list[lex_index].value = 19;
        lex_index++;
        break;

      case '=':
        list[lex_index].type = eqlsym;
        strcpy(list[lex_index].name, "eqlsym");
        list[lex_index].value = 20;
        lex_index++;
        break;

      case '!':
        if (string[idx + 1] == '=')
        {
          list[lex_index].type = neqsym;
          strcpy(list[lex_index].name, "neqsym");
          list[lex_index].value = 21;
          idx++;
        }
        else
        {
          halt = 1;
        }
        lex_index++;
        break;


      case '<':
        if (string[idx + 1] == '=')
        {
          list[lex_index].type = leqsym;
          strcpy(list[lex_index].name, "leqsym");
          list[lex_index].value = 23;
          idx++;
        }
        else
        {
          list[lex_index].type = lsssym;
          strcpy(list[lex_index].name, "lsssym");
          list[lex_index].value = 22;
        }
        lex_index++;
        break;

      case '>':
        if (string[idx + 1] == '=')
        {
          list[lex_index].type = geqsym;
          strcpy(list[lex_index].name, "geqsym");
          list[lex_index].value = 25;
          idx++;
        }
        else
        {
          list[lex_index].type = gtrsym;
          strcpy(list[lex_index].name, "gtrsym");
          list[lex_index].value = 24;
        }
        lex_index++;
        break;

      case '(':
        list[lex_index].type = lparensym;
        strcpy(list[lex_index].name, "lparensym");
        list[lex_index].value = 27;
        lex_index++;
        break;

      case ')':
        list[lex_index].type = rparensym;
        strcpy(list[lex_index].name, "rparensym");
        list[lex_index].value = 28;
        lex_index++;
        break;

      case ',':
        list[lex_index].type = commasym;
        strcpy(list[lex_index].name, "commasym");
        list[lex_index].value = 29;
        lex_index++;
        break;

      case '.':
        list[lex_index].type = periodsym;
        strcpy(list[lex_index].name, "periodsym");
        list[lex_index].value = 30;
        lex_index++;
        break;

      case ';':
        list[lex_index].type = semicolonsym;
        strcpy(list[lex_index].name, "semicolonsym");
        list[lex_index].value = 31;
        lex_index++;
        break;

      default:
        break;
    }

    // If we've gotten this far, that means that our string is not holding a character token, so we
    // are going to check to see if it is a number or word, in which case we continue to set it to
    // its corresponding type, name, and value within the list.
    if (isdigit(string[idx]) || isalpha(string[idx]))
    {
      char *nextString = newString(string);

      if (!strcmp(nextString, "const"))
      {
        list[lex_index].type = constsym;
        strcpy(list[lex_index].name, "const");
        list[lex_index].value = 1;
        lex_index++;
      }

      else if (!strcmp(nextString, "var"))
      {
        list[lex_index].type = varsym;
        strcpy(list[lex_index].name, "var");
        list[lex_index].value = 2;
        lex_index++;
      }

      else if (!strcmp(nextString, "procedure"))
      {
        list[lex_index].type = procsym;
        strcpy(list[lex_index].name, "procedure");
        list[lex_index].value = 3;
        lex_index++;
      }

      else if (!strcmp(nextString, "do"))
      {
        list[lex_index].type = dosym;
        strcpy(list[lex_index].name, "do");
        list[lex_index].value = 4;
        lex_index++;
      }

      else if (!strcmp(nextString, "od"))
      {
        list[lex_index].type = odsym;
        strcpy(list[lex_index].name, "od");
        list[lex_index].value = 5;
        lex_index++;
      }

      else if (!strcmp(nextString, "while"))
      {
        list[lex_index].type = whilesym;
        strcpy(list[lex_index].name, "while");
        list[lex_index].value = 6;
        lex_index++;
      }

      else if (!strcmp(nextString, "when"))
      {
        list[lex_index].type = whensym;
        strcpy(list[lex_index].name, "when");
        list[lex_index].value = 7;
        lex_index++;
      }

      else if (!strcmp(nextString, "elsedo"))
      {
        list[lex_index].type = elsedosym;
        strcpy(list[lex_index].name, "elsedo");
        list[lex_index].value = 8;
        lex_index++;
      }

      else if (!strcmp(nextString, "call"))
      {
        list[lex_index].type = callsym;
        strcpy(list[lex_index].name, "call");
        list[lex_index].value = 9;
        lex_index++;
      }

      else if (!strcmp(nextString, "write"))
      {
        list[lex_index].type = writesym;
        strcpy(list[lex_index].name, "write");
        list[lex_index].value = 10;
        lex_index++;
      }

      else if (!strcmp(nextString, "read"))
      {
        list[lex_index].type = readsym;
        strcpy(list[lex_index].name, "read");
        list[lex_index].value = 11;
        lex_index++;
      }

      else if (!strcmp(nextString, "odd"))
      {
        list[lex_index].type = oddsym;
        strcpy(list[lex_index].name, "odd");
        list[lex_index].value = 26;
        lex_index++;
      }

      // If our string is not letters nor character tokens, that means it is numbers.
      else if (isdigit(nextString[0]))
      {
        count = 0;

        // Iterate through our string and make sure that it is all numbers. If it is not, we mark it
        // and break. If it is, we count up the amount of consecutive integers to make sure that
        // it is less than the maximum number allowed.
        for (i = 0; i < strlen(nextString); i++)
        {
          if (isalpha(nextString[i]))
          {
            halt = 2;
            break;
          }
        }

        // If our string of numbers is greater than the maximum allowed (5), we return an error.
        for (j = 1; j < strlen(nextString); j++)
        {
          if (isdigit(nextString[j]))
          {
            count++;
          }
          if (count > 5)
          {
            halt = 3;
            break;
          }
        }

        // If the number of integers is less than our max allowed, we are good to go and we store
        // it at the proper token.
        if (count <= MAX_NUMBER_LEN)
        {
          list[lex_index].type = numbersym;
          strcpy(list[lex_index].name, "number");
          list[lex_index].value = atoi(nextString);
          lex_index++;
        }
      }

      // If we've gotten this far, that means that our string is an identifier, and it cannot be
      // greater than 11 characters, so we check for that, returning an error if it is longer than
      // the max identifier length.
      else
      {
        if (strlen(nextString) >= 11)
        {
          halt = 4;
          break;
        }

        list[lex_index].type = identsym;
        strcpy(list[lex_index].name, nextString);
        list[lex_index].value = 12;
        lex_index++;
      }
    }
    idx++;
  }

  // Last minute check to see if we have a halt flag or not, if we do, we print the corresponding
  // error. If we don't, then we continue to the end and print out our tokens.
  if (halt != 0)
  {
    printlexerror(halt);
    return NULL;
  }

	if (printFlag)
		printtokens();
	list[lex_index++].type = -1;
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "=", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case odsym:
				printf("%11s\t%d", "od", odsym);
				break;
			case whensym:
				printf("%11s\t%d", "when", whensym);
				break;
			case elsedosym:
				printf("%11s\t%d", "elsedo", elsedosym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
