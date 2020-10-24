#include <stdio.h>

extern int MOVEAXDS(int EAX);

int main(void)
{
	printf("%08x\n",MOVEAXDS(0x00010000));
	printf("%08x\n",MOVEAXDS(0x00FF0000));
	printf("%08x\n",MOVEAXDS(0xFF000000));
	return 0;
}
