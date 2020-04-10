/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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

