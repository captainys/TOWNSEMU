#include <stdio.h>

int main(void)
{
	printf("{\n");
	for(unsigned int MODR_M=0; MODR_M<256; ++MODR_M)
	{
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;

		int caseNum=0;

		printf("%d,",caseNum);

		if(31==(MODR_M&31))
		{
			printf("\n");
		}
	}
	printf("}\n");
	return 0;
}

