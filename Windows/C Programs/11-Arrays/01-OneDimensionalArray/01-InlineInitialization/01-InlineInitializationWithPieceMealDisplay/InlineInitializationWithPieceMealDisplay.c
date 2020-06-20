#include <stdio.h>

int main(void)
{
	//variable declarations
	int int_array[] = {4, 5, 7, 45, 54, 98, 25, 2, 12, 70};
	int int_size;
	int int_array_size;
	int int_array_num_elements;
	
	float float_array[] = {12.25f, 1.2f, 2.4f, 7.5f, 9.8f};
	int float_size;
	int float_array_size;
	int float_array_num_elements;
	
	char char_array[] = {'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M','P'};
	int char_size;
	int char_array_size;
	int char_array_num_elements;
	
	//code
	
	//--------int_array[]---------//
	 
	printf("\n\n");
	printf("In-line initialization and peice-meal display of elements of array 'int_array[]':\n");
	printf("int_array[0] (1st Element) = %d\n", int_array[0]);
	printf("int_array[1] (2nd Element) = %d\n", int_array[1]);
	printf("int_array[2] (3rd Element) = %d\n", int_array[2]);
	printf("int_array[3] (4th Element) = %d\n", int_array[3]);
	printf("int_array[4] (5th Element) = %d\n", int_array[4]);
	printf("int_array[5] (6th Element) = %d\n", int_array[5]);
	printf("int_array[6] (7th Element) = %d\n", int_array[6]);
	printf("int_array[7] (8th Element) = %d\n", int_array[7]);
	printf("int_array[8] (9th Element) = %d\n", int_array[8]);
	printf("int_array[9] (10th Element) = %d\n", int_array[9]);

	printf("\n");
	int_size = sizeof(int);
	int_array_size = sizeof(int_array);
	int_array_num_elements = int_array_size/int_size;
	
	printf("Size of data type 'int' = %d bytes\n", int_size);
	printf("Number of elements in 'int' Array 'int_array[]' = %d Elements\n", int_array_num_elements);
	printf("Size of Array 'int_array[]' (%d Elements * %d Bytes) = %d Bytes\n\n", int_array_num_elements, int_size, int_array_size);
	
		// ****** float_array[] ******
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements of Array 'float_array[]': \n\n");
	printf("float_array[0] (1st Element) = %f\n", float_array[0]);

	printf("float_array[1] (2nd Element) = %f\n", float_array[1]);
	printf("float_array[2] (3rd Element) = %f\n", float_array[2]);
	printf("float_array[3] (4th Element) = %f\n", float_array[3]);
	printf("float_array[4] (5th Element) = %f\n", float_array[4]);
	printf("float_array[5] (6th Element) = %f\n", float_array[5]);
	
	float_size = sizeof(float);
	float_array_size = sizeof(float_array);
	float_array_num_elements = float_array_size / float_size;
	
	printf("\n");
	
	printf("Size Of Data type 'float' = %d bytes\n",float_size);
	printf("Number Of Elements In 'float' Array 'float_array[]' = %d Elements\n",float_array_num_elements);
	printf("Size Of Array 'float_array[]' (%d Elements * %d Bytes) = %d Bytes\n\n",float_array_num_elements, float_size, float_array_size);
	
	// ****** char_array[] ******
	printf("\n\n");
	printf("In-line Initialization And Piece-meal Display Of Elements of Array 'char_array[]': \n\n");
	printf("char_array[0] (1st Element) = %c\n", char_array[0]);
	printf("char_array[1] (2nd Element) = %c\n", char_array[1]);
	printf("char_array[2] (3rd Element) = %c\n", char_array[2]);
	printf("char_array[3] (4th Element) = %c\n", char_array[3]);
	printf("char_array[4] (5th Element) = %c\n", char_array[4]);
	printf("char_array[5] (6th Element) = %c\n", char_array[5]);
	printf("char_array[6] (7th Element) = %c\n", char_array[6]);
	printf("char_array[7] (8th Element) = %c\n", char_array[7]);
	printf("char_array[8] (9th Element) = %c\n", char_array[8]);
	printf("char_array[9] (10th Element) = %c\n", char_array[9]);
	printf("char_array[10] (11th Element) = %c\n", char_array[10]);
	printf("char_array[11] (12th Element) = %c\n", char_array[11]);
	printf("char_array[12] (13th Element) = %c\n\n", char_array[12]);
	
	char_size = sizeof(char);
	char_array_size = sizeof(char_array);
	char_array_num_elements = char_array_size / char_size;
	
	printf("\n");
	printf("Size Of Data type 'char' = %d bytes\n", char_size);
	printf("Number Of Elements In 'char' Array 'char_array[]' = %d Elements\n", char_array_num_elements);
	printf("Size Of Array 'char_array[]' (%d Elements * %d Bytes) = %d Bytes\n\n", char_array_num_elements, char_size, char_array_size);
	
	return(0);
}