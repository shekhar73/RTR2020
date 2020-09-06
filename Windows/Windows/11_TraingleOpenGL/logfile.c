#include <stdio.h>

int main()
{
	FILE* pFile = NULL;

	if (fopen_s(&pFile, "SomshekharLogFile.txt", "w") != 0)
	{
		printf("Can not open desired file\n");
		exit(0);
	}

	fprintf(pFile, "Hello Somshekhar !!!\n");
	fclose(pFile);
	pFile = NULL;

	return (0);

}