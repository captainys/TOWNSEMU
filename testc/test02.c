#include <stdio.h>
#include <memory.h>

char buf[256];
char dst[256];

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

	for(i=0; i<256; ++i)
	{
		dst[i]=0;
	}

	for(i='A'; i<='Z'; ++i)
	{
		dst[i-'A']=i;
	}
	dst['Z'-'A'+1]=0;
	memcpy(dst,dst,'Z'-'A'+1);
	printf("%s\n",dst);

	return 0;
}
