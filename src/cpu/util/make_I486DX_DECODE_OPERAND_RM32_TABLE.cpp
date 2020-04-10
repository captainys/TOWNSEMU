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

