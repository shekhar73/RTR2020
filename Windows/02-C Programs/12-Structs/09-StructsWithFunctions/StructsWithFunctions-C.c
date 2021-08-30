#include <stdio.h>

// DEFINING STRUCT
struct MyStruct
{
	int inum;
	float fnum;
	double dnum;
	char ch;
};

int main(void)
{
	//function prototype
	struct MyStruct AddStructureMembers(struct MyStruct, struct MyStruct, struct MyStruct);
	
	//variable declarations
	struct MyStruct myData1, myData2, myData3, answerData;
	
	//code
	// *** myData1 ***
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t********** DATA 1 **********\n\n");
	
	printf("\tEnter integer value for 'inum' of 'struct MyStruct myData1' : ");
	scanf("%dnum", &myData1.inum);
	
	printf("\n\n");
	printf("\tEnter floating-point value for 'fnum' Of 'struct MyStruct myData1' : ");
	scanf("%fnum", &myData1.fnum);
	
	printf("\n\n");
	printf("\tEnter 'Double' value for 'dnum' ff 'struct MyStruct myData1' : ");
	scanf("%lf", &myData1.dnum);
	
	printf("\n\n");
	printf("\tEnter Character value for 'ch' of 'struct MyStruct myData1' : ");
	myData1.ch = getch();
	printf("\t%c", myData1.ch);
	
	// *** myData2 ***
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t********** DATA 2 **********\n\n");
	
	printf("\tEnter Integer Value For 'inum' Of 'struct MyStruct myData2' : ");
	scanf("%dnum", &myData2.inum);
	
	printf("\n\n");
	printf("\tEnter Floating-Point Value For 'fnum' Of 'struct MyStruct myData2' : ");
	scanf("%fnum", &myData2.fnum);
	
	printf("\n\n");
	printf("\tEnter 'Double' Value For 'dnum' Of 'struct MyStruct myData2' : ");
	scanf("%lf", &myData2.dnum);
	
	printf("\n\n");
	printf("\tEnter Character Value For 'ch' Of 'struct MyStruct myData2' : ");
	myData2.ch = getch();
	printf("\t%c", myData2.ch);
	
	// *** myData3 ***
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t********** DATA 3 **********\n\n");
	printf("\tEnter integer value for 'inum' of 'struct MyStruct myData3' : ");
	scanf("%dnum", &myData3.inum);
	
	printf("\n\n");
	printf("\tEnter Floating-Point value for 'fnum' of 'struct MyStruct myData3' : ");
	scanf("%fnum", &myData3.fnum);
	
	printf("\n\n");
	printf("\tEnter 'Double' value for 'dnum' of 'struct MyStruct myData3' : ");
	scanf("%lf", &myData3.dnum);
	
	printf("\n\n");
	printf("\tEnter Character value for 'ch' of 'struct MyStruct myData3' : ");
	myData3.ch = getch();
	printf("\t%c", myData3.ch);
	
	answerData = AddStructureMembers(myData1, myData2, myData3);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t********** ANSWER *********\n\n");
	
	printf("\tanswerData.inum = %d\n", answerData.inum);
	printf("\tanswerData.fnum = %f\n", answerData.fnum);
	printf("\tanswerData.dnum = %lf\n\n", answerData.dnum);
	
	answerData.ch = myData1.ch;
	printf("\tanswerData.ch (from myData1) = %c\n\n", answerData.ch);
	
	answerData.ch = myData2.ch;
	printf("\tanswerData.ch (from myData2) = %c\n\n", answerData.ch);
	
	answerData.ch = myData3.ch;
	printf("\tanswerData.ch (from myData3) = %c\n\n", answerData.ch);
	
	return(0);
}

struct MyStruct AddStructureMembers(struct MyStruct mydata1, struct MyStruct mydata2, struct MyStruct mydata3)
{
	//variable declarations
	struct MyStruct answer;

	//code
	answer.inum = mydata1.inum + mydata2.inum + mydata3.inum;
	answer.fnum = mydata1.fnum + mydata2.fnum + mydata3.fnum;
	answer.dnum = mydata1.dnum + mydata2.dnum + mydata3.dnum;
	return(answer);
}