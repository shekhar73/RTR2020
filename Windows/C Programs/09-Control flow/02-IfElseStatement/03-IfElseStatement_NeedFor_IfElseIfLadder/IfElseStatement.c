#include <stdio.h>
int main(void)
{
	//variable declarations
	int int_inum;
	
	//code
	printf("\n\n");
	
	printf("Enter Value For 'num' : ");
	scanf("%d", &int_inum);
	
	if (int_inum < 0) // 'if' - 01
	{
		printf("Num = %d Is Less Than 0 (NEGATIVE) !!!\n\n", int_inum);
	}
	else // 'else' - 01
	{
		if ((int_inum > 0) && (int_inum <= 100)) // 'if' - 02
		{
			printf("Num = %d Is Between 1 And 100 !!!\n\n", int_inum);
		}
		else // 'else' - 02
		{
			if ((int_inum > 100) && (int_inum <= 200)) // 'if' - 03
			{
				printf("Num = %d Is Between 101 And 200 !!!\n\n", int_inum);
			}
			else // 'else' - 03
			{
				if ((int_inum > 200) && (int_inum <= 300)) // 'if' - 04
				{
					printf("Num = %d Is Between 201 And 300 !!!\n\n", int_inum);
				}
				else // 'else' - 04
				{
					if ((int_inum > 300) && (int_inum <= 400)) // 'if' - 05
					{
						printf("Num = %d Is Between 301 And 400 !!!\n\n", int_inum);
					}
					else // 'else' - 05
					{
						if ((int_inum > 400) && (int_inum <= 500)) // 'if' - 06
						{
							printf("Num = %d Is Between 401 And 500 !!!\n\n",int_inum);
						}
						else // 'else' - 06
						{
							printf("Num = %d Is Greater Than 500 !!!\n\n", int_inum);
						} // closing brace of 'else' - 06
					} // closing brace of 'else' - 05
				} // closing brace of 'else' - 04
			} // closing brace of 'else' - 03
		} // closing brace of 'else' - 02
	} // closing brace of 'else' - 01
	return(0);
}