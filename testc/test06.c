#include <stdio.h>
#include <time.h>

int main(void)
{
	clock_t c0=clock();
	time_t t0=time(NULL);

	printf("TESTING:clock()\n");
	printf("C0: %d\n",c0);
	while(c0==clock())
	{
	}
	printf("C1: %d\n",clock());

	printf("TESTING:time(NULL)\n");
	printf("T0: %d\n",t0);
	while(t0==time(NULL))
	{
	}
	printf("T1: %d\n",time(NULL));

	return 0;
}
