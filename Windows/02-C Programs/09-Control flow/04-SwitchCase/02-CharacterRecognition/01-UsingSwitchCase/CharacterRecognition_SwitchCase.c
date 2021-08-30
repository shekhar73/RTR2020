#include <stdio.h>
#include <conio.h>
// ASCII Values For 'A' to 'Z' => 65 to 90
#define CHAR_ALPHABET_UPPER_CASE_BEGINNING 65
#define CHAR_ALPHABET_UPPER_CASE_ENDING 90
// ASCII Values For 'a' to 'z' => 97 to 122
#define CHAR_ALPHABET_LOWER_CASE_BEGINNING 97
#define CHAR_ALPHABET_LOWER_CASE_ENDING 122
// ASCII Values For '0' to '9' => 48 to 57
#define CHAR_DIGIT_BEGINNING 48
#define CHAR_DIGIT_ENDING 57

int main(void)
{
	//variable declarations
	char sk_ch;
	int sk_ch_value;
	//code
	
	printf("\n\n");
	printf("Enter Character : ");
	
	sk_ch = getch();
	printf("\n\n");
	
	switch (sk_ch)
	{
		// FALL THROUGH CONDITION...
		case 'A':
		case 'a':
		case 'E':
		case 'e':
		case 'I':
		case 'i':
		case 'O':
		case 'o':
		case 'U':
		case 'u':
			printf("Character \'%c\' Entered by you, is a Vowel character from the english alphabet !!!\n\n", sk_ch);
			break;
		
		default:
			sk_ch_value = (int)sk_ch;
			//If The Character Has ASCII Value Between 65 AND 90 OR Between 97 AND 122, It Is Still A Letter Of The Alphabet, But It Is A 'CONSONANT', and NOT a 'VOWEL'...
		
		if ((sk_ch_value >= CHAR_ALPHABET_UPPER_CASE_BEGINNING && sk_ch_value <= CHAR_ALPHABET_UPPER_CASE_ENDING) 
			|| (sk_ch_value >= CHAR_ALPHABET_LOWER_CASE_BEGINNING && sk_ch_value <= CHAR_ALPHABET_LOWER_CASE_ENDING))
		{
			printf("Character \'%c\' entered by you, is a CONSONANT CHARACTER from the english alphabet !!!\n\n", sk_ch);
		}
		else if (sk_ch_value >= CHAR_DIGIT_BEGINNING && sk_ch_value <= CHAR_DIGIT_ENDING)
		{
			printf("Character \'%c\' entered by you, is a DIGIT CHARACTER !!!\n\n", sk_ch);
		}
		else
		{
			printf("Character \'%c\' entered by you, is a SPECIAL CHARACTER !!!\n\n", sk_ch);
		}

		break;
	}
	printf("Switch case block completed !!!\n");
	return(0);
}