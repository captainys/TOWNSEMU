#include <time.h>
#include <stdio.h>
#include "vmif.h"

int main(void)
{
	for(;;)
	{
		TEST_DEBUG_BREAK();
		time_t t=time(NULL);
		struct tm *tm=localtime(&t);
		printf("%u %d\n",(unsigned int)t,tm->tm_sec);
	}
	return 0;
}
