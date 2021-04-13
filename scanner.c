/* File name : scanner.c
Compiler : GCC
Author : Alexandre Dagher 040868750
Course : CST 8152 – Compilers, Lab Section 11
Assignment 2
Date : April 28th 2019
Professor : Sv.Ranev
Purpose : Matching lexemes to create tokens, creating and calling appropriate accepting/error statements
Function list : char_class(), get_next_state(), iskeyword(), scanner_init(), malar_next_token(), get_next_state(), char_class(), aa_func02(), aa_func03(), aa_func05(), aa_func08(), aa_func10(), aa_func12()
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not hve any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/
static pBuffer sc_buf; /*pointer to input source buffer*/
					   /* No other global variable declarations/definitiond are allowed */

					   /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */

										/*
Purpose: Initializes scanner
Author : Sv. Ranev
History / Versions : [version numbers and dates]
Called functions : b_rewind(), b_clear()
Parameters : Buffer* psc_buf
Return value : int
Algorithm : Getting the buffer ready to be read
*/
int scanner_init(Buffer * psc_buf) {
	if (b_isempty(psc_buf)) return EXIT_FAILURE;/*1*/
												/* in case the buffer has been read previously  */
	b_rewind(psc_buf);
	b_clear(str_LTBL);
	line = 1;
	sc_buf = psc_buf;
	return EXIT_SUCCESS;/*0*/
						/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/*
Purpose: Reading the next lexeme to match and then creating the appropriate token and call the accepting function if necesary
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : aa_func02(), aa_func03(), aa_func05(), aa_func08(), aa_func10(), aa_func12(), isspace(), b_getc(), isalpha(), isdigit(), get_next_state(),  b_mark(), b_addc(), free(), b_getcoffset()
Parameters : void
Return value : Token
Algorithm : Create token, call accepting state if needed
*/
Token malar_next_token(void) {
	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/
	int accept = NOAS; /* type of state - initially not accepting */
	char temp_c;
	int i = 0;


	while (1) { /* endless loop broken by token returns it will generate a warning */


		/*Getting next symbol from buffer*/
		c = b_getc(sc_buf);

		/* Part 1: Implementation of token driven scanner */
		/* every token is possessed by its own dedicated code */


		if (isspace(c)) {  /* isspace() checks for ' ', '\t', '\v', '\n', '\f', '\r' */
			if (c == '\n')
				++line; /* if its a new line charactor we must incriments line number*/
			continue;
		}

		/* Check whether lexeme is a digit or number if so create buffer to store whole lexeme  */
		if (isalpha(c) || isdigit(c)) {

			/*Place a mark where the lexeme starts so we can pick it up later*/
			lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);
			/*Get next state to see where we are on the transition table if its a accpting state or not*/
			state = get_next_state(state, c, &accept);

			/*Keep pulling lexemes untill we hit a accepting state on the transition table*/
			while (accept == NOAS) {
				c = b_getc(sc_buf);
				state = get_next_state(state, c, &accept);
			}

			/*If you hit a accepting state with retract do so*/
			if (accept == ASWR) {
				b_retract(sc_buf);
			}

			/*place the marker where the end of the lexeme i*/
			lexend = b_getcoffset(sc_buf);

			/*create a temp buffer to store the whole lexeme*/
			lex_buf = b_allocate(lexend - lexstart, 0, 'f');


			/*if the buffer cannot be created throw a error*/
			if (!lex_buf) {
				scerrnum = 1;
				t.code = ERR_T;
				strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
				return t;
			}

			/*reset the offset back to the mark*/
			b_reset(sc_buf);

			/*copy the buffer to the temp buffer*/
			while (b_getcoffset(sc_buf) < lexend) {
				c = b_getc(sc_buf);
				b_addc(lex_buf, c);
			}

			/*add the eof symbol and shrink the buffer to smallest size*/
			b_compact(lex_buf, '\0');

			/*Add pointer to aa_table */
			t = aa_table[state](lex_buf->cb_head);
			b_free(lex_buf);
			return t;
		}

		switch (c)
		{
		case '=':
			/* Have to check if the next symbol is "=" to set appropriate token */
			if (b_getc(sc_buf) == '=') {
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}

			/* If it is not '=' retract to return taken lexeme*/
			b_retract(sc_buf);
			t.code = ASS_OP_T;
			return t;
		case '(':
			t.code = LPR_T;
			return t;
		case ')':
			t.code = RPR_T;
			return t;
		case '{':
			t.code = LBR_T;
			return t;
		case '}':
			t.code = RBR_T;
			return t;
		case '<':

			/*Get next symbol so we can see which token we have to make*/
			temp_c = b_getc(sc_buf);

			/*Assigning token depending on second lexeme*/
			if (temp_c == '>') {
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			else if (temp_c == '<') {
				t.code = SCC_OP_T;
				return t;
			}

			/* Put the second symbol back if it was niether < > and parse the correct */
			b_retract(sc_buf);
			t.code = REL_OP_T;
			t.attribute.rel_op = LT;
			return t;
		case '>':
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		case ';':
			t.code = EOS_T;
			return t;
		case '!':

			c = b_getc(sc_buf);

			/* In the case of two consecutive '!' create a comment token */
			if (c == '!') {
				/* Consume all following characters from buffer for the comment */
				while (c != '\n' && c != SEOF && c != 255 && c != '\r') {
					c = b_getc(sc_buf);
				}
				line++;
				continue;
			}
			else {
				/* If second '!' is absent, generate an error token */
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;
				t.attribute.err_lex[2] = '\0';

				/* Eat up the garbage after the wrong comment */
				while (1) {
					c = b_getc(sc_buf);
					if (c == NEW_LINE || c == SEOF || c == 255 || c == C_RETURN) {
						break;
					}
				}
				line++;
			}
			return t;
		case ',':
			t.code = COM_T;
			return t;
		case '+':
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;
		case '-':
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;
		case '*':
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;
		case '/':
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;
		case '.':

			/* Check Logical Operators .AND and .OR*/
			b_mark(sc_buf, b_getcoffset(sc_buf));
			c = b_getc(sc_buf);

			/* if 'AND' or 'OR' or the wrong tokens */
			if (c == 'A' && b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' && b_getc(sc_buf) == '.') {
				t.code = LOG_OP_T;
				t.attribute.log_op = AND;
				return t;
			}
			else if (c == 'O' && b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') {
				t.code = LOG_OP_T;
				t.attribute.log_op = OR;
				return t;
			}
			/* Nothing in the language specs has anything to do with . and another other then AND. or OR. so its a error, floating points are already taken care of so create error token*/
			else {
				b_reset(sc_buf);
				t.code = ERR_T;
				t.attribute.err_lex[0] = '.';
				t.attribute.err_lex[1] = '\0';
				return t;
			}


		case '"':

			/*Starting point for our string literal*/
			b_mark(sc_buf, b_getcoffset(sc_buf));
			lexstart = b_getcoffset(sc_buf);

			while (1) {
				c = b_getc(sc_buf);

				/*End of our string literal*/
				if (c == '"') {
					/*Setting end point*/
					lexend = b_getcoffset(sc_buf);
					b_reset(sc_buf);

					/*Set the next offset for a value for the string*/
					t.attribute.str_offset = b_limit(str_LTBL);

					/*Add the string in the string literal table*/
					for (i = lexstart; i < (lexend - 1); i++) {
						c = b_getc(sc_buf);
						b_addc(str_LTBL, c);
					}

					b_getc(sc_buf);

					/*Add the SEOF symbol */
					b_addc(str_LTBL, '\0');
					t.code = STR_T;

					return t;
				}
				else if (c == SEOF || c == 255 || c == EOF) {

					/*if we hit end of file we have to make a error token before EOFT so bring it back*/
					b_reset(sc_buf);
					b_retract(sc_buf);

					/*Put as much as we can into the err_lex attribute*/
					for (i = 0; i < (ERR_LEN - 3); i++) {
						c = b_getc(sc_buf);

						if (c == NEW_LINE || c == 255 || c == SEOF) {
							break;
						}

						t.attribute.err_lex[i] = c;
					}

					/*Put as much as we can into the err_lex attribute but add ... to show there is actually more and then add SEOF for the buffer*/
					t.code = ERR_T;
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '\0';

					c = b_getc(sc_buf);

					/*Read up the rest of the comment that failed*/
					while (c != SEOF && c != 255 && c != EOF) {
						c = b_getc(sc_buf);
					}

					return t;
				}
				else if (c == '\n') {
					line++;
				}
			}
		case SEOF:
		case EOF:
			t.code = SEOF_T;
			return t;

		default:

			/*Used for any charaters that are not in the grammer, create a error token and put the charater in the err_lex buffer*/
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
		}
	}
}

int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:
	Assertion failed: test, file filename, line linenum
	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}

/*
Purpose: show we row are on the transition table
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : none
Parameters : char
Return value : int
Algorithm : check char then set value in the transition table
*/
int char_class(char c)
{
	int val;

	if (isalpha(c)) {
		val = 0;
	}
	else if (isalnum(c)) {
		if (c == '0') {
			val = 1;
		}
		else {
			val = 2;
		}
	}
	else if (c == '.') {
		val = 3;
	}
	else if (c == '@') {
		val = 4;
	}
	else if (c == '""') {
		val = 5;
	}
	else if (c == SEOF || c == 255) {
		val = 6;
	}
	else {
		val = 7;
	}

	/*THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 2 REPRESENTS[A - Za - z]
	THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
	OF THE LETTERS A, B, ..., Z, a, b...z.
	PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)*/

	return val;
}

/*
Purpose: Function for the arithmentic variable identifier AND keywords(VID - AVID / KW)
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : iskeyword(), strlen(),
Parameters : char*
Return value : Token
Algorithm : check char then set value in the transition table
*/
Token aa_func02(char* lexeme) {

	Token t;
	int i;
	int num = iskeyword(lexeme);

	/* To check if the lexeme is a keyword */
	if (num != -1) {
		t.code = KW_T;
		t.attribute.kwt_idx = num;
		return t;
	}

	t.code = AVID_T;

	/* check if the length is longer than VID_LEN */
	if (strlen(lexeme) > VID_LEN) {

		/* store the character into variable array */
		for (i = 0; i < VID_LEN; ++i) {
			t.attribute.vid_lex[i] = lexeme[i];
		}

		/*add SOEF to the array if it cant fit*/
		t.attribute.vid_lex[VID_LEN] = '\0';
	}
	else {
		/*Else just copy it over cause it will fit*/
		strcpy(t.attribute.vid_lex, lexeme);
	}

	return t;
}

/*
Purpose: Function for the string variable identifier(VID - SVID
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : strlen()
Parameters : char*
Return value : Token
Algorithm : create the VID token
*/
Token aa_func03(char* lexeme) {

	Token t;
	int lexlength = strlen(lexeme);
	int i = 0;

	t.code = SVID_T;

	/* check if lexeme length is longer than VID_LEN */
	if (lexlength > VID_LEN) {

		/* to store the character into variable array */
		while (i < lexlength && i < VID_LEN - 1) {
			t.attribute.vid_lex[i] = lexeme[i];
			i++;
		}

		/* Add the variable identifier symbol to the end */
		t.attribute.vid_lex[i++] = '@';
		t.attribute.vid_lex[i] = '\0';
	}
	else {
		while (i < lexlength) {
			t.attribute.vid_lex[i] = lexeme[i];
			i++;
		}

		t.attribute.vid_lex[i] = '\0';
	}


	return t;
}

/*
Purpose: Function for the floating point literal
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : atof(), strlen();
Parameters : char*
Return value : Token
Algorithm : create the VID token
*/
Token aa_func08(char* lexeme) {

	Token t;
	double floatValue = (double)atof(lexeme);
	int lexlength = strlen(lexeme);

	/*If its in range copy it over and return the token*/
	if ((floatValue >= FLT_MIN && floatValue <= FLT_MAX) || floatValue == 0) {
		t.code = FPL_T;
		t.attribute.flt_value = (float)floatValue;
		return t;
	}

	/*If its to big or small create a error token for it*/
	t = aa_table[12](lexeme);
	t.code = ERR_T;

	/*If its to big we have to append the ...*/
	if (lexlength > ERR_LEN) {
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
		t.attribute.err_lex[ERR_LEN - 3] = '.';
		t.attribute.err_lex[ERR_LEN - 2] = '.';
		t.attribute.err_lex[ERR_LEN - 1] = '.';
		t.attribute.err_lex[ERR_LEN] = '\0';
	}
	else {
		/*Just copy it over cause it can fit*/
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
	}
	return t;
}

/*
Purpose: Function for the integer literal(IL)-decimal constant(DIL)
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : strlen(), atoi()
Parameters : char*
Return value : Token
Algorithm : create the ITL token
*/
Token aa_func05(char* lexeme) {

	Token t;
	int intValue = 0; /* Integer value of the lexeme char. */
	int lexlength = strlen(lexeme);

	intValue = atoi(lexeme);

	/* If it can fit set token*/
	if ((intValue >= SHRT_MIN && intValue <= SHRT_MAX) || intValue == 0) {
		t.code = INL_T;
		t.attribute.int_value = intValue;
	}
	else {

		/*create the error token*/
		t.code = ERR_T;
		t = aa_table[12](lexeme);

		/*If its to big we have to append the ...*/
		if (lexlength > ERR_LEN) {
			strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
			t.attribute.err_lex[ERR_LEN] = '\0';
			t.attribute.err_lex[ERR_LEN - 1] = '.';
			t.attribute.err_lex[ERR_LEN - 2] = '.';
			t.attribute.err_lex[ERR_LEN - 3] = '.';
		}
		else {
			strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
		}
	}
	return t;
}

/*
Purpose: Function for the string literals
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : strlen(), atoi()
Parameters : char*
Return value : Token
Algorithm : create the string literals token
*/
Token aa_func10(char* lexeme) {
	Token t;
	int i = 0;
	int len = 0;
	/*get the length of the lexeme*/
	len = strlen(lexeme) - 1;

	/*add the next index spot for a character*/
	t.attribute.str_offset = b_limit(str_LTBL);

	/*add the lexemes into the str_LTBL table*/
	for (i = 1; i < len; ++i) {
		if (lexeme[i] == '\n')
			++line;
		b_addc(str_LTBL, lexeme[i]);
	}

	/*append string terminator*/
	b_addc(str_LTBL, '\0');
	t.code = STR_T;
	return t;
}


/*
Purpose: Function for error tokens
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : strlen()
Parameters : char*
Return value : Token
Algorithm : create error token
*/
Token aa_func12(char* lexeme) {

	Token t;
	int length = strlen(lexeme);

	t.code = ERR_T;

	/* if its to big copy it over and append appropriate symbols*/
	if (length > ERR_LEN) {
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN - 3);
		t.attribute.err_lex[ERR_LEN - 3] = '.';
		t.attribute.err_lex[ERR_LEN - 2] = '.';
		t.attribute.err_lex[ERR_LEN - 1] = '.';
		t.attribute.err_lex[ERR_LEN] = '\0';
	}
	else {
		/* copy over error because it can fit */
		strncpy(t.attribute.err_lex, lexeme, length);
		t.attribute.err_lex[length] = '\0';
	}

	return t;
}

/*
Purpose: Function checking if a lexeme is a keyword
Author : Alexandre Dagher
History / Versions : April 27 2019
Called functions : none
Parameters : char*
Return value : Token
Algorithm : check and return value describing true or false
*/
int iskeyword(char* key_lexeme) {

	/* used for incrementing*/
	int i;

	/* to check if lexeme is has nothing in it */
	if (key_lexeme == NULL)
		return -1;

	/* Go through the keyword array and try and make a match with one */
	for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(kw_table[i], key_lexeme) == 0) {
			return i;
		}
	}

	return -1;
}
