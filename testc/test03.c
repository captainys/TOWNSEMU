#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int i;
	printf("TESTING: srand and rand\n");
	srand(100);
	for(i=0; i<10; ++i)
	{
		printf("%d ",rand()%100);
	}
	printf("\n");
	return 0;
}
