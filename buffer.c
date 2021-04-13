/*
File name : buffer.c
Compiler : MS Visual Studio 2015
Author : Alexandre Dagher 040868750
Course : CST 8152 – Compilers, Lab Section : 11
Assignment : 1
Date : October 18 2019
Professor : Sv.Ranev
Purpose : Creating a Buffer, to read in files with the chose of different operational modes ( muiltiplecative, self incrementing, fixed)
Function list : b_allocate(), b_addc(), b_clear(), b_free(), b_isfull(), b_limit(), b_capacity(), b_mark(),
b_mode(), b_incfactor(), b_load(), b_isempty(), b_getc(), b_eob(), b_print(), b_compact(), b_rflag(), b_retract(), b_reset()
b_getcoffset(), b_rewind(), b_location(),
*/
#include "buffer.h"

/*
Purpose: To allocate space on the heap for our buffer descriptor and our array of charactor (buffer)
also to properties to our buffer descriptor such as (mode, inc factor, initial compacity)
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions : free(), realloc(), calloc()
Parameters : short init_capacity (initial compacity of buffer) range: ( -32,768 to 32,767 )
char inc_factor (the incrementing factor for multiplicative and additive modes) range: ( 0 - 255)
char o_mode (defines which operational mode the buffer will work with) range (-1, 0, 1)
Algorithm : Check whether values are valid, if so allocate space in memory for the given values, if the
values are not good return, if the is no room for those values return, after creating the memory
input the correct values into the buffer descriptor that was allocted
*/
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode)
{
	/*pointer to the buffer we will allocate*/
	Buffer *tempBuffer = NULL;

	/*bool used to check whether values are correct before allocating memory*/
	char bool = 0;

	if (init_capacity <= 0 && o_mode == 'f') {
		return NULL;
	}

	if (o_mode == 'f') {
		bool = 1;
	}
	else if (o_mode == 'a' && ((unsigned char)inc_factor >= 0 && (unsigned char)inc_factor <= 255)) {
		bool = 1;
	}
	else if (o_mode == 'm' && ((unsigned char)inc_factor >= 0 && (unsigned char)inc_factor <= 100)) {
		bool = 1;
	}
	else
	{
		return NULL;
	}

	/*if values were good, try and now allocte space in memory*/
	if (bool = 1) {

		/*allocting buffer descriptor*/
		if ((tempBuffer = (Buffer*)calloc(1, sizeof(Buffer))) == NULL) {
			return NULL;
		}

		/*allocting char buffer*/
		else if (init_capacity >= 0 && init_capacity <= MAX_SHORT_M_ONE) {
			if ((tempBuffer->cb_head = (char*)malloc(init_capacity)) == NULL) {
				free(tempBuffer);
				return NULL;
			}
		}
		//		else {
		//		free(tempBuffer);
		//			return NULL;
		//		}

	}
	else {
		return NULL;
	}

	/*set the modes and incimental factors*/
	if (o_mode == 'a') {
		tempBuffer->mode = ADDITIVE_MODE;
		tempBuffer->inc_factor = (unsigned char)inc_factor;
	}
	else if (o_mode == 'm') {
		tempBuffer->mode = MULTI_MODE;
		tempBuffer->inc_factor = (unsigned char)inc_factor;
	}
	else {
		tempBuffer->mode = FIXED_MODE;
		tempBuffer->inc_factor = 0;
	}

	/*Assign the buffer descriptor its capacity if everything worked*/
	tempBuffer->capacity = init_capacity;
	/*set the defult flags*/
	tempBuffer->flags = DEFAULT_FALGS;
	return tempBuffer;
}

/*
Purpose: Add charactors in the buffer that is pointed by PBD
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  realloc()
Parameters : const Bufer* PBD (pointer to our buffer)
char sysmbol(symbol we would like to add to buffer) range: ( 0 - 255)
Algorithm : Check if the buffer is full, if not add it to the buffer, if it is full try allocting new
memory using one of the incremental modes, if it is not possible return null, if new space
can be created copy over the old elements in the array using realloc and then add the
symbol to the buffer, if anything fails return NULL
*/
pBuffer b_addc(pBuffer const pBD, char symbol)
{
	short newCapacity;
	short availableSpace;
	short newIncrement;


	if (!pBD) {
		return NULL;
	}

	/*check if buffer is not full, if not add the symbol to the buffer normaly*/
	if (b_isfull(pBD) == 0) {
		pBD->flags &= RESET_R_FLAG;
		pBD->cb_head[pBD->addc_offset] = symbol;
		pBD->addc_offset++;
		return pBD;
	}
	else {

		/*Else calculate the new size of the buffer depending on the mode*/

		if (pBD->mode == FIXED_MODE)
			return NULL;

		else if (pBD->mode == ADDITIVE_MODE) {

			if (pBD->inc_factor == 0) {
				return NULL;
			}

			newCapacity = pBD->capacity + (unsigned char)pBD->inc_factor;

			if (newCapacity < 0)
				return NULL;

			/*if the new capacity calculated is to big just assign it to the max potential buffer*/
			else if (newCapacity > MAX_SHORT_M_ONE) {
				pBD->capacity = MAX_SHORT_M_ONE;
			}

		}

		else if (pBD->mode == MULTI_MODE) {

			/*If the buffer is already its max return*/
			if (pBD->capacity == MAX_SHORT_M_ONE) {
				return NULL;
			}

			/*Error checking*/
			if (pBD->inc_factor == 0) {
				return NULL;
			}

			else {

				/*algonrithem for creating new buffer size using muiltiplicative mode*/
				availableSpace = MAX_SHORT_M_ONE - pBD->capacity;
				newIncrement = (short)(availableSpace * (unsigned char)pBD->inc_factor / 100);
				newCapacity = pBD->capacity + newIncrement;

				/*if there is no more space*/
				if (newIncrement == 0) {

					/*assign it to the max buffer*/
					if (pBD->capacity < MAX_SHORT_M_ONE) {
						newCapacity = MAX_SHORT_M_ONE;
					}
					else {
						return NULL;
					}
				}

			}
		}

		char *newBuffer = NULL;

		if (pBD->mode == ADDITIVE_MODE || pBD->mode == MULTI_MODE) {

			/*allocting the new buffer */
			if ((newBuffer = (char*)realloc(pBD->cb_head, newCapacity)) == NULL) {
				return NULL;
			}

			/*check if the realloc gave it a new position, if so set the r flag*/
			if (newBuffer != pBD->cb_head) {
				pBD->flags |= SET_R_FLAG;
			}

			/*if the realloc was succesfull, point the head to it and add the char*/
			pBD->cb_head = newBuffer;
			pBD->cb_head[pBD->addc_offset] = symbol;
			/*assign the buffer descriptor its new capacity*/
			pBD->capacity = newCapacity;

			/*make sure you not overrunning the buffer*/
			if (pBD->capacity > pBD->addc_offset) {
				pBD->addc_offset++;
			}
		}
	}

	return pBD;
}

/*
Purpose: reset buffer parameters to the start
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: Clear buffer perameters so that if a function was called to get or add chars to the buffer it
would start from the beggining
*/
int b_clear(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	pBD->addc_offset = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	pBD->flags = DEFAULT_FALGS;

	return TRUE;
}

/*
Purpose: free our buffer
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: free our buffer so there is no memory leak
*/
void b_free(Buffer * const pBD) {

	if (!pBD) {
		return;
	}

	free(pBD->cb_head);
	free(pBD);
}

/*
Purpose: check whether the buffer is full
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: if our buffer is full return 1, otherwise return 0
*/
int b_isfull(Buffer * const pBD) {

	if (!pBD)
		return RT_FAIL_1;

	else if ((short)(pBD->addc_offset * sizeof(char)) == pBD->capacity) {
		return TRUE;
	}

	return FALSE;
}

/*
Purpose: return offset
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: return our offset for printing purposes
*/
short b_limit(Buffer * const pBD) {

	if (!pBD)
		return RT_FAIL_1;

	return pBD->addc_offset;
}

/*
Purpose: return capacity
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: return our capacity for printing purposes
*/
short b_capacity(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	return pBD->capacity;
}

/*
Purpose: set markc_offset to the given mark
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
short mark (the location where the mark will be put)
Algorithm: used later on for the parser
*/
short b_mark(pBuffer const pBD, short mark)
{
	if (!pBD)
		return RT_FAIL_1;

	if (mark >= 0 && mark <= pBD->addc_offset) {
		pBD->markc_offset = mark;
		return pBD->markc_offset;
	}
	else
		return RT_FAIL_1;
}

/*
Purpose: return the mode
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: used to return the mode for printing purposes
*/
int b_mode(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_2;

	return pBD->mode;
}

/*
Purpose: return the inc factor
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: used to return the incrimental factor for printing purposes
*/
size_t b_incfactor(Buffer * const pBD)
{
	if (pBD == NULL) {
		return 0x100;
	}

	return (int)(unsigned char)pBD->inc_factor;
}

/*
Purpose: load file into buffers
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  feof(), b_addc
Parameters : const FILE* fi (pointer to the file object)
const Bufer* PBD (pointer to our buffer)
Algorithm: check if a object has actually been created if the file is found, then add charactors from
the file into our buffer keeping count of amount of chars stored
*/
int b_load(FILE * const fi, Buffer * const pBD)
{
	/*used as a temp buffer to check value and print to stdout*/
	char charactor;
	/*count of how many chars printed*/
	int numbOfChars = 0;

	if (fi == NULL || pBD == NULL)
		return RT_FAIL_1;

	/*run through buffer to get values from the file to be added to the buffer*/
	while (!feof(fi)) {
		charactor = (char)fgetc(fi);
		if (charactor != EOF) {
			/*if the buffer cannot add a char it will return and print what was the last element inputed*/
			if (b_addc(pBD, charactor) == NULL) {
				printf("The last character read from the file is: %c %d \n ", charactor, charactor);
				return LOAD_FAIL;
			}
			else {
				numbOfChars++;
			}
		}
	}

	return numbOfChars;
}

/*
Purpose: check if buffer is empty
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: check if the buffer is empty if so return 1, otherwise return 0
*/
int b_isempty(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	if (pBD->addc_offset == 0)
		return TRUE;

	return FALSE;
}
/*
Purpose: get a charator from the buffer
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: check if there is a charactor to get, if not set the end of file flag and return,
if not get the charator specified by getc and return that charactor
*/
char b_getc(Buffer * const pBD)
{
	/*used as a storage variable to return as a return vvalue*/
	char currentChar;

	if (!pBD)
		return RT_FAIL_2;

	/*if the EOB has been reached set the flag and return -1*/
	if (pBD->getc_offset == pBD->addc_offset) {
		pBD->flags |= SET_EOB;
		return FALSE;
	}
	else {
		/*reset relocation flag*/
		pBD->flags &= RESET_EOB;
		/*retrive charactor from buffer*/
		currentChar = *(pBD->cb_head + pBD->getc_offset);
		/*increment the offset to later retrive the next*/
		pBD->getc_offset++;
	}

	return currentChar;
}

/*
Purpose: check EOB flag
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: use bitwise operations to check the EOB flag
*/
int b_eob(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	return (int)pBD->flags & CHECK_EOB;
}

/*
Purpose: print the buffer to stdout
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  b_isempty(), b_getsc(),
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm: if the buffer is empty print message to stdout, otherwise call b_getc() to get a charator from
the buffer, print the charator to stdout and keep check if you have hit EOF, keep a count
of the number of chars printed and return this number
*/
int b_print(Buffer * const pBD)
{
	/*number of chars printed*/
	int numbOfChars = 0;
	/*used as a local storage variable to print the buffer*/
	char charator;

	if (!pBD)
		return RT_FAIL_1;

	/*if the buffer is empty report so and return 0*/
	if (b_isempty(pBD) == TRUE) {
		printf("Empty buffer!\n");
		return numbOfChars;
	}

	/*set the buffer getc to the strart to start retriveing values*/
	pBD->getc_offset = 0;

	/*run through loop and print the values*/
	while (1) {
		charator = b_getc(pBD);
		if (b_eob(pBD) == 1) {
			break;
		}
		printf("%c", charator);
		/*amount of chars printed*/
		numbOfChars++;
	}

	printf("\n");

	return numbOfChars;
}

/*
Purpose: compact the buffer
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  realloc()
Parameters : const Bufer* PBD (pointer to our buffer)
char symbol (EOF symbol) range (0-255)
Algorithm: compact the buffer by checking if there is one more space for a EOF, if so add that EOF and return
if there is more room reallocate the buffer with one more space to add the EOF charator, set the
reallocation flag updates the buffer destriptors new capacity, increment it by one and return the
pointer to the new chunk of memory
*/
Buffer * b_compact(Buffer * const pBD, char symbol)
{
	/*pointer to the soon to be compacted char* array*/
	char* Bpointer = NULL;
	/*new capacity variable holder*/
	short newCapacity = 0;

	if (!pBD)
		return NULL;

	/*check if we can even add EOF symbol*/
	newCapacity = (pBD->addc_offset + 1) * sizeof(char);

	/*if overflow returns*/
	if (newCapacity < 0) {
		return pBD;
	}

	/*if were already at the end of the buffer just add the symbol and return*/
	if (newCapacity == pBD->capacity) {
		pBD->cb_head[pBD->addc_offset] = symbol;
		pBD->addc_offset++;
		return pBD;
	}
	else {

		/*if its not the end reallocte enough space for just one more char for EOF*/
		if ((Bpointer = (char*)realloc(pBD->cb_head, pBD->addc_offset + 1)) == NULL)
			return NULL;

		if (Bpointer != pBD->cb_head)
			pBD->flags |= SET_R_FLAG;

		/*assign the new buffer to the buffer descriptor then assign the new values to descriptor*/
		pBD->cb_head = Bpointer;
		pBD->capacity = newCapacity;
		pBD->cb_head[pBD->addc_offset] = symbol;
		pBD->addc_offset++;
	}

	return pBD;
}

/*
Purpose: return r int value
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm:	return a int of the value of R flag
*/
char b_rflag(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	return (pBD->flags & CHECK_R_FLAG);
}

/*
Purpose: decrement the offset by 1
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm:	decrement the offset by 1 return decremented value
*/
short b_retract(Buffer * const pBD) {

	if (!pBD)
		return RT_FAIL_1;

	pBD->getc_offset--;

	return pBD->getc_offset;
}

/*
Purpose: move the getc offset to the mark offset
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm:	make the the getc offset equal the mark offset, return the new getc offset
*/
short b_reset(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	pBD->getc_offset = pBD->markc_offset;

	return pBD->getc_offset;
}

/*
Purpose: return the getc offset
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm:	return the getc offset
*/
short b_getcoffset(Buffer * const pBD) {

	if (!pBD)
		return RT_FAIL_1;

	return pBD->getc_offset;
}

/*
Purpose: rewing buffer values
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
Algorithm:	set getc_offset and markc_offset to equal 0, return 0 if succesful
*/
int b_rewind(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL_1;

	pBD->getc_offset = 0;
	pBD->markc_offset = 0;

	return 0;
}

/*
Purpose: return a pointer to the loc_offset values
Author : Alexandre Dagher
History / Versions : October 17 2018
Called functions :  none
Parameters : const Bufer* PBD (pointer to our buffer)
short loc_offset (value of where the loc offset is) range: -32,768 to 32,767
Algorithm: increment the head pointer to the number specified by loc_offset
*/
char * b_location(Buffer * const pBD, short loc_offset) {

	if (!pBD)
		return NULL;

	return (pBD->cb_head + loc_offset);
}
