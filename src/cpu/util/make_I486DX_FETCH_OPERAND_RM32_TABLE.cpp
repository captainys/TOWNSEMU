#include <stdio.h>

int main(void)
{
	printf("{\n");
	for(unsigned int MODR_M=0; MODR_M<256; ++MODR_M)
	{
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;
		if(0b00==MOD)
		{
			if(0b100==R_M) // SIB                         // CASE 1
			{
				printf("1,");
			}
			else if(0b101==R_M) // disp32                    CASE 2
			{
				printf("2,");
			}
			else
			{
				printf("0,");
			}
		}
		else if(0b01==MOD)
		{
			if(0b100==R_M) // SIB+disp8                      CASE 3
			{
				printf("3,");
			}
			else                                          // CASE 4
			{
				printf("4,");
			}
		}
		else if(0b10==MOD)
		{
			if(0b100==R_M) // SIB+disp32                     CASE 5
			{
				printf("5,");
			}
			else                                          // CASE 6
			{
				printf("6,");
			}
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

