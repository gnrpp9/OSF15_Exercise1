#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * INPUTS: 
 *	name the name of the matrix limited to 50 characters 
 *  rows the number of rows the matrix
 *  cols the number of cols the matrix
 * RETURN:
 *  If no errors occurred during instantiation then true
 *  else false for an error in the process.
 *
 **/

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows, const unsigned int cols) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if ((*new_matrix) != NULL)
	{
		printf("Matrix exists!\n");
		return false;
	}
	if (!name || strcmp(name, "\n") == 0)
	{
		printf("No name for new matrix!\n");
		return false;
	}
	if (rows <= 0 || cols <= 0)
	{
		printf("Not enough rows and or collumns!\n");
		return false;
	}

	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Destroy's matrix
        INPUT: m - matrix to be destroyed
        RETURN: Nothing
*/

void destroy_matrix (Matrix_t** m) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!*m)
	{
		printf("Matrix doesn't exist!\n");
		return;
	}

	free((*m)->data);
	free(*m);
	*m = NULL;
}


	
//TODO FUNCTION COMMENT
/*
        PURPOSE: Checks to see if two matrices are equal
        INPUT: a, b - matrices to be compared
        RETURN: if a and b are equal return true
		else false
*/

bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a || !b || !a->data || !b->data) {
		printf("One of the matrices is null or doensn't have any data!\n");
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Copy a matrix into a different matrix
        INPUT: src - matricy to be copied
		dest - matrix for src to be copied to
        RETURN: If successful returns true
		else false
*/

bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {


	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!src) {
		printf("No matrix to copy!\n");
		return false;
	}
	if (!dest)
	{
		printf("No matrix for the source to be copied to!\n");
		return false;
	}
	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Bit shift a matrix either to the left or right by a number of positions
        INPUT: a - matrix to be bit shifted
		direction - direction to be shifted
		shift - number of positions to be shifted
        RETURN: If successful return true
		else false
*/

bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a) {
		printf("No matrix to bit shift!\n");
		return false;
	}
	if (shift <= 0)
	{
		printf("Shift must be greater than 0!\n");
		return false;
	}

	if (direction == 'l') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Add matrix a and b and put the result into matrix c
        INPUT: a, b - matrices to be added
		c - matrix for the results to be stored
        RETURN: If successful return true
		else false
*/

bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a || !b || !c)
	{
		printf("One or more matrices are null!\n");
		return false;
	}
	if (a->rows != b->rows && a->cols != b->cols) {
		printf("Incompatible matrix rows and collumns!\n");
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Print matrix to screen
        INPUT: m - matrix to be printed
        RETURN: Nothing
*/

void display_matrix (Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!m)
	{
		printf("No matrix to display!\n");
		return;
	}
	if (!m->data)
	{
		printf("No data in matrix!\n");
		return;
	}

	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Read matrix from a file
        INPUT: matrix_input_filename - file to read matrix from
		m - matrix to put file matrix into
        RETURN: If successfull returns true
		else false
*/

bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!matrix_input_filename)
	{
		printf("No filename!\n");
		return false;
	}
	if (!m || !(*m)->data)
	{
		printf("No matrix and/or data!\n");
		return false;
	}

	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);
	free(data);
	if (close(fd)) {
		return false;

	}
	return true;
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Write a matrix to a file
        INPUT: matrix_output_filename - file for matrix to be wrote to
		m - matrix to be wrote to a file
        RETURN: If successfull return true
		else false
*/

bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
        if (!matrix_ouput_filename)
        {
                printf("No filename!\n");
                return false;
        }
        if (!m || !(*m)->data)
        {
                printf("No matrix and/or data!\n");
                return false;
        }

	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Fills a matrix with random values in a given range
        INPUT: m - matrix to be filled with random values
		start_range - the lower bound for range
		end_range - the upper bound for range
        RETURN: If successfull return true
                else false
*/

bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
        if (!m || !(*m)->data)
        {
                printf("No matrix and/or data!\n");
                return false;
        }
	if (start_range > end_range || start_range < 0)
	{
		printf("Start range is bigger than end range or start range is less than 0!\n");
		return false;
	}

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % (end_range + 1 - start_range) + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

//TODO FUNCTION COMMENT
/*
        PURPOSE: Fill a matrix with data
        INPUT: m - matrix to be filled with data
                data - data to be put in matrix
        RETURN: Nothing
*/

void load_matrix (Matrix_t* m, unsigned int* data) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!m || !data)
	{
		printf("No matrix and/or data!\n");
		return;
	}

	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

//TODO FUNCTION COMMENT
/*
        PURPOSE: Adds matrix to list of matrices. Overrides matrix in that position desired
        INPUT: mats - array of matrices for the matrix to be added to
                new_matrix - matrix to be added to array of matrices
		num_mats - number of matrices in mats
        RETURN: Position of new matrix in mats
*/

unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(!mats)
	{
		printf("No list of matrices!\n");
		return -1;
	}
	if (!new_matrix)
	{
		printf("No matrix to add!\n");
		return -1;
	}

	if (num_mats <= 0)
	{
		printf("Number of matrices is less than or equal to 0!\n");
		return -1;
	}

	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;
	return pos;
}
