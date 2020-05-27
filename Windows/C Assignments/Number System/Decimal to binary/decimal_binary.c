#include <stdio.h>

int main(void)
{
	int iNum = 0, iRem = 0, icnt = 0, jcnt = 0;
	int arr[10] = {0};
	
	
	printf("Enter a number:");
	scanf("%d", &iNum);
	
	
	while(iNum != 0)
	{
		iRem = iNum % 2;
		arr[icnt] = iRem;
		iNum = iNum / 2;
		icnt++;
	}
	
	for(jcnt = icnt - 1; jcnt >= 0; jcnt--)
	{
		printf("%d", arr[jcnt]);
	}
	
	return (0);
}