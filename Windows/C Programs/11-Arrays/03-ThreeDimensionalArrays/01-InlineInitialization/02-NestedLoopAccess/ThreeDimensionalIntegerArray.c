#include<stdio.h>

int main(void)
{
	//variable declarations
	//In-line initialization
	
	int iArray[5][3][2] = { { {9, 4}, {5, 4}, {5, 2} },
							{ {8, 5}, {3, 2}, {1, 2} },
							{ {6, 2}, {8, 9}, {7, 7} },
							{ {9, 3}, {2, 3}, {5, 5} },
							{ {6, 9}, {5, 2}, {3, 3} },
						  };
						  
	int intSize;
	int iArraySize;
	int iArrayNumElements, iArrayWidth, iArrayHeight, iArrayDepth;
	int icnt, jcnt, kcnt;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	intSize = sizeof(int);
		
	iArraySize = sizeof(iArray);
	printf("\tSize of three dimensional (3D) integer array is = %d\n\n", iArraySize);
	
	iArrayWidth = iArraySize / sizeof(iArray[0]);
	printf("\tNumber of Rows (width) in three-dimensional (3D) integer array is = %d\n\n", iArrayWidth);
	
	iArrayHeight = sizeof(iArray[0]) / sizeof(iArray[0][0]);
	printf("\tNumber of columns (Height) in three dimensional (3D) integer array is = %d\n\n", iArrayHeight);
	
	iArrayDepth = sizeof(iArray[0][0]) / intSize;
	printf("\tDepth of three dimensional (3D) integer array is = %d\n\n", iArrayDepth);
	
	iArrayNumElements = iArrayWidth * iArrayHeight * iArrayDepth;
	printf("\tNumber of elements in three dimensional (3D) integer array is = %d\n\n", iArrayNumElements);

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tElements in integer 3D array : \n\n");
	
	for(icnt = 0; icnt < iArrayWidth; icnt++)
	{
		printf("\t~~~~~~~~~~~~~~~~~~~~ROW %d ~~~~~~~~~~~~~~~~~~~~~\n\n", (icnt + 1));
		for(jcnt = 0; jcnt < iArrayHeight; jcnt++)
		{
			printf("\t~~~~~~~~~~~~~~~~~~~~COLUMNS %d ~~~~~~~~~~~~~~~~~~~~~\n", (jcnt + 1));
			for(kcnt = 0; kcnt < iArrayDepth; kcnt++)
			{
				printf("\t\tiArray[%d][%d][%d] = %d", icnt, jcnt, kcnt, iArray[icnt][jcnt][kcnt]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	
	return (0);
}