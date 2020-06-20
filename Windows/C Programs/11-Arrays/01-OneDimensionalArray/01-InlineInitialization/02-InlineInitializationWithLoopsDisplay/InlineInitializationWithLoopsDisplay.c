#include <stdio.h>
int main(void)
{
	//variable declaraions
	int int_array[] = { 19, 3, 26, 22, 88, 99, 22, 33, 23, 55 };
	int int_size;
	int int_array_size;
	int int_array_num_elements;
	
	float float_array[] = { 2.2f, 5.3f, 3.1f, 7.5f, 9.6f, 6.1f, 1.1f, 1.9f };
	int float_size;
	int float_array_size;
	int float_array_num_elements;
	
	char char_array[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P' };
	int char_size;
	int char_array_size;
	int char_array_num_elements;
	
	int i;
	
	//code
	// --------- int_array[] --------------
	printf("\n");
	printf("In-line Initialization And Loop (for) Display Of Elements of Array 'int_array[]': \n\n");
	
	int_size = sizeof(int);
	int_array_size = sizeof(int_array);
	int_array_num_elements = int_array_size / int_size;
	
	for (i = 0; i < int_array_num_elements; i++)
	{
		printf("int_array[%d] (Element %d) = %d\n", i, (i + 1), int_array[i]);
	}
	
	printf("\n");
	printf("Size Of Data type 'int' = %d bytes\n", int_size);
	printf("Number Of Elements In 'int' Array 'int_array[]' = %d Elements\n", int_array_num_elements);
	printf("Size Of Array 'int_array[]' (%d Elements * %d Bytes) = %d Bytes\n\n", int_array_num_elements, int_size, int_array_size);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	// ---------- float_array[] -------------
	printf("\n");
	printf("In-line Initialization And Loop (while) Display Of Elements of Array 'float_array[]': \n\n");

	float_size = sizeof(float);
	float_array_size = sizeof(float_array);
	float_array_num_elements = float_array_size / float_size;
	
	i = 0;
	while(i < float_array_num_elements)
	{
		printf("float_array[%d] (Element %d) = %f\n", i, (i + 1), float_array[i]);
		i++;
	}
	
	printf("\n\n");
	printf("Size Of Data type 'float' = %d bytes\n", float_size);
	printf("Number Of Elements In 'float' Array 'float_array[]' = %d Elements\n", float_array_num_elements);
	printf("Size Of Array 'float_array[]' (%d Elements * %d Bytes) = %d Bytes\n\n", float_array_num_elements, float_size, float_array_size);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	// ------ char_array[] ------
	printf("In-line Initialization And Loop (do while) Display Of Elements of Array 'char_array[]': \n\n");
	
	char_size = sizeof(char);
	char_array_size = sizeof(char_array);
	char_array_num_elements = char_array_size / char_size;
	
	i = 0;
	do
	{
		printf("char_array[%d] (Element %d) = %c\n", i, (i + 1), char_array[i]);
		i++;
	}while (i < char_array_num_elements);
	
	printf("\n\n");
	printf("Size Of Data type 'char' = %d bytes\n", char_size);
	printf("Number Of Elements In 'char' Array 'char_array[]' = %d Elements\n", char_array_num_elements);
	printf("Size Of Array 'char_array[]' (%d Elements * %d Bytes) = %d Bytes\n\n", char_array_num_elements, char_size, char_array_size);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	return(0);
}