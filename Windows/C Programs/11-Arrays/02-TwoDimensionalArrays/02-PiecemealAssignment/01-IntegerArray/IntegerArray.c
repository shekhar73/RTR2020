#include <stdio.h>
int main(void)
{
	//variable declaraions
	int int_array[3][5]; // 3 ROWS (0, 1, 2) AND 5 COLUMNS (0, 1, 2, 3, 4)
	int int_size;
	int int_array_size;
	int int_array_num_elements, int_array_num_rows, int_array_num_columns;
	int i, j;
	
	//code
	printf("\n\n");
	int_size = sizeof(int);
	int_array_size = sizeof(int_array);
	
	printf("Size Of Two Dimensional ( 2D ) Integer Array Is = %d\n\n",int_array_size);
	int_array_num_rows = int_array_size / sizeof(int_array[0]);
	printf("Number of Rows in two Dimensional ( 2D ) Integer Array Is = %d\n\n",int_array_num_rows);
	int_array_num_columns = sizeof(int_array[0]) / int_size;
	printf("Number of Columns in two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_num_columns);
	int_array_num_elements = int_array_num_rows * int_array_num_columns;
	printf("Number of elements in Two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_num_elements);
	printf("\n\n");
	printf("Elements In The 2D Array : \n\n");
	
	
	// ****** ROW 1 ******
	int_array[0][0] = 24;
	int_array[0][1] = 43;
	int_array[0][2] = 62;
	int_array[0][3] = 83;
	int_array[0][4] = 135;
	
	// ****** ROW 2 ******
	int_array[1][0] = 24;
	int_array[1][1] = 46;
	int_array[1][2] = 65;
	int_array[1][3] = 858;
	int_array[1][4] = 10;
	
	// ****** ROW 3 ******
	int_array[2][0] = 23;
	int_array[2][1] = 16;
	int_array[2][2] = 29;
	int_array[2][3] = 32;
	int_array[2][4] = 15;
	
	// ****** ROW 3 ******
	int_array[2][0] = 33;
	int_array[2][1] = 46;
	int_array[2][2] = 59;
	int_array[2][3] = 62;
	int_array[2][4] = 715;
	
	// *** DISPLAY ***
	for (i = 0; i < int_array_num_rows; i++)
	{
		printf("****** ROW %d ******\n", (i + 1));
		for (j = 0; j < int_array_num_columns; j++)
		{
			printf("int_array[%d][%d] = %d\n", i, j, int_array[i][j]);
		}
		printf("\n\n");
	}
	return(0);
}