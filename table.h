/* File name : table.c
Compiler : GCC
Author : Alexandre Dagher 040868750
Course : CST 8152 – Compilers, Lab Section 11
Assignment 2
Date : April 28th 2019
Professor : Sv.Ranev
Purpose : defineing transition table, defines, accpeting or none accepting states, error states
Function list: none
*/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/*   Source end-of-file (SEOF) sentinel symbol
*    '\0' or one of 255,0xFF,EOF
*/
#define SEOF '\0'
#define NEW_LINE '\n'
#define C_RETURN '\r'


#define ES 11 /* Error state  with no retract */
#define ER 12 /* Error state  with retract */
#define IS -1    /* Inavalid state */


#define TABLE_COLUMNS 8
/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
	/* State 0 */{ 1, 6, 4, ES, ES, 9, ER, ES },
	/* State 1 */{ 1, 1, 1, 2, 3, 2, 2, 2 },
	/* State 2 */{ IS, IS, IS, IS, IS, IS, IS, IS },
	/* State 3 */{ IS, IS, IS, IS, IS, IS, IS, IS },
	/* State 4 */{ ES, 4, 4, 7, 5, 5, 5, 5 },
	/* State 5 */{ IS, IS, IS, IS, IS, IS, IS, IS },
	/* State 6 */{ ES, 6, ES, 7, 5, 5, 5, 5 },
	/* State 7 */{ 8, 7, 7, 8, 8, 8, 8, 8 },
	/* State 8 */{ IS, IS, IS, IS, IS, IS, IS, IS },
	/* State 9 */{ 9, 9, 9, 9, 9, 10, ER, 9 },
	/* State 10 */{ IS, IS, IS, IS, IS, IS, IS, IS },
	/* State 11 */{ IS, IS, IS, IS, IS, IS, IS, IS },
	/* State 12 */{ IS, IS, IS, IS, IS, IS, IS, IS },
};

/* Accepting state table definition */
#define ASWR     0  /* accepting state with retract */
#define ASNR     1  /* accepting state with no retract */
#define NOAS     2  /* not accepting state */

int as_table[] = {
	/* State 0 */	NOAS,
	/* State 1 */	NOAS,
	/* State 2 */	ASWR,
	/* State 3 */	ASNR,
	/* State 4 */	NOAS,
	/* State 5 */	ASWR,
	/* State 6 */	NOAS,
	/* State 7 */	NOAS,
	/* State 8 */	ASWR,
	/* State 9 */	NOAS,
	/* State 10 */	ASNR,
	/* State 11 */	ASNR,
	/* State 12 */	ASWR
};

/* Accepting action function declarations */
Token aa_func02(char* lexeme); /* AVID/KW */
Token aa_func03(char* lexeme); /* SVID */
Token aa_func05(char* lexeme); /* IL */
Token aa_func08(char* lexeme); /* FPL */
Token aa_func10(char* lexeme); /* SL */
Token aa_func12(char* lexeme); /* ERROR TOKEN  */



							   /* defining a new type: pointer to function (of one char * argument)
							   returning Token
							   */
typedef Token(*PTR_AAF)(char *lexeme);


PTR_AAF aa_table[] = {

	/* State 0 */ NULL,
	/* State 1 */ NULL,
	/* State 2 */ aa_func02,
	/* State 3 */ aa_func03,
	/* State 4 */ NULL,
	/* State 5 */ aa_func05,
	/* State 6 */ NULL,
	/* State 7 */ NULL,
	/* State 8 */ aa_func08,
	/* State 9 */ NULL,
	/* State 10 */ aa_func10,
	/* State 11 */ aa_func12,
	/* State 12 */ aa_func12

};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  10

char * kw_table[] =
{
	"ELSE",
	"FALSE",
	"IF",
	"PLATYPUS",
	"READ",
	"REPEAT",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

#endif
