#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int i,iSum;
	short sSum;
	char cSum;
	printf("TESTING: Summations and Subtractions\n");

	iSum=0;
	sSum=0;
	cSum=0;
	for(i=1; i<=1000; ++i)
	{
		iSum+=i;
	}
	for(i=1; i<=100; ++i)
	{
		sSum+=i;
	}
	for(i=1; i<=10; ++i)
	{
		cSum+=i;
	}
	printf("Added up: %d %d %d\n",iSum,(int)sSum,(int)cSum);
	for(i=1; i<=1000; ++i)
	{
		iSum-=i;
	}
	for(i=1; i<=100; ++i)
	{
		sSum-=i;
	}
	for(i=1; i<=10; ++i)
	{
		cSum-=i;
	}
	printf("Subtracted: %d %d %d\n",iSum,(int)sSum,(int)cSum);

	return 0;
}
