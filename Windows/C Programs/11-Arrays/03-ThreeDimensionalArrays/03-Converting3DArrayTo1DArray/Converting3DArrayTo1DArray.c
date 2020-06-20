#include<stdio.h>

#define NUMBER_ROWS 5
#define NUMBER_COLUMNS 3
#define DEPTH 2

int main(void)
{
	int iArray[NUMBER_ROWS][NUMBER_COLUMNS][DEPTH] = { { {9, 4}, {5, 4}, {5, 2} },
							{ {8, 5}, {3, 2}, {1, 2} },
							{ {6, 2}, {8, 9}, {7, 7} },
							{ {9, 3}, {2, 3}, {5, 5} },
							{ {6, 9}, {5, 2}, {3, 3} },
						  };
						  
	int icnt, jcnt, kcnt;
	
	int iArray_1D[NUMBER_ROWS * NUMBER_COLUMNS * DEPTH];
	
	//Displaying 3D array
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tElements in integer 3D array : \n\n");
	
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		printf("\t~~~~~~~~~~~~~~~~~~~~ROW %d ~~~~~~~~~~~~~~~~~~~~~\n\n", (icnt + 1));
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			printf("\t~~~~~~~~~~~~~~~~~~~~COLUMNS %d ~~~~~~~~~~~~~~~~~~~~~\n", (jcnt + 1));
			for(kcnt = 0; kcnt < DEPTH; kcnt++)
			{
				printf("\t\tiArray[%d][%d][%d] = %d", icnt, jcnt, kcnt, iArray[icnt][jcnt][kcnt]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	
	//converting 3D to 1D array
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			for(kcnt = 0; kcnt < DEPTH; kcnt++)
			{
				iArray_1D[(icnt * NUMBER_COLUMNS * DEPTH) + (jcnt * DEPTH) + kcnt] = iArray[icnt][jcnt][kcnt];
			}
		}
	}
	
	//printing 1D array
	printf("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("One-Dimensional (1D) array of integers : \n\n");
	for(icnt = 0; icnt < (NUMBER_ROWS * NUMBER_COLUMNS * DEPTH); icnt++)
	{
		printf("\tiArray_1D[%d] = %d", icnt, iArray_1D[icnt]);
	}
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	return (0);
}