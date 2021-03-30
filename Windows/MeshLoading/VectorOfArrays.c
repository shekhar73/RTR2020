#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SUCCESS 1

struct vector
{
	int **pp;
	int n;
};

struct vector* create_vector();
int push_back(struct vector* p_vec, int *p_arr);
int destroy_vector(struct vector* p_vec);

/* Define a symbolic constant depicting the maz size of line a data file*/
#define LINE_LENGTH 256

/* Allocate an array of character to hold the current line in a file */
char line[LINE_LENGTH];

int main(int argc, char* argv[])
{
	/*File pointer*/
	FILE *fp = NULL;

	/*Base address of vector*/
	struct vector* p_vec = NULL;

	/*char pointer for holding current*/
	char *token = NULL;

	/*Seperator*/
	char *spe = ",";

	/*base address of array which holds these integers on each line*/
	int *arr = NULL;

	/*counter variables*/
	int i, j;

	/*Make sure that command line argement number is 2*/
	assert(argc == 2);

	/*create an empty vecotr*/
	p_vec = create_vector();

	/*Open a data file*/
	fp = fopen(argv[1], "r");

	/*Make sure that file is opened properly*/
	assert(fp != NULL);

	/*Read file line by line*/
	while(fgets(line, LINE_LENGTH, fp))
	{
		/*Tokenize the line i.e. separate contents between commas*/
		arr = (int*)malloc(3 * sizeof(int));
		assert(arr != NULL);

		/*	Get next token
			Convert it into integer
			store it in next array index
			push back the array in vector
		*/
		token = strtok(line, sep);
		arr[0] = atoi(token);

		token = strtok(NULL, sep);
		arr[1] = atoi(token);

		token = strtok(NULL, sep);
		arr[2] = atoi(token);

		push_back(p_vec, arr);
	}

	/* show the content */
	for(i = 0; i < p_vec->n; ++i)
	{
		printf("Printing Array Number %d\n", i + 1);
		for(j = 0; j < 3; j++)
		{
			printf("arr[%d]:%d\n",j, p_vec->pp[i][j]);
		}
	}

	/*Destroy vector*/
	destroy_vector(p_vec);
	p_vec = NULL;

	/*Close file*/
	fclose(fp);
	fp = NULL;

	/*exit with success*/
}

struct vector* create_vector()
{
	struct vector* p_vec = NULL;
	p_vec = (struct vector*)malloc(sizeof(struct vecotr));
	assert(p_vec != NULL);
	p_vec->pp = NULL;
	p_vec->n = 0;
	return (p_vec);
}

int push_back(struct vector* p_vec, int *p_arr)
{
	p_vec->pp = (int**)realloc(p_vec->pp, (p_vec->n + 1)*sizeof(int*));
	assert(p_vec->pp != NULL);
	p_vec->n = p_vec->n+1;
	p_vec->pp[p_vec->n-1] = p_arr;
	return (SUCCESS);
}

int destroy_vector(struct vector* p_vec)
{
	int i;
	for(i = 0; i < p_vec->n; ++i)
	{
		free(p_vec->pp[i]);
	}
	free(p_vec->pp);
	free(p_vec);
	return SUCCESS;
}
