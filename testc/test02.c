#include <stdio.h>

char buf[256];

int main(void)
{
	int i;
	printf("TESTING: DATA Segment\n");
	for(i=0; i<96; ++i)
	{
		buf[i]=' '+i;
	}
	buf[96]=0;
	printf("%s\n",buf);
	return 0;
}
