#include <stdio.h>

struct MyNumber
{
	int iNum;
	int iNumTable[10];
};

struct NumTables
{
	struct MyNumber p;
	struct MyNumber q;
	struct MyNumber r;
};

int main(void)
{
	//variable declarations
	struct NumTables tablesObj;
	int icnt;
	
	//code
	tablesObj.p.iNum = 2;
	for(icnt = 0; icnt < 10; icnt++)
		tablesObj.p.iNumTable[icnt] = tablesObj.p.iNum * (icnt + 1);
	
	printf("\n\n");
	printf("\tTable of %d : \n\n", tablesObj.p.iNum);
	for(icnt = 0; icnt < 10; icnt++)
		printf("\t%d * %d = %d\n", tablesObj.p.iNum, (icnt + 1), tablesObj.p.iNumTable[icnt]);
	
	tablesObj.q.iNum = 3;
	for(icnt = 0; icnt < 10; icnt++)
		tablesObj.q.iNumTable[icnt] = tablesObj.q.iNum * (icnt + 1);
	
	printf("\n\n");
	printf("\tTable of %d : \n\n", tablesObj.q.iNum);
	for(icnt = 0; icnt < 10; icnt++)
		printf("\t%d * %d = %d\n", tablesObj.q.iNum, (icnt + 1), tablesObj.q.iNumTable[icnt]);
	
	tablesObj.r.iNum = 4;
	for(icnt = 0; icnt < 10; icnt++)
		tablesObj.r.iNumTable[icnt] = tablesObj.r.iNum * (icnt + 1);
	
	printf("\n\n");
	printf("\tTable of %d : \n\n", tablesObj.r.iNum);
	for(icnt = 0; icnt < 10; icnt++)
		printf("\t%d * %d = %d\n", tablesObj.r.iNum, (icnt + 1), tablesObj.r.iNumTable[icnt]);
	
	return (0);
}
