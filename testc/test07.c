#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Make sure to wait 1/100sec.
   In High-C for TOWNS, CLOCKS_PER_SEC is 100.
*/
void Wait(void)
{
	clock_t c0=clock();
	while(c0==clock());
}

int main(void)
{
	int i;
	char *mem[256];

	printf("TESTING: malloc() and free()\n");
	printf("1\n");
	Wait();
	for(i=0; i<100; ++i)
	{
		char *m=(char *)malloc(100);
		free(m);
	}
	printf("2\n");
	Wait();
	for(i=0; i<100; ++i)
	{
		mem[i]=(char *)malloc(7777);
	}
	for(i=0; i<100; ++i)
	{
		free(mem[i]);
	}

	return 0;
}
