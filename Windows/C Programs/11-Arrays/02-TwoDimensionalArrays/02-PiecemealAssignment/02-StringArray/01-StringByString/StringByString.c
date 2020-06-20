#include <stdio.h>
#define MAX_STRING_LENGTH 512
int main(void)
{
	//function prototype
	void my_string_copy(char[], char[]);
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
	
	printf("Size of two dimensioanl (2D) character array (String Array) id = %d \n\n", string_array_size);
	string_array_num_rows = string_array_size / sizeof(string_array[0]);
	
	printf("Number of rows (strings) in two dimensional ( 2D ) character array (String Array) Is = %d\n\n", string_array_num_rows);
	string_array_num_columns = sizeof(string_array[0]) / char_size;
	
	printf("Number of columns in two dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_columns);
	
	string_array_num_elements = string_array_num_rows * string_array_num_columns;
	printf("Maximum Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_elements);
	
	// *** PIECE-MEAL ASSIGNMENT ***
	my_string_copy(string_array[0], "My");
	my_string_copy(string_array[1], "Name");
	my_string_copy(string_array[2], "Is");
	my_string_copy(string_array[3], "shekhar");
	my_string_copy(string_array[4], "Karle");
	
	printf("\n\n");
	printf("The Strings In the 2D Character Array Are : \n\n");
	
	for (i = 0; i < string_array_num_rows; i++)
	{
		printf("%s ", string_array[i]);
	}
	
	printf("\n\n");
	return(0);
}	

void my_string_copy(char str_destination[], char str_source[])
{
	//function prototype
	int string_length(char[]);

	//variable declarations
	int int_string_length = 0;
	int j;

	//code
	int_string_length = string_length(str_source);
	for (j = 0; j < int_string_length; j++)
	{
		str_destination[j] = str_source[j];
	}
	str_destination[j] = '\0';
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