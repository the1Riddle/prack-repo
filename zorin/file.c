#include <stdio.h>
/**
 * main - the entry point
 *
 * Return: 0 for success
 *
 */
int main(void)
{
	const int size = 8;
	int j, x;
	
	for (j = 0; j < size; j++)
	{
		if(j%2 != 0)
		{

			printf("      ");
			printf("      ");
			printf("      ");
		}
		for (x = 0; x < size/2; x++)
		{
			printf("      ");
			printf("      ");
			printf("      ");
			printf("██████");
			printf("██████");
			printf("██████");
		}

		printf("\n");
	}
	return (0)
}
