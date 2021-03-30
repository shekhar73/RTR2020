#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SUCCESS 1
#define BUFFER_SIZE 128

struct vector
{
	int* a;
	int n;
};

struct vector* create_vector();
int push_back(struct vector* p_vec, int new_element);
void destroy_vector(struct vector* p_vec);

char buffer[BUFFER_SIZE];

int main(int argc, char* argv[])
{
	FILE *fp = NULL;
	int n;
	int i;

	struct vector* p_vec = create_vector();

	if(argc != 2)
	{
		printf("Usage Error : %s file_name\n", argv[0]);
		exit(-1);
	}

	fp = fopen(argv[1], "r");
	assert(fp != NULL);

	while(fgets(buffer, BUFFER_SIZE, fp))
	{
		n = atoi(buffer);
		push_back(p_vec, n);
		memset(buffer, 0, BUFFER_SIZE);
	}

	for(i = 0; i < p_vec->n; ++i)
	{
		printf("p_vec->a[%d]:%d\n", i, p_vec->a[i]);
	}

	destroy_vector(p_vec);
	p_vec = NULL;

	fclose(fp);
	fp = NULL;

	return (0);

}

struct vector* create_vector()
{
	struct vector* p_vec = NULL;
	p_vec = (struct vector*)malloc(sizeof(struct vector));
	assert(p_vec != NULL);

	p_vec->a = NULL;
	p_vec->n = 0;

	return(p_vec);
}

int push_back(struct vector* p_vec, int new_element)
{
	p_vec->a = (int*)realloc(p_vec->a, (p_vec->n+1) * sizeof(int));
	p_vec->n = p_vec->n + 1;
	p_vec->a[p_vec->n - 1] = new_element;
	return (SUCCESS);
}

void destroy_vector(struct vector* p_vec)
{
	free(p_vec->a);
	free(p_vec);
}