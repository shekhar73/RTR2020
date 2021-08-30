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
	int i, j;
	
	//code
	printf("\n\n");
	int_size = sizeof(int);
	int_array_size = sizeof(int_array);
	printf("Size Of Two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_size);
	int_array_num_rows = int_array_size / sizeof(int_array[0]);
	
	printf("Number of Rows In Two Dimensional ( 2D ) Integer Array Is = %d\n\n",int_array_num_rows);
	int_array_num_columns = sizeof(int_array[0]) / int_size;
	printf("Number of Columns In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_num_columns);
	
	int_array_num_elements = int_array_num_rows * int_array_num_columns;
	printf("Number of Elements In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", int_array_num_elements);
	printf("\n\n");
	printf("Elements In The 2D Array : \n\n");
	
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