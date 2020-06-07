#include <stdio.h>
int main(void)
{
	//variable declaraions
	int int_array[5][4] = {
							{4, 2, 2, 3}, 
							{7, 2, 4, 6}, 
							{9, 3, 3, 5}, 
							{5, 7, 1, 2}, 
							{2, 1, 10, 22} 
						  };
	int int_size;
	int int_array_size;
	int int_array_num_elements, int_array_num_rows, int_array_num_columns;
	
	//code
	printf("\n\n");
	int_size = sizeof(int);
	int_array_size = sizeof(int_array);
	printf("Size Of Two Dimensional ( 2D ) Integer Array Is = %d\n\n",int_array_size);
	
	int_array_num_rows = int_array_size / sizeof(int_array[0]);
	printf("Number of Rows In Two Dimensional ( 2D ) Integer Array Is = %d\n\n",int_array_num_rows);
	
	int_array_num_columns = sizeof(int_array[0]) / int_size;
	printf("Number of Columns In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_num_columns);
	
	int_array_num_elements = int_array_num_rows * int_array_num_columns;
	printf("Number of Elements In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_num_elements);
	printf("\n\n");
	printf("Elements In The 2D Array : \n\n");
	
	// *** ARRAY INDICES BEGIN FROM 0, HENCE, 1ST ROW IS ACTUALLY 0TH ROW AND 1ST COLUMN IS ACTUALLY 0TH COLUMN ***
	// *** ROW 1 ***
	printf("****** ROW 1 ******\n");
	printf("int_array[0][0] = %d\n", int_array[0][0]); // *** COLUMN 1 *** (0th Element)=> 4
	printf("int_array[0][1] = %d\n", int_array[0][1]); // *** COLUMN 2 *** (1st Element)=> 2
	printf("int_array[0][2] = %d\n", int_array[0][2]); // *** COLUMN 3 *** (2nd Element)=> 3
	printf("int_array[0][3] = %d\n", int_array[0][3]); // *** COLUMN 4 *** (2nd Element)=> 3
	printf("\n\n");
	
	// *** ROW 2 ***
	printf("****** ROW 2 ******\n");
	printf("int_array[1][0] = %d\n", int_array[1][0]); // *** COLUMN 1 *** (0th Element)=> 7
	printf("int_array[1][1] = %d\n", int_array[1][1]); // *** COLUMN 2 *** (1st Element)=> 4
	printf("int_array[1][2] = %d\n", int_array[1][2]); // *** COLUMN 3 *** (2nd Element)=> 2
	printf("int_array[1][3] = %d\n", int_array[1][3]); // *** COLUMN 4 *** (2nd Element)=> 6
	printf("\n\n");
	
	// *** ROW 3 ***
	printf("****** ROW 3 ******\n");
	printf("int_array[2][0] = %d\n", int_array[2][0]); // *** COLUMN 1 *** (0th Element)=> 9
	printf("int_array[2][1] = %d\n", int_array[2][1]); // *** COLUMN 2 *** (1st Element)=> 3
	printf("int_array[2][2] = %d\n", int_array[2][2]); // *** COLUMN 3 *** (2nd Element)=> 3
	printf("int_array[2][3] = %d\n", int_array[2][3]); // *** COLUMN 4 *** (2nd Element)=> 5
	printf("\n\n");
	
	// *** ROW 4 ***
	printf("****** ROW 4 ******\n");
	printf("int_array[3][0] = %d\n", int_array[3][0]); // *** COLUMN 1 *** (0th Element)=> 5
	printf("int_array[3][1] = %d\n", int_array[3][1]); // *** COLUMN 2 *** (1st Element)=> 7
	printf("int_array[3][2] = %d\n", int_array[3][2]); // *** COLUMN 3 *** (2nd Element)=> 1
	printf("int_array[3][3] = %d\n", int_array[3][3]); // *** COLUMN 4 *** (2nd Element)=> 2
	printf("\n\n");
	
	// *** ROW 5 ***
	printf("****** ROW 5 ******\n");
	printf("int_array[4][0] = %d\n", int_array[4][0]); // *** COLUMN 1 *** (0th Element)=> 2
	printf("int_array[4][1] = %d\n", int_array[4][1]); // *** COLUMN 2 *** (1st Element)=> 1
	printf("int_array[4][2] = %d\n", int_array[4][2]); // *** COLUMN 3 *** (2nd Element)=> 10
	printf("int_array[4][3] = %d\n", int_array[4][3]); // *** COLUMN 4 *** (2nd Element)=> 22
	printf("\n\n");
	
	return(0);
}