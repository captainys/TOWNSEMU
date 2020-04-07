#include <stdio.h>

int main(void)
{
	printf("{\n");
	for(unsigned int MODR_M=0; MODR_M<256; ++MODR_M)
	{
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;
		if(0b00==MOD && 0b110==R_M) // disp16             CASE 0
		{
			printf("2,");
		}
		else if(0b01==MOD)          //                    CASE 1
		{
			printf("1,");
		}
		else if(0b10==MOD)          //                    CASE 0
		{
			printf("2,");
		}
		else
		{
			printf("0,");
		}
		if(31==(MODR_M&31))
		{
			printf("\n");
		}
	}
	printf("}\n");
	return 0;
}

