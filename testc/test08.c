/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <stdio.h>
#include "io.h"

extern TEST_FAILED(void);

/*
00000000 01 FF 42 4F 4F 54 A8 00 40 00 01 FE 53 45 54 55|  BOOT  @   SETU
00000010 E8 00 00 01 01 FD 4C 4F 47 20 E8 01 10 03 01 FC|      LOG
00000020 4F 41 53 59 F8 04 20 00 01 FB 58 45 4E 49 18 05|OASY      XENI
00000030 10 00 01 FA 54 4F 57 4E 28 05 00 01 00 00 00 00|    TOWN(
00000040 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
00000050 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
00000060 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
00000070 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
00000080 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
00000090 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
000000A0 28 06 99 01 B5 7F 79 41 FF FF FF FF FF FF 00 00|(    yA
000000B0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00|
000000C0 00 00 00 01 00 3F 00 08 00 00 00 00 12 41 45 0F|     ?       AE
000000D0 86 0F 01 00 FE 05 FF FF 00 00 00 00 00 00 00 00|
000000E0 00 00 00 00 00 00 00 00 95 78 8E 6D 92 CA 00 00|         x m
000000F0 00 01 05 00 FF FF FF FF FF FF FF FF FF FF FF FF|
*/

unsigned char ReadCMOS(unsigned int offset)
{
	return IOReadByte(0x3000+offset*2);
}

int main(void)
{
	printf("Testing first 6 bytes.\n");
	{
		unsigned char first6Bytes[]=
		{
			0x01,0xFF,0x42,0x4F,0x4F,0x54,
		};
		for(unsigned int i=0; i<6; ++i)
		{
			if(first6Bytes[i]!=ReadCMOS(i))
			{
				printf("Error in CMOS! (Offset=%xH)\n",i);
				TEST_FAILED();
				return 1;
			}
		}
	}
	printf("Testing memory-test bits.\n");
	{
		// Memory-Test Results from 1MB+ RAM space
		// Assuming 4MB main RAM
		for(unsigned int i=0xA8; i<0xAE; ++i)
		{
			if(ReadCMOS(i)!=0xFF)
			{
				printf("Error in CMOS! (Offset=%xH)\n",i);
				TEST_FAILED();
				return 1;
			}
		}
	}
	/* FM TOWNS 2F Boot ROM doesn't seem to write to these two bytes.
	printf("Testing RAM availability byte.\n");
	{
		unsigned int memAvail=ReadCMOS(0xA8+0x1D);
		// (memAvailable+1)*64K bytes must be the size of the main RAM.
		// Assuming 4MB main RAM
		unsigned int RAMinKB=(memAvail+1)*64;
		if(4096!=RAMinKB)
		{
			printf("BIOS not detecting the main RAM size correctly.\n");
			TEST_FAILED();
			return 1;
		}
	} */
	printf("Dump\n");
	{
		for(unsigned int i=0; i<256; ++i)
		{
			printf("%02x ",ReadCMOS(i));
			if(15==i%16)
			{
				printf("\n");
			}
		}
	}

	return 0;
}
