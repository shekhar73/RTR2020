#include <stdio.h>

struct MyNumber
{
	int iNum;
	int iNumTable[10];
};

struct NumTables
{
	struct MyNumber p;
};

int main(void)
{
	//variable declarations
	struct NumTables tables[10];
	
	int icnt, jcnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		tables[icnt].p.iNum = (icnt + 1);
	}
	
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\n\n");
		printf("\tTable of %d : \n\n", tables[icnt].p.iNum);
		for(jcnt = 0; jcnt < 10; jcnt++)
		{
			tables[icnt].p.iNumTable[jcnt] = tables[icnt].p.iNum * (jcnt + 1);
			printf("\t%d * %d = %d\n", tables[icnt].p.iNum, (jcnt + 1), tables[icnt].p.iNumTable[jcnt]);
		}
	}
	
	return (0);
}
