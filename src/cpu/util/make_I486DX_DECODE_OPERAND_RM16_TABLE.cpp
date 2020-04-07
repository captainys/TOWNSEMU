#include <stdio.h>

int main(void)
{
	printf("{\n");
	for(unsigned int MODR_M=0; MODR_M<256; ++MODR_M)
	{
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;

		int caseNum=0;

		if(0b00==MOD && 0b110==R_M)                                     // CASE 0
		{
			caseNum=0;
		}
		else if(0b11!=MOD) // <=> if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			if(0b01==MOD)                                             // CASE 1
			{
				caseNum=1;
			}
			else if(0b10==MOD)                                        // CASE 2
			{
				caseNum=2;
			}
			else                                                      // CASE 3
			{
				caseNum=3;
			}
		}
		else                                                          // CASE 4
		{
			caseNum=4;
		}

		printf("%d,",caseNum);

		if(31==(MODR_M&31))
		{
			printf("\n");
		}
	}
	printf("}\n");
	return 0;
}

