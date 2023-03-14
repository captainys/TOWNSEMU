#include <stdio.h>

extern void TEST_LAR_LSL(
     unsigned int res[4],int opSize,unsigned int descType,unsigned int GFlag,unsigned int Pflag,unsigned int RPL,unsigned int DPL);



int main(void)
{
	unsigned int res[4];
	TEST_LAR_LSL(res,32,0x1F,1,1,2,2);
	return 0;
}