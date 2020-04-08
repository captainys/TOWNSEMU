#include <stdio.h>

int main(void)
{
	printf("{\n");
	for(unsigned int MODR_M=0; MODR_M<256; ++MODR_M)
	{
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;

		int caseNum=0;

		if(0b00==MOD && 0b101==R_M)                                     // CASE 1
		{
			caseNum=1;
		}
		else if(0b11!=MOD) // <=> if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			if(0b100==R_M) // Depends on SIB                               CASE 2
			{
				caseNum=2;
			}
			else
			{
				if(0b01==MOD) // 8-bit offset                              CASE 3
				{
					caseNum=3;
				}
				else if(0b10==MOD) // 32-bit offset                        CASE 4
				{
					caseNum=4;
				}
				else                                                    // CASE 5
				{
					caseNum=5;
				}
			}
		}
		else if(0b11==MOD)                                              // CASE 6
		{
			caseNum=6;
		}
		// else                                                         // CASE 0

		printf("%d,",caseNum);

		if(31==(MODR_M&31))
		{
			printf("\n");
		}
	}
	printf("}\n");
	return 0;
}

