#include <stdio.h>
#define MAX_STRING_LENGTH 512
int main(void)
{
	//variable declaraions
	
	
	char string_array[5][10];
	int char_size;
	int string_array_size;
	int string_array_num_elements, string_array_num_rows, string_array_num_columns;
	int i;
	
	//code
	printf("\n\n");
	char_size = sizeof(char);
	string_array_size = sizeof(string_array);
	
	printf("Size Of Two Dimensional ( 2D ) Character Array (String Array) Is = %d \n\n", string_array_size);
	string_array_num_rows = string_array_size / sizeof(string_array[0]);
	
	printf("Number of Rows (Strings) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_rows);
	string_array_num_columns = sizeof(string_array[0]) / char_size;
	
	printf("Number of Columns In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_columns);
	string_array_num_elements = string_array_num_rows * string_array_num_columns;
	
	printf("Maximum Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_elements);
	
	// *** PIECE-MEAL ASSIGNMENT ***
	// ****** ROW 1 / STRING 1 ******
	string_array[0][0] = 'M';
	string_array[0][1] = 'y';
	string_array[0][2] = '\0'; //NULL-TERMINATING CHARACTER

	// ****** ROW 2 / STRING 2 ******
	string_array[1][0] = 'N';
	string_array[1][1] = 'a';
	string_array[1][2] = 'm';
	string_array[1][3] = 'e';
	string_array[1][4] = '\0'; //NULL-TERMINATING CHARACTER
	
	// ****** ROW 3 / STRING 3 ******
	string_array[2][0] = 'I';
	string_array[2][1] = 's';
	string_array[2][2] = '\0'; //NULL-TERMINATING CHARACTER
	
	// ****** ROW 4 / STRING 4 ******
	string_array[3][0] = 'S';
	string_array[3][1] = 'h';
	string_array[3][2] = 'e';
	string_array[3][3] = 'k';
	string_array[3][4] = 'h';
	string_array[3][5] = 'a';
	string_array[3][6] = 'r';
	string_array[3][7] = '\0'; //NULL-TERMINATING CHARACTER
	
	// ****** ROW 5 / STRING 5 ******
	string_array[4][0] = 'K';
	string_array[4][1] = 'a';
	string_array[4][2] = 'r';
	string_array[4][3] = 'l';
	string_array[4][4] = 'e';
	string_array[4][5] = '\0'; //NULL-TERMINATING CHARACTER
	
	printf("\n\n");
	printf("The Strings In the 2D Character Array Are : \n\n");
	for (i = 0; i < string_array_num_rows; i++)
	{
		printf("%s ", string_array[i]);
	}
	printf("\n\n");
	return(0);
}

int string_length(char str[])
{
	//variable declarations
	int j;
	int string_length = 0;

	//code

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);
}