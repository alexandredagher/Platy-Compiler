/*
File name : buffer.h
Compiler : MS Visual Studio 2015
Author : Alexandre Dagher 040868750
Course : CST 8152 – Compilers, Lab Section : 11
Assignment : 1
Date : October 17 2019
Professor : Sv.Ranev
Purpose : Creating a Buffer, to read in files with the chose of different operational modes ( muiltiplecative, self incrementing, fixed)
Function list : none
*/
#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

							/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

														   /* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

														   /* constant definitions */
#define RT_FAIL_1 -1         /* fail return value */
#define RT_FAIL_2 -2         /* fail return value */
#define LOAD_FAIL -2       /* load fail return value */
#define TRUE	   1		/*SUCCESS VALUE*/
#define	FALSE      0		/*FAIL VALUE*/


#define FIXED_MODE 0       /*fixed size mode*/
#define ADDITIVE_MODE 1    /*Additive seld incrementing mode*/
#define MULTI_MODE -1      /*Mulyiplicative self incrementing mode*/

														   /* You should add your own constant definitions here *
														   /* Enter your bit-masks constant definitions here */
#define DEFAULT_FALGS  0xFFFC /*default flags value*/
#define SET_EOB        0x0001   /*set eob mask*/
#define RESET_EOB      0xFFFE   /*reset eob mask*/
#define CHECK_EOB      0x0001   /*check eob mask*/
#define SET_R_FLAG     0x0002   /*set r_flag mask*/
#define RESET_R_FLAG   0xFFFD   /*reset r_flag mask*/
#define CHECK_R_FLAG   0x0002   /*check r_flag mask*/


#define MAX_SHORT_M_ONE	(SHRT_MAX-1)   /* MAX SHORT - 1 */

														   /* user data type declarations */
typedef struct BufferDescriptor {
	char *cb_head;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor; /* character array increment factor */
	char  mode;       /* operational mode indicator*/
	unsigned short flags; /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, *pBuffer;


/* function declarations */
extern Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode);
extern pBuffer b_addc(pBuffer const pBD, char symbol);
extern int b_clear(Buffer * const pBD);
extern void b_free(Buffer * const pBD);
extern int b_isfull(Buffer * const pBD);
extern short b_limit(Buffer * const pBD);
extern short b_capacity(Buffer * const pBD);
extern short b_mark(pBuffer const pBD, short mark);
extern int b_mode(Buffer * const pBD);
extern size_t b_incfactor(Buffer * const pBD);
extern int b_load(FILE * const fi, Buffer * const pBD);
extern int b_isempty(Buffer * const pBD);
extern char b_getc(Buffer * const pBD);
extern int b_eob(Buffer * const pBD);
extern int b_print(Buffer * const pBD);
extern Buffer * b_compact(Buffer * const pBD, char symbol);
extern char b_rflag(Buffer * const pBD);
extern short b_retract(Buffer * const pBD);
extern short b_reset(Buffer * const pBD);
extern short b_getcoffset(Buffer * const pBD);
extern int b_rewind(Buffer * const pBD);
extern char * b_location(Buffer * const pBD, short loc_offset);

#endif
