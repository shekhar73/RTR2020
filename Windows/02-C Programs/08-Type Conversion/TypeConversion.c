#include <stdio.h>

int main(void)
{
	int int_i, int_j;
	char ch_01, ch_02;

	int int_a, int_result;
	float float_f, float_result;

	int int_explicit;
	float float_explicit;

	printf("\n\n");

	//INTERCONVERSION AND IMPLICIT TYPE-CASTING BETWEEN 'char' AND 'int' TYPES....

	int_i = 73;
	ch_01 = int_i; //it will print the corresponding ASCII value for 73 is I
	printf("I = %d\n", int_i);
	printf("Character 1 (after ch_01 = int_i) = %c\n\n", ch_01);

	ch_02 = 'G';
	int_j = ch_02; //it will print the ASCII value of G i.e. 71
	printf("Character 2 = %c\n", ch_02);
	printf("J (after j =ch_02) = %d\n\n", int_j);

	//IMPLICIT CONVERSION OF 'int' TO 'float'...
	int_a = 7;
	float_f = 9.8f;
	float_result = int_a + float_f;
	printf("Integer int_a = %d And Floating Point Number float_f = %f added Gives Floating-Point sum = %f\n\n", int_a, float_f, float_result);

	int_result = int_a + float_f;
	printf("Integer int_a = %d And Floating Point Number float_f = %f added Integer sum = %d\n\n", int_a, float_f, int_result);

	//EXPLICIT TYPE-CASTING USING CAST OPERATOR...
	float_explicit = 33.123324f;
	int_explicit = (int)float_explicit;
	printf("Floating Point Number which will be type Casted Explicitly = %f\n", float_explicit);
	printf("Resultant integer after explicit type casting of float_explicit = %f => %d\n\n", float_explicit, int_explicit);

	return (0);
}
